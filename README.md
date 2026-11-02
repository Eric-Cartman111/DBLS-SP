# DBLS-SP：编译、运行与参数配置说明

本仓库提供 DBLS-SP 算法的 C++ 源代码，用于求解**带时间窗的同时取送货车辆路径问题（VRPSPDTW）**。

对应论文：Meng Wang，*DBLS-SP: A Dynamic Balanced Local Search with Solution Pool for the Vehicle Routing Problem with Simultaneous Pickup and Delivery and Time Windows*，Mathematics，2026，14(15)，2781。

[论文链接](https://doi.org/10.3390/math14152781) · [数据来源与校验说明](datasets/README.md)

仓库已包含全部 88 个配置中的测试算例：68 个 WC、20 个 JD，拉取后无需另行下载。原始实验日志、论文结果文件尚未提供。

## 1. 第一次使用：先把一个小算例跑起来

以下主要面向 **Windows + VS Code + PowerShell + MinGW/GCC**。VS Code 是编辑器，真正把 C++ 代码编译成程序的是 GCC。

### 第一步：下载或更新代码

如果还没有下载，在终端运行：

```powershell
git clone https://github.com/Eric-Cartman111/DBLS-SP.git
cd DBLS-SP
code .
```

如果已经打开了这个项目，只需在项目终端更新：

```powershell
git pull origin main
```

后面的命令都在**仓库根目录**运行，也就是同时能看到 `src`、`include`、`datasets` 和 `CMakeLists.txt` 的文件夹。不要先进入 `src` 或 `build` 再运行。

### 第二步：确认编译器可用

在 VS Code 中打开“终端 → 新建终端”，运行：

```powershell
g++ --version
```

如果能显示版本，说明可以继续。需要支持 C++17 和 `std::filesystem` 的编译器，建议 GCC 9 或更新版本。

如果提示找不到 `g++`，需要把 MinGW 的 `bin` 目录加入系统 `PATH`，然后重新打开 VS Code。例如，当前作者测试环境使用 `D:\mingw64\bin`；请按自己电脑上的实际安装位置设置。

### 第三步：编译

下面这组命令不需要 CMake，直接用 GCC 编译，复制到 **PowerShell** 即可：

```powershell
New-Item -ItemType Directory -Force build | Out-Null
$sources = (Get-ChildItem src\*.cpp).FullName
g++ -std=c++17 -O3 -I include $sources -o build\DBLS-SP.exe
```

三行分别表示：创建存放程序的目录、收集所有 C++ 源文件、生成可执行程序。`-O3` 开启编译优化，适合运行算法实验。

编译成功后，会得到 `build\DBLS-SP.exe`。编译可能需要等待一段时间；如果出现错误，请先处理错误，再运行下一步。

### 第四步：运行一个很小的算例

```powershell
.\build\DBLS-SP.exe -D 0 -I 0 -S 1 -t 10
```

这条命令表示：选择 WC 小规模数据集的第一个算例 `rcdp0501`，随机种子设为 1，最多运行 10 次搜索迭代。这里使用的是源码默认组件设置，适合先检查环境和数据是否能正常运行。

看到终端最后出现 `[SUMMARY]`，并且 `output` 文件夹中生成了解文件，就说明程序已完成这次运行。

## 2. 如果想通过 VS Code / CMake 编译

直接用上面的 GCC 命令编译就可以使用程序；也可以选择 CMake 方式。使用 CMake 需要安装 **CMake 3.18 或更新版本**；在 VS Code 中通过按钮操作还需要 CMake Tools 扩展。

VS Code 出现“选择工具包”时，选择你安装的 **GCC / MinGW 64 位工具包**。例如本机列表中的 `GCC 16.1.0 x86_64-w64-mingw32`。构建类型选择 **Release**，然后执行“生成 / Build”。

也可以在终端中执行下列命令。这里使用独立的 `build-cmake` 文件夹，避免和已有 `build` 目录的配置混在一起：

```powershell
cmake -S . -B build-cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-cmake --parallel
.\build-cmake\DBLS-SP.exe -D 0 -I 0 -S 1 -t 10
```

`MinGW Makefiles` 方式还需要 `mingw32-make` 可用。如果找不到它，可以先使用第 1 节的直接 GCC 编译方法。

**程序路径取决于构建目录和工具链。** 后面的示例统一使用 `.\build\DBLS-SP.exe`；如果你编译到 `build-cmake`，请替换成 `.\build-cmake\DBLS-SP.exe`。如果使用 Visual Studio 的多配置生成器，程序通常位于 `build\Release\DBLS-SP.exe`，以实际生成位置为准。

## 3. 如何选择测试数据

`-D` 选择数据集，`-I` 选择这个数据集中的具体算例。**编号都从 0 开始。**

| `-D` 的值 | 数据目录 | 算例数 | 客户数 | `-I` 可选范围 |
| --- | --- | --- | --- | --- |
| 0 | `datasets/WC_small/` | 12 | 5、10、25、50 | 0～11 |
| 1 | `datasets/WC_medium/` | 56 | 100 | 0～55 |
| 2 | `datasets/JD/` | 20 | 200、400、600、800、1000 | 0～19 |

例如：

```powershell
# WC 小规模的第一个算例：rcdp0501
.\build\DBLS-SP.exe -D 0 -I 0 -S 1 -t 10

# WC 中规模的第一个算例：cdp101
.\build\DBLS-SP.exe -D 1 -I 0 -S 1 -t 10

# JD 的第一个算例：200_1
.\build\DBLS-SP.exe -D 2 -I 0 -S 1 -t 10
```

具体编号对应哪个文件，以 [include/Config.hpp](include/Config.hpp) 中的 `FILENAMES` 列表为准，**不是文件夹按名称排序后的顺序**。

## 4. 如何开启算法组件

当前源码提供五个可选开关。它们都**默认关闭**：不写开关表示关闭，写上开关表示开启，不需要在后面加 `1` 或 `true`。

| 开关 | 作用 |
| --- | --- |
| `--repair_score` | 对不可行解使用面向修复的评分方式 |
| `--mab_ratio` | 使用多臂老虎机机制选择 ARIX 路径继承比例 |
| `--ejection_insertion` | 在选定的插入操作前，使用深度为 1 的弹出插入 |
| `--vla_archive` | 搜索停滞后，使用车辆层档案引导搜索 |
| `--qf_nnr` | 使用质量优先的最近邻解池替换方式 |

下面是在 WC 中规模第一个算例上开启全部五项组件的示例，最多 100 次迭代、时间限制设为 60 秒：

```powershell
.\build\DBLS-SP.exe -D 1 -I 0 -S 1 -t 100 -e 60 --repair_score --mab_ratio --ejection_insertion --vla_archive --qf_nnr
```

这是一条说明当前代码用法的命令。**尚未逐项核实论文各组实验的完整配置，不能把“开启全部开关”直接等同于已复现论文实验。**

## 5. 常用参数怎么改

参数写在运行命令后面，只影响本次运行，通常不需要改源代码。

| 参数 | 当前默认值 | 含义和使用方法 |
| --- | --- | --- |
| `-D` | 1 | 数据集编号：0 为 WC 小规模，1 为 WC 中规模，2 为 JD |
| `-I` | 0 | 算例编号，从 0 开始 |
| `-S` | 0 | 随机种子；0 表示每次自动产生种子；固定为 1、2 等便于对比 |
| `-t` | 5000 | 最大搜索迭代次数，例如 `-t 100` |
| `-e` | 7200 | 运行时间限制，单位秒，例如 `-e 60` |
| `-p` | 500 | 连续没有改进时，允许的停滞迭代次数 |
| `-z` | 10 | 解池大小，也就是搜索中维护的候选解数量 |
| `-c` | 2000 | 每辆车的派遣成本 |
| `-v` | 1 | 单位成本参数 |
| `-r` | 0.9 | ARIX 的初始路径继承比例 |
| `-y` | 0.8 | 适应度系数 |
| `-u` | 0.5 | 调整系数 |
| `--dataset_dir` | `./datasets/` | 数据根目录，通常不用改 |
| `--output_dir` | `./output/` | 结果保存根目录，通常不用改 |

第一次使用，先调整 `-D`、`-I`、`-S`、`-t` 和 `-e` 即可。其他参数可以先保留默认值。默认值定义在 `include/Config.hpp`，命令行解析在 `src/Parameters.cpp`。修改源代码中的默认值后，需要重新编译；只修改运行命令则不需要。

程序会在达到迭代、停滞或时间停止条件时结束搜索。时间限制在搜索迭代之间检查，**不会在初始化或某次迭代中途强制中断**，因此实际耗时可能超过 `-e`。

JD 文件自带派遣成本和单位成本，会覆盖命令行中的对应数值。当前 WC 读取器直接用坐标计算欧氏距离；`-v` 不会对 WC 距离进行额外缩放。

查看程序支持的全部参数：

```powershell
.\build\DBLS-SP.exe --help
```

### 自定义数据和输出路径

如果需要修改目录，**两个目录参数必须一起写，并且都以 `/` 结尾**。例如：

```powershell
.\build\DBLS-SP.exe --dataset_dir ./datasets/ --output_dir ./output-test/ -D 0 -I 0 -S 1 -t 10
```

`--dataset_dir` 指向包含 `WC_small`、`WC_medium` 和 `JD` 的父目录，不要直接填某个数据子目录。若只写其中一个目录参数，当前解析器会继续使用默认目录。输出目录的父目录需要提前存在。

## 6. 运行结果在哪里、怎么看

程序会自动创建 `output` 目录，并按下面的形式保存解文件：

```text
output/
  算例名-时间戳/
    算例名-时间戳-迭代编号.txt
```

默认配置下，初始解和后续改进解都可能保存，因此同一次运行的目录中可能有多个文件。

解文件第一行包含三个指标：

| 标记 | 含义 |
| --- | --- |
| `NV` | 使用的车辆数 |
| `TD` | 距离或运输成本；WC 按坐标距离计算，JD 按文件距离和单位成本计算 |
| `TC` | 总目标值：运输成本加车辆派遣成本 |

后面的 `Route 1:`、`Route 2:` 等记录每条路径的客户访问顺序。WC 小规模文件里的原始客户编号可能不连续；**当前程序按读入顺序将客户重新编号**，输出的是程序内部编号。

终端最后的 `[SUMMARY]` 是本次运行摘要，其中 `nb_vehicles` 是车辆数，`best_cost` 是运输成本，`best_total_cost` 是总目标值，`running_time` 是程序记录的搜索耗时。

如果还想保存终端输出，在命令后面接 `Tee-Object`：

```powershell
.\build\DBLS-SP.exe -D 0 -I 0 -S 1 -t 10 | Tee-Object -FilePath run-WC-small-seed1.log
```

这样既能在终端看到进度，也能得到一个日志文件。程序不会自动把完整终端输出保存成日志。

## 7. 常见问题

| 现象 | 处理方法 |
| --- | --- |
| 找不到 `g++` | 检查 MinGW 是否安装，并把实际的 `bin` 目录加入 `PATH`；重启 VS Code |
| 找不到 `cmake` | 安装 CMake 并配置 `PATH`，或使用第 1 节直接 GCC 编译方法 |
| 找不到 `DBLS-SP.exe` | 检查是否编译成功，以及实际生成目录；CMake 的目录可能与示例不同 |
| 切换工具包后提示生成器不一致 | 给新工具链使用一个新的构建目录，例如上面的 `build-cmake` |
| 缺少 `libstdc++-6.dll` 等 MinGW 运行库 | 将所用 MinGW 的 `bin` 目录加入 `PATH`，并重新打开终端 |
| 读数据时出错或程序异常退出 | 确认终端在仓库根目录，`datasets` 已拉取完整，且 `-D`、`-I` 没有越界 |
| `-e 2` 但运行超过 2 秒 | 时间限制不是实时中断，初始化和单次迭代可能使实际耗时超过限制 |
| 两次运行结果不一样 | 检查是否固定了 `-S`；默认种子 0 会自动产生随机种子 |

## 8. Linux / macOS 下运行

以下命令需要已安装支持 C++17 和 `std::filesystem` 的 C++ 编译器。使用终端进入仓库根目录后：

```sh
mkdir -p build
g++ -std=c++17 -O3 -I include src/*.cpp -o build/DBLS-SP
./build/DBLS-SP -D 0 -I 0 -S 1 -t 10
```

也可以使用 CMake：

```sh
cmake -S . -B build-cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build-cmake --parallel
./build-cmake/DBLS-SP -D 0 -I 0 -S 1 -t 10
```

其他参数的写法相同，只需把 Windows 的程序路径替换为实际生成的程序路径。

## 9. 目录说明、数据来源与论文引用

| 文件或目录 | 用途 |
| --- | --- |
| `src/` | C++ 算法实现 |
| `include/` | 头文件、默认参数和算例列表 |
| `datasets/` | 88 个 WC / JD 算例、来源说明及校验清单 |
| `build/` | 本地编译生成的程序，不上传到 GitHub |
| `output/` | 本地运行生成的解文件，不上传到 GitHub |
| `CMakeLists.txt` | CMake 构建配置 |
| `LICENSE` | 原仓库保留的 MIT 许可证 |

测试数据取自 [VRPenstein 的公开数据包](https://github.com/senshineL/VRPenstein/tree/c912c57aa387d31c20c036f11d0bca3c69ce93da/data)，只统一换行符，没有修改数值。原始来源、许可证和校验清单见 [datasets/README.md](datasets/README.md)。

目前已完成全部 88 个数据文件的结构检查，并对 WC 小规模、WC 中规模和 JD 各一个代表算例做过短运行测试。这些检查用于确认数据和程序可以配合使用，**不代表已经复现论文的全部实验结果**。做正式对比实验时，请记录实例、随机种子、算法开关、停止条件、编译器和硬件配置。

如果在研究中使用本算法，请引用：

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

本仓库原名为 `MA_FIRD`，原 README 将代码来源关联到 Zhenyu Lei、Jin-Kao Hao 的论文 [A memetic algorithm for vehicle routing with simultaneous pickup and delivery and time windows](https://ieeexplore.ieee.org/abstract/document/10669598)。仓库保留了已有提交历史、MIT 许可证及 `Copyright (c) 2024 Z.LEI` 声明，以说明代码来源。
