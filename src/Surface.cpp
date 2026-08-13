#include "Surface.hpp"

#include <stdexcept>
#include <utility>

#include "geometrycentral/surface/meshio.h"

using namespace geometrycentral::surface;

Surface::Surface(std::unique_ptr<SurfaceMesh> mesh, std::unique_ptr<VertexPositionGeometry> geom,
                 const std::string &name)
    : name(name), mesh_(std::move(mesh)), geom_(std::move(geom)) {
  if (!mesh_) {
    throw std::invalid_argument("Surface: null mesh");
  }
  if (!geom_) {
    throw std::invalid_argument("Surface: null geometry");
  }
}

Surface
Surface::load(const std::string &path, const std::string &name) {
  auto [mesh, geom] = readSurfaceMesh(path);

  Surface surface(std::move(mesh), std::move(geom), name);

  surface.mesh_->compress();
  surface.geom_->refreshQuantities();

  return surface;
}