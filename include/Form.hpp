#pragma once

#include <cstddef>
#include <stdexcept>

#include <Eigen/Dense>

enum class Duality { Primal, Dual };

template <int K, Duality D = Duality::Primal>
class Form {
 public:
  static constexpr int degree      = K;
  static constexpr Duality duality = D;

  Form() = default;

  explicit Form(size_t n) : data_(Eigen::VectorXd::Zero(n)) {}

  Form(size_t n, double value) : data_(Eigen::VectorXd::Constant(n, value)) {}

  explicit Form(Eigen::VectorXd data) : data_(std::move(data)) {}

  static Form
  zeros(size_t n) {
    return Form(n);
  }

  static Form
  ones(size_t n) {
    return Form(n, 1.0);
  }

  static Form
  random(size_t n) {
    return Form(Eigen::VectorXd::Random(n));
  }

  size_t
  size() const {
    return static_cast<size_t>(data_.size());
  }

  Eigen::VectorXd &
  data() {
    return data_;
  }

  const Eigen::VectorXd &
  data() const {
    return data_;
  }

  double &
  operator[](size_t i) {
    return data_[i];
  }

  double
  operator[](size_t i) const {
    return data_[i];
  }

  // arithmetic

  Form &
  operator+=(const Form &other) {
    checkSize(other);
    data_ += other.data_;
    return *this;
  }
  Form &
  operator-=(const Form &other) {
    checkSize(other);
    data_ -= other.data_;
    return *this;
  }
  Form &
  operator*=(double s) {
    data_ *= s;
    return *this;
  }
  Form &
  operator/=(double s) {
    data_ /= s;
    return *this;
  }
  Form
  operator-() const {
    return Form(Eigen::VectorXd(-data_));
  }

  double
  norm() const {
    return data_.norm();
  }

  double
  maxAbs() const {
    return data_.cwiseAbs().maxCoeff();
  }

  double
  dot(const Form &other) const {
    checkSize(other);
    return data_.dot(other.data_);
  }

 private:
  void
  checkSize(const Form &other) const {
    if (data_.size() != other.data_.size()) {
      throw std::invalid_argument("Form: size mismatch");
    }
  }

  Eigen::VectorXd data_;
};

template <int K, Duality D>
Form<K, D>
operator+(Form<K, D> a, const Form<K, D> &b) {
  a += b;
  return a;
}

template <int K, Duality D>
Form<K, D>
operator-(Form<K, D> a, const Form<K, D> &b) {
  a -= b;
  return a;
}

template <int K, Duality D>
Form<K, D>
operator*(Form<K, D> a, double s) {
  a *= s;
  return a;
}

template <int K, Duality D>
Form<K, D>
operator*(double s, Form<K, D> a) {
  a *= s;
  return a;
}

template <int K, Duality D>
Form<K, D>
operator/(Form<K, D> a, double s) {
  a /= s;
  return a;
}

using Form0 = Form<0, Duality::Primal>;
using Form1 = Form<1, Duality::Primal>;
using Form2 = Form<2, Duality::Primal>;

using DualForm0 = Form<0, Duality::Dual>;
using DualForm1 = Form<1, Duality::Dual>;
using DualForm2 = Form<2, Duality::Dual>;
