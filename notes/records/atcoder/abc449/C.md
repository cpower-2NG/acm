---
platform: atcoder
contest: abc449
problem: C
title: "C - Comfortable Distance"
url: "https://atcoder.jp/contests/abc449/tasks/abc449_c"
tags: []
tags_display: ""
difficulty: null
status: ac
code_path: problems/atcoder/abc449/C.cpp
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


int main() {
    int n, l, r;
    cin>>n>>l>>r;
    string s;
    cin>>s;
    
    vector<int> cnt(26, 0);
    long long ans = 0;
    
    for (int i = 0; i < n; ++i){
        if(i-l>=0) cnt[s[i-l] - 'a']++;
        if(i-r>=1) cnt[s[i-r-1] - 'a']--;
        ans += cnt[s[i] - 'a'];
    }

    cout<<ans<<endl;

    return 0;
}
```
