#pragma once

#include <memory>
#include <stdexcept>

#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

#include "Form.hpp"

class LinearSolver {
 public:
  virtual ~LinearSolver() = default;

  virtual void compute(const Eigen::SparseMatrix<double> &A) = 0;

  virtual Eigen::MatrixXd solveRaw(const Eigen::MatrixXd &B) const = 0;

  virtual bool ready() const = 0;

  virtual size_t size() const = 0;

  template <int K, Duality D, int C>
  Form<K, D, C>
  solve(const Form<K, D, C> &b) const {
    if (!ready()) {
      throw std::runtime_error("LinearSolver: no matrix");
    }
    if (b.size() != size()) {
      throw std::invalid_argument("LinearSolver: size mismatch");
    }

    return Form<K, D, C>(typename Form<K, D, C>::Storage(solveRaw(b.data())));
  }
};

class DirectSolver : public LinearSolver {
 public:
  DirectSolver() = default;

  explicit DirectSolver(const Eigen::SparseMatrix<double> &A) { compute(A); }

  void
  compute(const Eigen::SparseMatrix<double> &A) override {
    if (A.rows() != A.cols()) {
      throw std::invalid_argument("DirectSolver: matrix is not square");
    }

    ready_ = false;
    n_     = static_cast<size_t>(A.rows());

    ldlt_.compute(A);
    if (ldlt_.info() == Eigen::Success) {
      symmetric_ = true;
      ready_     = true;
      return;
    }

    lu_.compute(A);
    if (lu_.info() == Eigen::Success) {
      symmetric_ = false;
      ready_     = true;
      return;
    }

    throw std::runtime_error("DirectSolver: matrix is singular");
  }

  Eigen::MatrixXd
  solveRaw(const Eigen::MatrixXd &B) const override {
    if (symmetric_) {
      return ldlt_.solve(B);
    }
    return lu_.solve(B);
  }

  bool
  ready() const override {
    return ready_;
  }

  size_t
  size() const override {
    return n_;
  }

 private:
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> ldlt_;
  Eigen::SparseLU<Eigen::SparseMatrix<double>> lu_;
  bool ready_     = false;
  bool symmetric_ = false;
  size_t n_       = 0;
};

class IterativeSolver : public LinearSolver {
 public:
  IterativeSolver() = default;

  explicit IterativeSolver(const Eigen::SparseMatrix<double> &A) { compute(A); }

  void
  compute(const Eigen::SparseMatrix<double> &A) override {
    if (A.rows() != A.cols()) {
      throw std::invalid_argument("IterativeSolver: matrix is not square");
    }

    n_ = static_cast<size_t>(A.rows());
    cg_.compute(A);
    ready_ = (cg_.info() == Eigen::Success);

    if (!ready_) {
      throw std::runtime_error("IterativeSolver: setup failed");
    }
  }

  Eigen::MatrixXd
  solveRaw(const Eigen::MatrixXd &B) const override {
    return cg_.solve(B);
  }

  bool
  ready() const override {
    return ready_;
  }

  size_t
  size() const override {
    return n_;
  }

  void
  setTolerance(double tol) {
    cg_.setTolerance(tol);
  }

  void
  setMaxIterations(int n) {
    cg_.setMaxIterations(n);
  }

  int
  iterations() const {
    return static_cast<int>(cg_.iterations());
  }

 private:
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper> cg_;
  bool ready_ = false;
  size_t n_   = 0;
};
