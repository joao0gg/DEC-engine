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

    auto [injection, ground] = zExtremeVertices(surface);

    Laplacian lap = tuftedLaplacian(surface);

    // one grounded vertex
    DirichletBC bc(surface.numVertices());
    bc.constrain(ground, 0.0);

    // unit current
    Form0 load(surface.numVertices());
    load[injection] = 1.0;

    ConstrainedSystem sys(lap.L, bc);
    DirectSolver solver(sys.matrix());

    Form0 u = sys.scatter(solver.solve(sys.reduce(load)));

    Eigen::VectorXd residual = lap.L * u.data() - load.data();
    residual[ground]         = 0.0;

    // clang-format off
    log("Cable (tufted):\n"
        "  vertices:      {}\n"
        "  edges:           {}\n"
        "  faces:         {}\n"
        "  free dofs:     {}\n"
        "  L nnz:         {}\n"
        "  residual:      {:.6e}\n"
        "  u range:       [{:.6e}, {:.6e}]\n",
        surface.numVertices(), surface.numEdges(), surface.numFaces(), 
        sys.numFree(),
        lap.L.nonZeros(), 
        residual.norm() / load.norm(), 
        u.data().minCoeff(), 
        u.data().maxCoeff());
    // clang-format on

    polyscope::init();

    polyscope::SurfaceMesh *psMesh = registerSurface(surface);

    std::vector<glm::vec3> points;
    for (size_t i : {injection, ground}) {
      geometrycentral::Vector3 p = surface.geometry().inputVertexPositions[i];
      points.emplace_back(p.x, p.y, p.z);
    }

    polyscope::PointCloud *psPoints = polyscope::registerPointCloud("injection / ground", points);
    psPoints->setPointRadius(0.005, true);
    psPoints->addScalarQuantity("role", std::vector<double>{1.0, -1.0})
        ->setColorMap("jet")
        ->setEnabled(true);

    psMesh->addVertexScalarQuantity("load", load.data())->setColorMap("jet");
    psMesh->addVertexScalarQuantity("potential", u.data())->setColorMap("jet")->setEnabled(true);

    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}