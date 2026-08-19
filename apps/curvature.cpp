#include "Form.hpp"
#include "Laplacian.hpp"
#include "Surface.hpp"
#include "log.hpp"

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

#include <algorithm>
#include <exception>

#define DEFAULT_SURFACE_COLOR {0.91f, 0.85f, 0.77f}

int
main(int argc, char const *argv[]) {
  try {
    if (argc != 2) {
      fatal("Usage: {} <mesh>", argv[0]);
    }

    Surface surface = Surface::load(argv[1]);
    Laplacian lap   = laplacian0(surface);

    // vertex positions as a vector-valued 0-form
    VectorForm0 X(surface.numVertices());
    for (size_t i = 0; i < surface.numVertices(); ++i) {
      geometrycentral::Vector3 p = surface.geometry().inputVertexPositions[i];
      X(i, 0)                    = p.x;
      X(i, 1)                    = p.y;
      X(i, 2)                    = p.z;
    }

    Eigen::MatrixXd HN = lap.L * X.data();

    surface.geometry().requireVertexNormals();

    // compute mean curvature
    Eigen::VectorXd H(surface.numVertices());
    for (size_t i = 0; i < surface.numVertices(); ++i) {
      geometrycentral::Vector3 n = surface.geometry().vertexNormals[i];
      H[i] = (HN(i, 0) * n.x + HN(i, 1) * n.y + HN(i, 2) * n.z) / (2.0 * lap.M.diagonal()[i]);
    }

    Eigen::VectorXd sorted = H;
    std::sort(sorted.data(), sorted.data() + sorted.size());
    const double p01 = sorted[static_cast<Eigen::Index>(0.01 * (sorted.size() - 1))];
    const double p50 = sorted[static_cast<Eigen::Index>(0.50 * (sorted.size() - 1))];
    const double p99 = sorted[static_cast<Eigen::Index>(0.99 * (sorted.size() - 1))];

    log("Mean curvature:\n"
        "  p01:  {:.6e}\n"
        "  p50:  {:.6e}\n"
        "  p99:  {:.6e}\n"
        "  min:  {:.6e}\n"
        "  max:  {:.6e}\n",
        p01, p50, p99, H.minCoeff(), H.maxCoeff());

    polyscope::init();

    polyscope::SurfaceMesh *psMesh = polyscope::registerSurfaceMesh(
        surface.name(), surface.geometry().inputVertexPositions,
        surface.mesh().getFaceVertexList());
    psMesh->setSurfaceColor(DEFAULT_SURFACE_COLOR);

    auto *q = psMesh->addVertexScalarQuantity("mean curvature", H);
    q->setColorMap("jet");
    q->setMapRange({p01, p99});
    q->setEnabled(true);

    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}