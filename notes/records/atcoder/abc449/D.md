---
platform: atcoder
contest: abc449
problem: D
title: "D - Make Target 2"
url: "https://atcoder.jp/contests/abc449/tasks/abc449_d"
tags: []
tags_display: ""
difficulty: null
status: ac
code_path: problems/atcoder/abc449/D.cpp
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

int l, r, d, u;
int f(int k){
    int lx=max(l,-k), rx=min(r,k);
    int ly=max(d,-k), ry=min(u,k);
    
    if(lx>rx || ly>ry) return 0;
    return (rx-lx+1)*(ry-ly+1);
}

int main(){
    
    cin>>l>>r>>d>>u;

    int k = max({abs(l),abs(r),abs(d),abs(u)});
    long long ans = 0;

    for(int i = 0; i <= k; i += 2)
        ans += f(i) - f(i-1);

    cout<<ans<<endl;
}
```
