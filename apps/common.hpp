#pragma once

#include "Surface.hpp"
#include "log.hpp"

#include "polyscope/point_cloud.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

#include <limits>
#include <string>
#include <vector>

#define YESNO(b) ((b) ? "yes" : "no")

#define DEFAULT_SURFACE_COLOR {0.91f, 0.85f, 0.77f}

inline polyscope::SurfaceMesh *
registerSurface(const Surface &surface) {
  polyscope::SurfaceMesh *psMesh = polyscope::registerSurfaceMesh(
      surface.name(), surface.geometry().inputVertexPositions, surface.mesh().getFaceVertexList());
  psMesh->setSurfaceColor(DEFAULT_SURFACE_COLOR);

  return psMesh;
}

struct ZExtremes {
  size_t top;
  size_t bottom;
};

inline ZExtremes
zExtremeVertices(const Surface &surface) {
  size_t top = 0, bottom = 0;
  double zMin = std::numeric_limits<double>::infinity();
  double zMax = -std::numeric_limits<double>::infinity();

  for (size_t i = 0; i < surface.numVertices(); ++i) {
    double z = surface.geometry().inputVertexPositions[i].z;

    if (z < zMin) {
      zMin   = z;
      bottom = i;
    }
    if (z > zMax) {
      zMax = z;
      top  = i;
    }
  }

  return {top, bottom};
}
