---
platform: atcoder
contest: abc449
problem: E
title: "E - A += v"
url: "https://atcoder.jp/contests/abc449/tasks/abc449_e"
tags: []
tags_display: ""
difficulty: null
status: ac
code_path: problems/atcoder/abc449/E.cpp
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
using ll = long long;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N,M;
    cin>>N>>M;

    vector<ll> A(N+1),cnt(M+1);

    for(int i=1;i<=N;i++){
        cin>>A[i];
        cnt[A[i]]++;
    }

    ll mn=*min_element(cnt.begin()+1,cnt.end());

    vector<int> pre;
    for(int i=1;i<=M;i++)
        if(cnt[i]==mn)
            pre.push_back(i);

    ll P=pre.size();

    int Q;
    cin>>Q;

    while(Q--){
        long long X;
        cin>>X;

        if(X<=N){
            cout<<A[X]<<"\n";
            continue;
        }

        X-=N;

        if(X<=P){
            cout<<pre[X-1]<<"\n";
            continue;
        }

        X-=P;
        cout<<(X-1)%M+1<<"\n";
    }
}
```
