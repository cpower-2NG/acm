**题目与目标**

给定序列 $A=(A_1,\dots,A_N)$ 与 $B=(B_1,\dots,B_M)$，求
$$
\sum_{i=1}^N\sum_{j=1}^M A_i\,B_j\,(i\bmod j)\pmod{998244353}.
$$

下面给出完整的数学推导与算法，实现时应注意取模和整型溢出。

**一、式子变换（去掉模运算）**

利用恒等
$$
i\bmod j = i - j\left\lfloor\frac{i}{j}\right\rfloor,
$$
可得
\begin{align*}
S &= \sum_{i=1}^N\sum_{j=1}^M A_i B_j (i\bmod j) \\
  &= \sum_{i=1}^N\sum_{j=1}^M A_i B_j \Big(i - j\big\lfloor\frac{i}{j}\big\rfloor\Big) \\
  &= \sum_{i=1}^N A_i i\sum_{j=1}^M B_j - \sum_{i=1}^N A_i \sum_{j=1}^M B_j\, j\Big\lfloor\frac{i}{j}\Big\rfloor.
\end{align*}

把求和顺序按 $j$ 展开也是常见做法；下面给出两种等价且常用的实现思路，重点在于如何高效计算含有 $\lfloor\cdot\rfloor$ 的项。

**二、按 $j$ 外层（推荐实现）——对 $A$ 分段**

我们把原式按 $j$ 展开：令
$$
S = \sum_{j=1}^M B_j\cdot S_j,\qquad S_j=\sum_{i=1}^N A_i (i\bmod j).
$$
由 $i\bmod j = i - j\lfloor i/j\rfloor$，得到
$$
S_j = \underbrace{\sum_{i=1}^N A_i i}_{\text{可预计算一次}} - j\cdot T_j,
\qquad T_j=\sum_{i=1}^N A_i\left\lfloor\frac{i}{j}\right\rfloor.
$$

关键在于高效计算 $T_j$. 观察到：对固定 $j$，$\lfloor i/j\rfloor$ 在 $i$ 上是分段常数：当 $k\ge1$ 时
$$\lfloor i/j\rfloor = k \iff kj \le i \le \min((k+1)j-1,\;N).$$
因此
\[\displaystyle
T_j=\sum_{k=1}^{\lfloor N/j\rfloor} k \sum_{i=kj}^{\min((k+1)j-1,N)} A_i.
\]

用前缀和 $\mathrm{pref}[x]=\sum_{i=1}^x A_i$, 区间和在 $O(1)$ 内得到。于是对每个 $j$ 遍历 $k=1\ldots\lfloor N/j\rfloor$ 即可算出 $T_j$。

复杂度分析：对每个 $j$ 的时间为 $O(\lfloor N/j\rfloor)$，总时间
$$\sum_{j=1}^M O\big(\lfloor N/j\rfloor\big)=O\Big(N\sum_{j=1}^M\frac{1}{j}\Big)=O(N\log M).$$

实现要点：
- 预算好 $P=\sum_{i=1}^N A_i i\pmod p$（只需一次）。
- 对每个 $j$：若 $j>N$ 则 $T_j=0$，且 $S_j=P$；否则按上面分段计算 $T_j$ 并得到 $S_j=(P - jT_j)\bmod p$。

注意取模和负数处理：每次减法后若为负须加上模 $p$。

**三、按 $i$ 外层——对 $B$ 分段（另一种视角）**

也可按 $i$ 外层求
$$
S = \sum_{i=1}^N A_i \Big(\sum_{j=1}^M B_j (i\bmod j)\Big) = \sum_{i=1}^N A_i\,U_i.
$$
对固定 $i$，把 $j$ 分成 $j>i$ 与 $j\le i$ 两部分：
- 若 $j>i$，有 $i\bmod j = i$，这部分贡献 $i\sum_{j=i+1}^M B_j$；
- 若 $j\le i$，使用 $i\bmod j = i - j\lfloor i/j\rfloor$, 得到
\[\displaystyle
\sum_{j=1}^{\min(M,i)} B_j(i\bmod j) = i\sum_{j=1}^{\min(M,i)} B_j - \sum_{j=1}^{\min(M,i)} B_j\, j\Big\lfloor\frac{i}{j}\Big\rfloor.
\]

对第二项令 $q=\lfloor i/j\rfloor$, 则当且仅当
$$\left\lfloor\frac{i}{q+1}\right\rfloor < j \le \left\lfloor\frac{i}{q}\right\rfloor.$$ 
因此可以把 $j$ 按 $q$ 分段：
\[\displaystyle
\sum_{j=1}^{\min(M,i)} B_j\, j\Big\lfloor\frac{i}{j}\Big\rfloor = \sum_{q\ge1} q \sum_{j=\lfloor i/(q+1)\rfloor+1}^{\lfloor i/q\rfloor} jB_j,
\]
区间和可用前缀和 $\mathrm{prefJB}[x]=\sum_{j=1}^x jB_j$ 在 $O(1)$ 内得到。

该方法的总复杂度受每个 $i$ 的分段数量影响，通常也可用平方根分割等技巧优化。

**四、总结与实现细节**
- 推荐实现：按 $j$ 外层、用 $A$ 的前缀和与上面 (二) 中的分段方法，代码简洁且复杂度约 $O(N\log M)$。
- 数据类型：中间乘法与累加请用 64 位整型并及时对 $998244353$ 取模。
- 边界：若 $j>N$, $T_j=0$；若 $A,B$ 很大，输入时可对模取余（若题目允许）。

**五、伪代码（按 j 外层）**
```
read N, M, A[1..N], B[1..M]
pref[0]=0; for i=1..N: pref[i]=pref[i-1]+A[i]
P = sum_{i=1..N} A[i]*i (mod p)
ans = 0
for j = 1..M:
  if j > N: S_j = P
  else:
    T = 0
    for k = 1..floor(N/j):
      l = k*j; r = min(N, (k+1)*j-1)
      seg = pref[r]-pref[l-1]
      T += k * seg (mod p)
    S_j = (P - j*T) mod p
  ans = (ans + B[j] * S_j) mod p
print ans
```

如需，我可以在此附上完整 C++ 参考实现并把其加入本仓库。
