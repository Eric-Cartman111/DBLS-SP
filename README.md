# DBLS-SP

C++ implementation associated with **DBLS-SP: A Dynamic Balanced Local Search with Solution Pool for the Vehicle Routing Problem with Simultaneous Pickup and Delivery and Time Windows**, by Meng Wang, *Mathematics* **2026**, 14(15), 2781.

Paper: <https://doi.org/10.3390/math14152781>

DBLS-SP solves the vehicle routing problem with simultaneous pickup and delivery and time windows (VRPSPDTW). This release contains all source files supplied in the author's local `DBLS-SP_code` directory and all 88 configured WC/JD benchmark instances. Original experiment logs and result files are not included.

## Contents

- `include/`: headers, configuration, and bundled command-line parser.
- `src/`: algorithm, solution pool, local search, insertion, evaluation, and data readers.
- `datasets/`: 68 WC and 20 JD instances, source attribution, upstream license, and checksum manifest.
- `CMakeLists.txt`: C++17 build configuration.
- `LICENSE`: existing MIT license and original copyright notice.

## Build

Requirements: a C++17 compiler with `std::filesystem` support (for example GCC 9 or newer), and CMake 3.18 or newer for the CMake workflow. No external library dependencies are required.

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

For Windows with MinGW, select the generator explicitly if needed:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Alternatively, compile directly with GCC (without CMake).

Linux / macOS shell:

```sh
mkdir -p build
g++ -std=c++17 -O3 -I include src/*.cpp -o build/DBLS-SP
```

Windows PowerShell with MinGW on PATH:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
$sources = (Get-ChildItem src/*.cpp).FullName
g++ -std=c++17 -O3 -I include $sources -o build/DBLS-SP.exe
.\build\DBLS-SP.exe --help
```

With a Visual Studio multi-configuration generator, the executable is normally `build/Release/DBLS-SP.exe`; with MinGW it is `build/DBLS-SP.exe`.

## Benchmark data

All configured WC and JD instances are included in `datasets/`, so no separate download is needed. They were imported from the public [VRPenstein benchmark archive](https://github.com/senshineL/VRPenstein/tree/c912c57aa387d31c20c036f11d0bca3c69ce93da/data). See [datasets/README.md](datasets/README.md) for provenance, processing, upstream license, and checksums. Only line endings were normalized; numerical contents were preserved.

```text
datasets/
  WC_small/    # e.g. rcdp0501.txt
  WC_medium/   # e.g. cdp101.txt
  JD/          # e.g. 200_1.vrpsdptw
```

Dataset indices (`-D`): 0 = WC_small, 1 = WC_medium, 2 = JD. Instance indices (`-I`) are zero-based within the lists in `include/Config.hpp`; consult those lists for supported filenames and ordering. The current lists contain 12 WC_small, 56 WC_medium, and 20 JD instances.

Run from the repository root, or pass explicit paths. **Supply `--dataset_dir` and `--output_dir` together, with a trailing `/` on both paths.** Custom paths are used only when both options are supplied; the parser concatenates the dataset subdirectory to the dataset root. The input file must exist before running. Use an existing parent directory for the output path.

## Run

Show the options:

```sh
./build/DBLS-SP --help
```

Run the first WC_medium instance with a fixed seed and the source defaults:

```sh
./build/DBLS-SP --dataset_dir ./datasets/ --output_dir ./output/ -D 1 -I 0 -S 1
```

The five optional components are **off by default** in the supplied source. Enable all five explicitly to use repair scoring, MAB ratio control, ejection insertion, vehicle-layer archive guidance, and quality-first nearest-neighbor replacement:

```sh
./build/DBLS-SP --dataset_dir ./datasets/ --output_dir ./output/ -D 1 -I 0 -S 1 --repair_score --mab_ratio --ejection_insertion --vla_archive --qf_nnr
```

For JD, change `-D 1` to `-D 2`. On Windows use `.\build\DBLS-SP.exe` (or `.\build\Release\DBLS-SP.exe`). To save the console log in PowerShell:

```powershell
.\build\DBLS-SP.exe --dataset_dir ./datasets/ --output_dir ./output/ -D 2 -I 0 -S 1 --repair_score --mab_ratio --ejection_insertion --vla_archive --qf_nnr | Tee-Object -FilePath run-JD-200_1-seed1.log
```

These examples describe the supplied source; they are not a verified reconstruction of every paper experiment. Record the instance, seed, flags, stopping criteria, compiler, and hardware for comparisons.

## Parameters

Defaults come from `include/Config.hpp` and `src/Parameters.cpp`.

| Option | Default | Meaning |
| --- | --- | --- |
| `--dataset_dir` | `./datasets/` | Dataset root; pass with `--output_dir` |
| `--output_dir` | `./output/` | Output root; pass with `--dataset_dir` |
| `-D`, `--datasets` | 1 | Dataset index |
| `-I`, `--instance` | 0 | Instance index |
| `-S`, `--seed` | 0 | Random seed; 0 draws from `std::random_device` |
| `-c`, `--dispatch_cost` | 2000 | Vehicle dispatch cost |
| `-v`, `--unit_cost` | 1 | Unit cost parameter |
| `-t`, `--max_iterations` | 5000 | Iteration limit |
| `-e`, `--max_running_time` | 7200 | Time limit in seconds, checked between iterations |
| `-p`, `--patience` | 500 | Stagnation iteration limit |
| `-z`, `--population_size` | 10 | Solution pool size |
| `-r`, `--init_ratio` | 0.9 | Initial ARIX ratio |
| `-y`, `--fit_coef` | 0.8 | Fitness coefficient |
| `-u`, `--adjust_factor` | 0.5 | Adjustment factor |
| `--repair_score` | off | Repair scoring for infeasible solutions |
| `--mab_ratio` | off | MAB selection of the ARIX ratio |
| `--ejection_insertion` | off | Depth-1 ejection insertion |
| `--vla_archive` | off | Vehicle-layer archive guidance after stagnation |
| `--qf_nnr` | off | Quality-first nearest-neighbor replacement |

JD input files supply dispatch and unit costs, overriding CLI values. WC distances are computed directly from coordinates in the current reader. The time limit does not interrupt initialization or a search iteration.

## Output

Solutions are written to `output/<instance>-<timestamp>/<instance>-<timestamp>-<iteration>.txt`. Files contain vehicle count (`NV`), distance/cost (`TD`), total objective (`TC`), and each route's customer sequence. Console output includes parameters, progress, and a final `[SUMMARY]` line. Capture console output separately for experiment logs.

## Citation and provenance

If you use DBLS-SP in research, please cite:

```bibtex
@article{wang2026dblssp,
  author = {Wang, Meng},
  title = {{DBLS-SP}: A Dynamic Balanced Local Search with Solution Pool for the Vehicle Routing Problem with Simultaneous Pickup and Delivery and Time Windows},
  journal = {Mathematics},
  year = {2026},
  volume = {14},
  number = {15},
  pages = {2781},
  doi = {10.3390/math14152781}
}
```

This repository was previously named `MA_FIRD`. Its earlier README credited the implementation accompanying Zhenyu Lei and Jin-Kao Hao, [A memetic algorithm for vehicle routing with simultaneous pickup and delivery and time windows](https://ieeexplore.ieee.org/abstract/document/10669598). The existing MIT license, including the 2024 Z.LEI copyright notice, and repository history are retained to acknowledge that provenance.
