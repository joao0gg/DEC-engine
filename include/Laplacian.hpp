#pragma once

#include <Eigen/Sparse>

#include "Surface.hpp"

#include "geometrycentral/surface/tufted_laplacian.h"

struct Laplacian {
  Eigen::SparseMatrix<double> M;
  Eigen::SparseMatrix<double> L;
};

template <typename Complex>
Laplacian
laplacian0(const Complex &c) {
  Eigen::SparseMatrix<double> L = c.d0().transpose() * c.hodge1() * c.d0();
  return {c.hodge0(), std::move(L)};
}

inline Laplacian
tuftedLaplacian(const Surface &s, double mollification = 1e-6) {
  auto [L, M] = geometrycentral::surface::buildTuftedLaplacian(s.mesh(), s.geometry(),
                                                               mollification);
  return {std::move(M), std::move(L)};
}