# WC 与 JD 基准算例 / Benchmark instances

本目录提供 `include/Config.hpp` 中列出的全部 **88 个算例**，可直接用于当前 DBLS-SP 数据读取器。

| 目录 | 数据集索引 `-D` | 数量 | 客户数 |
| --- | --- | --- | --- |
| `WC_small/` | 0 | 12 | 5、10、25、50 |
| `WC_medium/` | 1 | 56 | 100 |
| `JD/` | 2 | 20 | 200、400、600、800、1000 |

## 来源与处理 / Source and processing

全部文件取自 [senshineL/VRPenstein](https://github.com/senshineL/VRPenstein) 公开提供的 `data/data.tar.gz`，固定版本为 [`c912c57aa387d31c20c036f11d0bca3c69ce93da`](https://github.com/senshineL/VRPenstein/tree/c912c57aa387d31c20c036f11d0bca3c69ce93da)。

- WC：压缩包中的 `Wang_Chen/*.txt`，按代码中的文件名列表分入两个目录。
- JD：压缩包中的 `Liu_Tang_Yao/*.vrpsdptw`，复制到 `JD/`。
- 仅统一换行符为 LF；没有修改数值、客户顺序、坐标、时间窗或距离矩阵。
- 未收录包中另外提供的 `explicit_*.vrpsdptw` WC 转换版本，当前代码读取 WC 原始坐标格式。
- 下载日期：2026-11-02。

All 88 instances were imported from the pinned VRPenstein archive above. Only line endings were normalized to LF. Numerical data and record ordering were preserved. WC files were organized into the two groups expected by the solver; JD files came from the archive's `Liu_Tang_Yao/` directory.

原压缩包 SHA-256 / Archive SHA-256:

```text
fa0bfd7f1a249e0874e4b99038d056b46246d74d7899b3cc68a6fe2855a65489
```

`manifest.csv` 记录每个文件的原包路径、客户数量、文件大小、原始字节 SHA-256 与统一换行后的 SHA-256，便于核对数据版本。

## 使用 / Usage

在仓库根目录运行（Windows PowerShell，MinGW 构建）：

```powershell
# WC：第一个小规模算例，固定种子，开启五项可选组件
.\build\DBLS-SP.exe -D 0 -I 0 -S 1 --repair_score --mab_ratio --ejection_insertion --vla_archive --qf_nnr

# JD：第一个 200 客户算例
.\build\DBLS-SP.exe -D 2 -I 0 -S 1 --repair_score --mab_ratio --ejection_insertion --vla_archive --qf_nnr
```

Linux 使用 `./build/DBLS-SP`；Visual Studio 构建通常使用 `.\build\Release\DBLS-SP.exe`。完整参数与编译说明见根目录 README。

## 署名和许可 / Attribution and license

数据来自 WC（Wang–Chen）和 JD 基准系列，并通过 VRPenstein 的公开数据包获取。请在论文中同时注明所用基准数据的原始来源及相关算法论文。VRPenstein 仓库附带的 MIT 许可证保存在 `LICENSE.VRPenstein`，原版权声明为 `Copyright (c) 2020 Shengcai Liu`；保留该声明并不将原始基准数据的作者改为本仓库作者。

The upstream repository's MIT license is retained in `LICENSE.VRPenstein`. Original benchmark authorship remains with the respective creators. These are benchmark inputs, not original DBLS-SP experiment logs, solution files, or evidence that the paper's numerical results have been reproduced.
