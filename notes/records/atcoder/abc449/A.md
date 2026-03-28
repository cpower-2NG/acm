---
platform: atcoder
contest: abc449
problem: A
title: "A - π"
url: "https://atcoder.jp/contests/abc449/tasks/abc449_a"
tags: ["图论", "动态规划", "贪心"]
tags_display: "图论(graph), 动态规划(dp), 贪心(greedy)"
difficulty: null
status: ac
code_path: problems/atcoder/abc449/A.cpp
updated_at: 2026-03-27T03:36:04Z
---

## Idea

TODO

## Pitfalls

TODO

## AC Code

```cpp
#include <bits/stdc++.h>
using namespace std;

const double pi = 3.141592653589793;

int main() {
    int d;
    cin>>d;
    long double ans = d*d/4.0*pi;
    cout<<fixed<<setprecision(8)<<ans<<endl;
    
    return 0;
}
```
