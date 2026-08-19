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

    Laplacian lap = laplacian0(surface);

    auto [source, sink] = zExtremeVertices(surface);

    // find a vertex that is not the source or sink to use as a Dirichlet boundary condition
    int dirichlet = 0;
    while (dirichlet == static_cast<int>(source) || dirichlet == static_cast<int>(sink)) {
      ++dirichlet;
    }

    // setup linear system
    Eigen::SparseMatrix<double> A = lap.L;
    for (int k = 0; k < A.outerSize(); ++k) {
      for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
        if (it.row() == dirichlet || it.col() == dirichlet) {
          it.valueRef() = 0.0;
        }
      }
    }
    A.coeffRef(dirichlet, dirichlet) = 1.0;
    A.prune(0.0);

    Form0 b(surface.numVertices());
    b[source]    = 1.0;
    b[sink]      = -1.0;
    b[dirichlet] = 0.0;

    DirectSolver direct(A);
    IterativeSolver iterative(A);

    Form0 ud = direct.solve(b);
    Form0 ui = iterative.solve(b);

    Eigen::VectorXd udView = ud.data().array() - ud.data().mean();
    Eigen::VectorXd uiView = ui.data().array() - ui.data().mean();

    log("Poisson (dipole):\n"
        "  direct residual:     {:.6e}\n"
        "  iterative residual:  {:.6e}  ({} iterations)\n"
        "  |ud - ui| / |ud|:    {:.6e}\n"
        "  u range:             [{:.6e}, {:.6e}]\n",
        (A * ud.data() - b.data()).norm() / b.data().norm(),
        (A * ui.data() - b.data()).norm() / b.data().norm(), iterative.iterations(),
        (ud.data() - ui.data()).norm() / ud.data().norm(), udView.minCoeff(), udView.maxCoeff());

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

    psMesh->addVertexScalarQuantity("potential (iterative)", uiView)->setColorMap("jet");
    psMesh->addVertexScalarQuantity("difference", (udView - uiView).eval())->setColorMap("jet");
    psMesh->addVertexScalarQuantity("potential (direct)", udView)->setColorMap("jet");

    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}