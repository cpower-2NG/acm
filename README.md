# acm

个人 ACM / OI 学习与比赛仓库，包含代码、题解、算法模板与比赛记录。

默认原则：所有题目代码都进入 Git；文字内容按需维护，不要求每题都写文案。

## 目录结构

```
.
├── problems/                # 题目代码
│   ├── atcoder/
│   │   └── abc449/
│   │       ├── A.cpp
│   │       ├── B.cpp
│   │       ├── C.cpp
│   │       ├── D.cpp
│   │       └── E.cpp
│   └── practice/
│       └── test.cpp
├── templates/               # 可复用算法模板
│   └── cpp/
├── notes/                   # 文字内容（题解 / 比赛游记 / 学习笔记）
│   ├── solutions/
│   ├── contests/
│   └── topics/
├── build/                   # 编译产物（自动生成，不入库）
└── Makefile
```

## 编译方式

统一使用根目录 `Makefile` 编译，所有可执行文件输出到 `build/bin/` 下，并保持与 `problems/` 相同的相对路径层级。

## CPH 使用兼容性

当前目录结构兼容 CPH（Competitive Programming Helper）常见工作流：

- 直接在 `problems/atcoder/abc449/A.cpp`、`problems/codeforces/.../*.cpp` 这类路径中写题。
- CPH 生成的 `.prob` 和 `.cph/` 临时文件已通过 `.gitignore` 递归忽略，不会污染提交。
- 建议保持目录名无空格（当前已是无空格风格），可减少脚本与命令行参数问题。

## 题目索引与标签自动抓取

仓库提供自动索引脚本：`scripts/update_problem_index.py`

- 自动扫描 `problems/` 下所有 `.cpp` 题目文件。
- 自动生成：
	- `data/problems_index.json`（机器可读索引）
	- `data/problem_annotations.tsv`（手工补标签/难度入口）
	- `data/tag_aliases.json`（中英文标签映射词表）
	- `PROBLEMS.md`（总索引）
	- `PROBLEMS_BY_TAG.md`（按标签索引）
	- `PROBLEMS_UNTAGGED.md`（待补标签清单）
	- `notes/records/<platform>/<contest>/<problem>.md`（逐题记录）
- 标签维护方式：编辑 `data/problem_annotations.tsv` 即可。
- 标签可用中文或英文填写，脚本会按 `data/tag_aliases.json` 自动归一，并在索引中显示中英文。
- 为方便手工补全，`problem_annotations.tsv` 会把未打标签题目自动排在前面（新发现题优先）。
- 元数据来源：
	- Codeforces: 标题、标签、rating。
	- AtCoder: 标题（含页面回退抓取）、difficulty（源可用时）。

## 内容维护建议

- 题解：默认只记录值得复盘的好题，放在 `notes/solutions/`。
- 模板：放在 `templates/cpp/`，按主题拆分（图论、DP、字符串等）。
- 比赛游记：可选，仅在需要系统复盘时写到 `notes/contests/`。

## 后续可扩展

- 增加 `scripts/` 存放本地工具脚本（批量新建题目、统计题量等）。
- 按平台增加 `problems/codeforces/`、`problems/luogu/` 等目录。
