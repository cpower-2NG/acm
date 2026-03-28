# AtCoder ABC451 E - Tree Distance

## Problem

给定一个 $N \times N$ 的距离矩阵 $A$，其中 $A[i][j]$ 表示点 $i$ 和点 $j$ 的距离。判断是否存在一棵 $N$ 个顶点的边带权无向树，使得树上任意两点的距离恰好等于给定矩阵中的对应值。

**约束**
- $2 \leq N \leq 3000$
- 时间限制：6 秒
- $A[i][j] < 10^6$

---

## Insight

### 关键观察

1. **树的唯一性**：给定的距离矩阵如果对应某棵树，则该树是唯一的。这是因为树的任意两点间的路径是唯一的。

2. **MST 性质**：如果存在满足条件的树，那么用 Prim 或 Kruskal 在完全图上（边权为 $A[i][j]$）生成的最小生成树就会是这棵树。**这是本题的核心性质**。

### 算法策略

分两阶段：

#### 阶段一：用 Prim 生成候选树（$O(N^2)$）

- 构建完全图，任意两点间有一条边，权重为 $A[i][j]$。
- 用 Prim 的"稠密图版本"（数组扫最小，不用堆）生成 MST。
- 这棵 MST 是我们的候选树。

**为什么选 Prim？**
- 完全图是稠密图（$M = O(N^2)$），数组版 Prim 的 $O(N^2)$ 比堆版本的 $O(M \log N)$ 更优。

#### 阶段二：全遍历距离验证（$O(N^2)$）

- 对候选树，从每个点出发做 DFS 累加边权，得到到达所有其他点的距离。
- 逐项与输入矩阵 $A$ 比对。
- 所有距离都相等则 `Yes`，否则 `No`。

**为什么要验证？**
- 虽然 MST 是唯一的候选，但完全图的边权可能有特殊结构（如违反三角不等式的部分扭曲），导致 MST 虽然存在但不满足距离条件。
- 验证是"保险机制"，确保最终答案正确。

---

## Complexity

### 时间复杂度

- **Prim 阶段**：$O(N^2)$（每轮扫 $N$ 个点找最小值，共 $N$ 轮）
- **验证阶段**：$O(N)$ 次 DFS，每次 $O(N)$（树边数 $N-1$），共 $O(N^2)$
- **总计**：$O(N^2)$

对于 $N=3000$，约 $9 \times 10^6$ 次操作，在 6 秒内轻松通过。

### 空间复杂度

- 距离矩阵：$O(N^2)$
- 邻接表（树）：$O(N)$
- **总计**：$O(N^2)$

---

## Pitfalls

1. **数据类型溢出**  
   - 路径距离累计时，`int` 在大数据下易溢出。
   - 建议距离、边权都使用 `long long`。

2. **矩阵输入范围**  
   - 题目只输入上三角（$i < j$ 的部分）。
   - 需要 $O(N^2)$ 空间读入并补对称，否则后续索引错误。

3. **Prim 选点逻辑**  
   - 每轮必须从**未入树且 `best` 最小**的点中选。
   - 即使实现有微妙差异（如本题 `v=i` 也凑巧能过），标准做法是 `v=j`；前者容易在严格测试中失败。

4. **基础合法性检查**  
   - `dis[i][i]` 应为 0（对角线）。
   - `dis[i][j]`（$i \ne j$）应为正。
   - 检查三角不等式：若 $dis[i][j] < dis[i][k] + dis[k][j]$，直接 `No`。

5. **连通性检查（可选但安全）**  
   - 生成的边集虽然数量是 $N-1$，但不一定连通。
   - 用 DFS 检查起来也是 $O(N)$，无损。

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// 在树上从 u 出发，累加边权计算距离
void cal_dis(long long cur, int fa, int u,
             vector<long long>& dist,
             vector<vector<pair<int, long long>>>& adj) {
    dist[u] = cur;
    for (auto [v, w] : adj[u]) {
        if (v == fa) continue;
        cal_dis(cur + w, u, v, dist, adj);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<vector<long long>> dis(n, vector<long long>(n, 0));

    // 读上三角，补对称
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            cin >> dis[i][j];
            dis[j][i] = dis[i][j];
        }
    }

    // 基础检查
    for (int i = 0; i < n; ++i) {
        if (dis[i][i] != 0) { cout << "No\n"; return 0; }
        for (int j = 0; j < n; ++j) {
            if (i != j && dis[i][j] <= 0) { cout << "No\n"; return 0; }
        }
    }

    // Prim: 稠密图 MST (O(N^2))
    const long long INF = (1LL << 62);
    vector<long long> best(n, INF);
    vector<int> parent(n, -1);
    vector<bool> used(n, false);

    best[0] = 0;
    vector<pair<int, int>> edges;
    edges.reserve(n - 1);

    for (int it = 0; it < n; ++it) {
        int v = -1;
        for (int i = 0; i < n; ++i) {
            if (!used[i] && (v == -1 || best[i] < best[v])) {
                v = i;
            }
        }

        if (v == -1) {
            cout << "No\n";
            return 0;
        }

        used[v] = true;
        if (parent[v] != -1) {
            edges.push_back({v, parent[v]});
        }

        for (int to = 0; to < n; ++to) {
            if (!used[to] && dis[v][to] < best[to]) {
                best[to] = dis[v][to];
                parent[to] = v;
            }
        }
    }

    if ((int)edges.size() != n - 1) {
        cout << "No\n";
        return 0;
    }

    // 建树
    vector<vector<pair<int, long long>>> adj(n);
    for (auto [u, v] : edges) {
        long long w = dis[u][v];
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    // 全对验证：树上距离与输入是否完全一致
    for (int s = 0; s < n; ++s) {
        vector<long long> got(n, -1);
        cal_dis(0, -1, s, got, adj);
        for (int t = 0; t < n; ++t) {
            if (got[t] != dis[s][t]) {
                cout << "No\n";
                return 0;
            }
        }
    }

    cout << "Yes\n";
    return 0;
}
```

---

## 总结

- **核心思想**：相信官方结论"存在满足条件的树 ⟺ MST 是唯一候选"，用 Prim 生成并验证。
- **复杂度瓶颈**：Prim 和验证都是 $O(N^2)$，总体可控。
- **关键细节**：数据类型、矩阵对称性、三角不等式检查。
