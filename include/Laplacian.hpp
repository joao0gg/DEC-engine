#pragma once

#include <Eigen/Sparse>

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