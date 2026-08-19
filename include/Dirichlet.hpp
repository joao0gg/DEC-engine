#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include <Eigen/Sparse>

#include "Form.hpp"

class DirichletBC {
 public:
  explicit DirichletBC(size_t n) : constrained_(n, false), values_(n, 0.0) {}

  void
  constrain(size_t i, double value) {
    constrained_[i] = true;
    values_[i]      = value;
  }

  size_t
  size() const {
    return constrained_.size();
  }

  bool
  isConstrained(size_t i) const {
    return constrained_[i];
  }

  double
  value(size_t i) const {
    return values_[i];
  }

 private:
  std::vector<bool> constrained_;
  std::vector<double> values_;
};

// Eliminates constrained degrees of freedom, producing a smaller nonsingular
// system. Constrained values move to the right-hand side.
class ConstrainedSystem {
 public:
  ConstrainedSystem(const Eigen::SparseMatrix<double> &A, const DirichletBC &bc) : bc_(bc) {
    const size_t n = bc.size();

    if (static_cast<size_t>(A.rows()) != n) {
      throw std::invalid_argument("ConstrainedSystem: size mismatch");
    }

    index_.assign(n, -1);
    for (size_t i = 0; i < n; ++i) {
      if (!bc.isConstrained(i)) {
        index_[i] = numFree_++;
      }
    }

    std::vector<Eigen::Triplet<double>> triplets;
    shift_ = Eigen::VectorXd::Zero(numFree_);

    for (int k = 0; k < A.outerSize(); ++k) {
      for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
        const size_t row = it.row(), col = it.col();
        if (bc.isConstrained(row)) {
          continue;
        }
        if (bc.isConstrained(col)) {
          shift_[index_[row]] -= it.value() * bc.value(col);
        } else {
          triplets.emplace_back(index_[row], index_[col], it.value());
        }
      }
    }

    reduced_.resize(numFree_, numFree_);
    reduced_.setFromTriplets(triplets.begin(), triplets.end());
  }

  const Eigen::SparseMatrix<double> &
  matrix() const {
    return reduced_;
  }

  Form0
  reduce(const Form0 &load) const {
    Eigen::VectorXd b = shift_;
    for (size_t i = 0; i < bc_.size(); ++i) {
      if (!bc_.isConstrained(i)) {
        b[index_[i]] += load[i];
      }
    }
    return Form0(std::move(b));
  }

  Form0
  scatter(const Form0 &x) const {
    Form0 u(bc_.size());
    for (size_t i = 0; i < bc_.size(); ++i) {
      u[i] = bc_.isConstrained(i) ? bc_.value(i) : x[index_[i]];
    }
    return u;
  }

  size_t
  numFree() const {
    return static_cast<size_t>(numFree_);
  }

 private:
  const DirichletBC &bc_;
  Eigen::SparseMatrix<double> reduced_;
  Eigen::VectorXd shift_;
  std::vector<int> index_;
  int numFree_ = 0;
};