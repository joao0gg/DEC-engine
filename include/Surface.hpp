#pragma once

#include <memory>
#include <string>

#include "geometrycentral/surface/surface_mesh.h"
#include "geometrycentral/surface/vertex_position_geometry.h"

class Surface {
 public:
  std::string name;

  Surface(std::unique_ptr<geometrycentral::surface::SurfaceMesh> mesh,
          std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> geom,
          const std::string &name = "");

  Surface(Surface &&)                 = default;
  Surface &operator=(Surface &&)      = default;
  Surface(const Surface &)            = delete;
  Surface &operator=(const Surface &) = delete;

  static Surface load(const std::string &path, const std::string &name = "");

  geometrycentral::surface::SurfaceMesh &
  mesh() {
    return *mesh_;
  }
  const geometrycentral::surface::SurfaceMesh &
  mesh() const {
    return *mesh_;
  }

  geometrycentral::surface::VertexPositionGeometry &
  geometry() {
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

  bool
  isManifold() const {
    return mesh_->isManifold();
  }

 private:
  std::unique_ptr<geometrycentral::surface::SurfaceMesh> mesh_;
  std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> geom_;
};