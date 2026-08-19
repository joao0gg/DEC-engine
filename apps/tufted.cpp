#include "Dirichlet.hpp"
#include "Form.hpp"
#include "Laplacian.hpp"
#include "LinearSolver.hpp"
#include "common.hpp"

#include <exception>

int
main(int argc, char const *argv[]) {
  try {
    if (argc != 2) {
      fatal("Usage: {} <mesh>", argv[0]);
    }

    Surface surface = Surface::load(argv[1]);

    auto [source, sink] = zExtremeVertices(surface);

    Laplacian lap = tuftedLaplacian(surface);

    // hot and cold ends held at fixed potential
    DirichletBC bc(surface.numVertices());
    bc.constrain(source, 1.0);
    bc.constrain(sink, -1.0);

    ConstrainedSystem sys(lap.L, bc);
    Form0 load(surface.numVertices());

    DirectSolver solver(sys.matrix());
    Form0 u = sys.scatter(solver.solve(sys.reduce(load)));

    log("Laplace (tufted):\n"
        "  vertices:      {}\n"
        "  free dofs:     {}\n"
        "  L nnz:         {}\n"
        "  u range:       [{:.6e}, {:.6e}]\n",
        surface.numVertices(), sys.numFree(), lap.L.nonZeros(), u.data().minCoeff(),
        u.data().maxCoeff());

    polyscope::init();

    polyscope::SurfaceMesh *psMesh = registerSurface(surface);

    std::vector<glm::vec3> points;
    for (size_t i : {source, sink}) {
      geometrycentral::Vector3 p = surface.geometry().inputVertexPositions[i];
      points.emplace_back(p.x, p.y, p.z);
    }

    polyscope::PointCloud *psPoints = polyscope::registerPointCloud("source / sink", points);
    psPoints->setPointRadius(0.005, true);
    psPoints->addScalarQuantity("sign", std::vector<double>{1.0, -1.0})
        ->setColorMap("jet")
        ->setEnabled(true);

    psMesh->addVertexScalarQuantity("potential", u.data())->setColorMap("jet")->setEnabled(true);

    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}