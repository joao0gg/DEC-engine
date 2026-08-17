#pragma once

#include "Form.hpp"
#include "Surface.hpp"

// exterior derivative

template <int C>
Form<1, Duality::Primal, C>
d(const Surface &s, const Form<0, Duality::Primal, C> &alpha) {
  return Form<1, Duality::Primal, C>(s.d0() * alpha.data());
}

template <int C>
Form<2, Duality::Primal, C>
d(const Surface &s, const Form<1, Duality::Primal, C> &alpha) {
  return Form<2, Duality::Primal, C>(s.d1() * alpha.data());
}

// hodge star

template <int C>
Form<2, Duality::Dual, C>
star(const Surface &s, const Form<0, Duality::Primal, C> &alpha) {
  return Form<2, Duality::Dual, C>(s.hodge0() * alpha.data());
}

template <int C>
Form<1, Duality::Dual, C>
star(const Surface &s, const Form<1, Duality::Primal, C> &alpha) {
  return Form<1, Duality::Dual, C>(s.hodge1() * alpha.data());
}

template <int C>
Form<0, Duality::Dual, C>
star(const Surface &s, const Form<2, Duality::Primal, C> &alpha) {
  return Form<0, Duality::Dual, C>(s.hodge2() * alpha.data());
}

// L2 inner product of primal K-forms

template <int K, int C>
double
inner(const Surface &s, const Form<K, Duality::Primal, C> &a,
      const Form<K, Duality::Primal, C> &b) {
  static_assert(K >= 0 && K <= 2, "inner: degree must be in [0,2]");

  const Eigen::SparseMatrix<double> *h = nullptr;
  if constexpr (K == 0) {
    h = &s.hodge0();
  } else if constexpr (K == 1) {
    h = &s.hodge1();
  } else {
    h = &s.hodge2();
  }

  return (a.data().array() * ((*h) * b.data()).array()).sum();
}