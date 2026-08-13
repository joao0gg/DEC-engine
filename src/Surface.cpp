#include "Surface.hpp"

#include <filesystem>
#include <stdexcept>
#include <utility>

#include "geometrycentral/surface/meshio.h"

using namespace geometrycentral::surface;

Surface::Surface(std::unique_ptr<SurfaceMesh> mesh, std::unique_ptr<VertexPositionGeometry> geom,
                 std::string name)
    : name_(std::move(name)), mesh_(std::move(mesh)), geom_(std::move(geom)) {
  if (!mesh_) {
    throw std::invalid_argument("Surface: null mesh");
  }
  if (!geom_) {
    throw std::invalid_argument("Surface: null geometry");
  }
}

Surface
Surface::load(const std::string &path, std::string name) {
  namespace fs = std::filesystem;

  if (!fs::exists(path)) {
    throw std::runtime_error("Surface::load: no such file: " + path);
  }

  std::unique_ptr<SurfaceMesh> mesh;
  std::unique_ptr<VertexPositionGeometry> geom;

  try {
    std::tie(mesh, geom) = readSurfaceMesh(path);
  } catch (const std::exception &e) {
    throw std::runtime_error("Surface::load: failed to read " + path + ": " + e.what());
  }

  if (!mesh || mesh->nVertices() == 0 || mesh->nFaces() == 0) {
    throw std::runtime_error("Surface::load: empty mesh: " + path);
  }

  mesh->compress();

  if (name.empty()) {
    name = fs::path(path).stem().string();
  }

  return Surface(std::move(mesh), std::move(geom), std::move(name));
}
