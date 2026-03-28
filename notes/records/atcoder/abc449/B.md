---
platform: atcoder
contest: abc449
problem: B
title: "B - Deconstruct Chocolate"
url: "https://atcoder.jp/contests/abc449/tasks/abc449_b"
tags: []
tags_display: ""
difficulty: null
status: ac
code_path: problems/atcoder/abc449/B.cpp
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
    int h, w, q;
    cin>>h>>w>>q;
    for (int i = 0; i < q; ++i){
        int sign;
        cin>>sign;
        if (sign == 1){
            int r;
            cin>>r;
            cout<<r*w<<endl;
            h -= r;
        }
        else{
            int c;
            cin>>c;
            cout<<c*h<<endl;
            w -= c;
        }
    }
    
    return 0;
}
```
