#pragma once

#include <cstddef>
#include <stdexcept>

#include <Eigen/Dense>

enum class Duality { Primal, Dual };

template <int K, Duality D = Duality::Primal, int C = 1>
class Form {
 public:
  static constexpr int degree      = K;
  static constexpr Duality duality = D;
  static constexpr int components  = C;

  static_assert(C >= 1, "Form: component count must be positive");

  using Storage = Eigen::Matrix<double, Eigen::Dynamic, C>;

  Form() = default;

  explicit Form(size_t n) : data_(Storage::Zero(n, C)) {}

  Form(size_t n, double value) : data_(Storage::Constant(n, C, value)) {}

  explicit Form(Storage data) : data_(std::move(data)) {}

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
    return Form(Storage::Random(n, C));
  }

  size_t
  size() const {
    return static_cast<size_t>(data_.rows());
  }

  Storage &
  data() {
    return data_;
  }

  const Storage &
  data() const {
    return data_;
  }

  double &
  operator()(size_t i, int c) {
    return data_(i, c);
  }

  double
  operator()(size_t i, int c) const {
    return data_(i, c);
  }

  double &
  operator[](size_t i)
    requires(C == 1)
  {
    return data_[i];
  }

  double
  operator[](size_t i) const
    requires(C == 1)
  {
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
    return Form(Storage(-data_));
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
    return (data_.array() * other.data_.array()).sum();
  }

 private:
  void
  checkSize(const Form &other) const {
    if (data_.rows() != other.data_.rows()) {
      throw std::invalid_argument("Form: size mismatch");
    }
  }

  Storage data_;
};

template <int K, Duality D, int C>
Form<K, D, C>
operator+(Form<K, D, C> a, const Form<K, D, C> &b) {
  a += b;
  return a;
}

template <int K, Duality D, int C>
Form<K, D, C>
operator-(Form<K, D, C> a, const Form<K, D, C> &b) {
  a -= b;
  return a;
}

template <int K, Duality D, int C>
Form<K, D, C>
operator*(Form<K, D, C> a, double s) {
  a *= s;
  return a;
}

template <int K, Duality D, int C>
Form<K, D, C>
operator*(double s, Form<K, D, C> a) {
  a *= s;
  return a;
}

template <int K, Duality D, int C>
Form<K, D, C>
operator/(Form<K, D, C> a, double s) {
  a /= s;
  return a;
}

// aliases

using Form0 = Form<0, Duality::Primal, 1>;
using Form1 = Form<1, Duality::Primal, 1>;
using Form2 = Form<2, Duality::Primal, 1>;

using DualForm0 = Form<0, Duality::Dual, 1>;
using DualForm1 = Form<1, Duality::Dual, 1>;
using DualForm2 = Form<2, Duality::Dual, 1>;

using VectorForm0 = Form<0, Duality::Primal, 3>;
using VectorForm1 = Form<1, Duality::Primal, 3>;
using VectorForm2 = Form<2, Duality::Primal, 3>;