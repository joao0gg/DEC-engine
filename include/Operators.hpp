#pragma once

#include "Form.hpp"
#include "Surface.hpp"

// exterior derivative

inline Form1
d(const Surface &s, const Form0 &alpha) {
  return Form1(s.d0() * alpha.data());
}

inline Form2
d(const Surface &s, const Form1 &alpha) {
  return Form2(s.d1() * alpha.data());
}

// hodge star

inline DualForm2
star(const Surface &s, const Form0 &alpha) {
  return DualForm2(s.hodge0() * alpha.data());
}

inline DualForm1
star(const Surface &s, const Form1 &alpha) {
  return DualForm1(s.hodge1() * alpha.data());
}

inline DualForm0
star(const Surface &s, const Form2 &alpha) {
  return DualForm0(s.hodge2() * alpha.data());
}

// L2 inner product of primal K-forms

inline double
inner(const Surface &s, const Form0 &a, const Form0 &b) {
  return a.data().dot(s.hodge0() * b.data());
}

inline double
inner(const Surface &s, const Form1 &a, const Form1 &b) {
  return a.data().dot(s.hodge1() * b.data());
}

inline double
inner(const Surface &s, const Form2 &a, const Form2 &b) {
  return a.data().dot(s.hodge2() * b.data());
}