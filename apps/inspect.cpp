#include "Laplacian.hpp"
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

    const long long euler_characteristic = static_cast<long long>(surface.numVertices()) -
                                           static_cast<long long>(surface.numEdges()) +
                                           static_cast<long long>(surface.numFaces());

    // compute sanity checks
    Eigen::SparseMatrix<double> dd   = surface.d1() * surface.d0();
    Eigen::SparseMatrix<double> asym = Eigen::SparseMatrix<double>(lap.L.transpose()) - lap.L;
    Eigen::VectorXd ones             = Eigen::VectorXd::Ones(surface.numVertices());

    Eigen::VectorXd h1 = surface.hodge1().diagonal();

    // clang-format off
    log("Mesh: {}\n"
        "  nVertices:    {}\n"
        "  nEdges:       {}\n"
        "  nFaces:       {}\n"
        "  components:   {}\n"
        "  boundary:     {}\n"
        "  manifold:     {}\n"
        "  oriented:     {}\n"
        "  triangular:   {}\n"
        "  euler charac: {}\n"
        "  |d1 * d0|:    {:.6e}\n"
        "  |L - L^T|:    {:.6e}\n"
        "  |L * 1|:      {:.6e}\n"
        "  total area:   {:.6e}\n"
        "  hodge1 min:   {:.6e}  ({} negative of {})\n",
        surface.name(), 
        surface.numVertices(), surface.numEdges(), surface.numFaces(),
        surface.mesh().nConnectedComponents(),
        YESNO(surface.mesh().hasBoundary()),
        YESNO(surface.isManifold()), YESNO(surface.isOriented()), YESNO(surface.isTriangular()),
        euler_characteristic, 
        dd.norm(), 
        asym.norm(), 
        (lap.L * ones).norm(), 
        lap.M.diagonal().sum(), 
        h1.minCoeff(),
        (h1.array() < 0.0).count(), 
        h1.size());
    // clang-format on

    polyscope::init();

    polyscope::SurfaceMesh *psMesh = registerSurface(surface);

    psMesh->addVertexScalarQuantity("dual area", lap.M.diagonal())
        ->setColorMap("jet")
        ->setEnabled(true);

    polyscope::show();
  } catch (const std::exception &e) {
    log("error: {}", e.what());
    return 1;
  }

  return 0;
}