#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "geometrycentral/surface/surface_mesh.h"
#include "geometrycentral/surface/vertex_position_geometry.h"

#include <Eigen/Sparse>

class Surface {
 public:
  Surface(std::unique_ptr<geometrycentral::surface::SurfaceMesh> mesh,
          std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> geom,
          std::string name = "");

  Surface(Surface &&)                 = default;
  Surface &operator=(Surface &&)      = default;
  Surface(const Surface &)            = delete;
  Surface &operator=(const Surface &) = delete;

  // reads .obj/.ply/.stl/.off
  static Surface load(const std::string &path, std::string name = "");

  const std::string &
  name() const {
    return name_;
  }

  geometrycentral::surface::SurfaceMesh &
  mesh() const {
    return *mesh_;
  }

  geometrycentral::surface::VertexPositionGeometry &
  geometry() const {
    return *geom_;
  }

  size_t
  numVertices() const {
    return mesh_->nVertices();
  }

  size_t
  numEdges() const {
    return mesh_->nEdges();
  }

  size_t
  numFaces() const {
    return mesh_->nFaces();
  }

  size_t
  numHalfedges() const {
    return mesh_->nHalfedges();
  }

  bool
  isManifold() const {
    return mesh_->isManifold();
  }

  bool
  isEdgeManifold() const {
    return mesh_->isEdgeManifold();
  }

  bool
  isOriented() const {
    return mesh_->isOriented();
  }

  bool
  isTriangular() const {
    return mesh_->isTriangular();
  }

  // DEC operators

  const Eigen::SparseMatrix<double> &
  d0() const {
    return geom_->d0;
  }

  const Eigen::SparseMatrix<double> &
  d1() const {
    return geom_->d1;
  }

  const Eigen::SparseMatrix<double> &
  hodge0() const {
    return geom_->hodge0;
  }

  const Eigen::SparseMatrix<double> &
  hodge1() const {
    return geom_->hodge1;
  }

  const Eigen::SparseMatrix<double> &
  hodge2() const {
    return geom_->hodge2;
  }

 private:
  std::string name_;
  mutable std::unique_ptr<geometrycentral::surface::SurfaceMesh> mesh_;
  mutable std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> geom_;
};