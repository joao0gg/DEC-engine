#include "Surface.hpp"
#include "log.hpp"

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

#include <exception>

using namespace geometrycentral;
using namespace geometrycentral::surface;

#define BOOL2YESNO(x) ((x) ? "yes" : "no")

int
main(int argc, char const *argv[]) {
  if (argc != 2) {
    fatal("Usage: {} <mesh>", argv[0]);
  }

  try {
    Surface surface = Surface::load(argv[1], "neuron");

    log("Surface:\n"
        "  name: {}\n"
        "  nVertices: {}\n"
        "  nEdges:    {}\n"
        "  nFaces:    {}\n"
        "  is manifold: {}\n",
        surface.name, surface.numVertices(), surface.numEdges(), surface.numFaces(),
        BOOL2YESNO(surface.isManifold()));

    polyscope::init();
    polyscope::SurfaceMesh *psMesh = polyscope::registerSurfaceMesh(
        surface.name, surface.geometry().vertexPositions, surface.mesh().getFaceVertexList());

    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}