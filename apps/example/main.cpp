#include "Surface.hpp"
#include "Form.hpp"
#include "operators.hpp"
#include "log.hpp"
#include "Laplacian.hpp"

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

#include <exception>

static constexpr const char *
yesno(bool b) {
  return b ? "yes" : "no";
}

int
main(int argc, char const *argv[]) {
  try {
    if (argc != 2) {
      fatal("Usage: {} <mesh>", argv[0]);
    }

    Surface surface = Surface::load(argv[1]);

    log("Surface:\n"
        "  name:        {}\n"
        "  nVertices:   {}\n"
        "  nEdges:      {}\n"
        "  nFaces:      {}\n"
        "  manifold:    {}\n"
        "  oriented:    {}\n"
        "  triangular:  {}\n",
        surface.name(), surface.numVertices(), surface.numEdges(), surface.numFaces(),
        yesno(surface.isManifold()), yesno(surface.isOriented()), yesno(surface.isTriangular()));

    polyscope::init();
    polyscope::registerSurfaceMesh(surface.name(), surface.geometry().inputVertexPositions,
                                   surface.mesh().getFaceVertexList());
    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}