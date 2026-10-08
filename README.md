# C++ NEAT

A full, independent C++14 reimplementation of **NeuroEvolution of Augmenting
Topologies (NEAT)**. Its initial implementation was written by hand.

NEAT evolves neural-network structure and weights through mutation, crossover,
and speciation. The algorithm is described in Kenneth O. Stanley and Risto
Miikkulainen's [*Evolving Neural Networks through Augmenting Topologies*
(2002)](https://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf).

Supports recurrent networks, sigmoid/tanh/ReLU activation, custom fitness
functions with optional parallel evaluation, and SVG network export.

## Build

Requires CMake 3.16 or newer, a C++14 compiler, and POSIX threads. The library
and tests have been verified on Linux with GCC. The SVG helper is bundled.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Use `-DCMAKE_BUILD_TYPE=Debug` in a separate build directory to enable assertions.

## Use in your project

Place this repository at `external/cpp-neat` and link the static `neat` target:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/cpp-neat)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE neat)
```

Configure your application with `-DBUILD_TESTING=OFF` to omit the library's
tests, which are enabled by default. The `neat` target supplies its include
directory and thread dependency automatically.

This complete `main.cpp` evolves a network for XOR:

```cpp
#include "Population.h"
#include "Phenotype.h"
#include <iostream>
#include <vector>

int main() {
    NEAT::Opts opts;
    opts.populationSize(150);

    const std::vector<std::vector<double>> inputs =
        {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    const std::vector<std::vector<double>> outputs = {{0}, {1}, {1}, {0}};

    NEAT::Population population(inputs, outputs, opts);
    population.createInitialConnections();
    for (unsigned generation = 0; generation < 300; ++generation)
        population.calcNextGeneration();

    auto best = population.getBestOrganism();
    NEAT::Phenotype network(best->getGenome(), opts);
    network.initNet(0);
    std::cout << network.run({0, 1})[0] << '\n';
    network.saveAsSVG("network.svg");
}
```

Training-data fitness rewards lower squared error. Use nonempty input/output
datasets with matching sample counts and consistent row dimensions. Evolution
is stochastic; the example's generation budget does not guarantee a solution.

Configure [`Opts`](src/Opts.h) before constructing a population, which freezes
the settings. Keep options and the population alive while using its organisms
and networks; a `Phenotype` also references its source genotype.

Recurrent networks retain state between calls. Use `initNet(0)` to reset state
before evaluating an independent sample.

## Custom fitness

Implement [`NEAT::FitnessCalculator`](src/Organism.h) with
`double operator()(const NEAT::Genotype&)`, then construct
`NEAT::Population population(&fitness, opts)`. Higher fitness wins; every score
must be finite. Set `numInputNeurons`, `numOutputNeurons`, and `populationSize`
before construction; outputs and population size must be positive. Keep the
callback alive for the population's lifetime.

Set `opts.numThreads(n)` for parallel custom fitness evaluation (default: one
thread). The shared callback must support concurrent calls. Training-data
fitness is evaluated serially.

## Saving networks

The [genome serialization API](src/Genotype.h) saves genomes for inference.
It excludes `Opts`, so retain the corresponding network settings separately.
Keep options and `GeneHistory` alive while using a loaded genotype. The format
does not provide evolution checkpoints or guarantee compatibility across
versions.

## Tests and reproducibility

Tests cover components, regression cases, XOR, and recurrence. Run them serially
because some include timing checks:

```sh
(cd build && ctest -j1 --output-on-failure)
```

CTest uses a fixed seed; configure with `-DNEAT_TEST_SEED=42` to change it.
For application runs, set `NEAT_SEED` before starting the executable:

```sh
NEAT_SEED=42 ./my_app
```

`NEAT_SEED` accepts a nonzero unsigned 64-bit decimal value. Without it, the seed
comes from the current time. Repeatability applies to serial runs of the same
executable; parallel callback scheduling may vary.

## Project history

This repository was extracted from a larger project. Its history was filtered
to retain the C++ NEAT library, tests, build files, and supporting dependencies,
including their earlier locations. Original authorship and commit messages
are preserved, so some older entries refer to virtual creature simulations
and other experiments.

## License

Project code is licensed under the [MIT License](LICENSE).
The bundled [simple-svg helper](lib/simple_svg_1.0.0.hpp), copyright 2010
Mark Turney, retains its separate BSD license and original notice.
