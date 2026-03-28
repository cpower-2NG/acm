#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct BIT {
    vector<ll> a;
    int n;
    
    BIT(int n_) : n(n_), a(n_ + 1, 0) {}
    
    void add(int i, ll x) {
        i++;
        while (i <= n) {
            a[i] += x;
            i += i & (-i);
        }
    }
    
    ll sum(int r) {
        ll s = 0;
        while (r) {
            s += a[r];
            r -= r & (-r);
        }
        return s;
    }
    
    int lower_bound(ll w) {
        int x = 0;
        int k = 1 << (int)(__lg(n) + 1);
        while (k) {
            if (x + k <= n && a[x + k] < w) {
                w -= a[x + k];
                x += k;
            }
            k >>= 1;
        }
        return x + 1;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;
    
    vector<int> A(N);
    vector<ll> cnt(M, 0);
    
    for (int i = 0; i < N; i++) {
        cin >> A[i];
        A[i]--;
        cnt[A[i]]++;
    }
    
    // 按频次和值排序
    vector<pair<ll, int>> s;
    for (int i = 0; i < M; i++) {
        s.push_back({cnt[i], i});
    }
    sort(s.begin(), s.end());
    
    // 计算阶段累积长度
    vector<ll> r(M + 1);
    r[0] = N;
    for (int i = 0; i < M - 1; i++) {
        r[i + 1] = r[i] + (ll)(i + 1) * (s[i + 1].first - s[i].first);
    }
    r[M] = 1e18 + 2026;
    
    int Q;
    cin >> Q;
    
    vector<ll> ans(Q, -1);
    vector<tuple<int, ll, int>> que;
    
    for (int i = 0; i < Q; i++) {
        ll X;
        cin >> X;
        
        if (X <= N) {
            ans[i] = A[X - 1];
            continue;
        }
        
        // 二分找所在阶段
        int ng = 0, ok = M;
        while (ok - ng != 1) {
            int vs = (ok + ng) >> 1;
            if (r[vs] >= X) {
                ok = vs;
            } else {
                ng = vs;
            }
        }
        
        // 计算该阶段内的相对位置
        X -= r[ng] + 1;
        X %= ok;
        
        que.push_back({ok, X, i});
    }
    
    sort(que.begin(), que.end());
    
    BIT fw(M);
    int idx = 0;
    for (auto [ok, x, i] : que) {
        while (idx < ok) {
            fw.add(s[idx].second, 1);
            idx++;
        }
        ans[i] = fw.lower_bound(x + 1) - 1;
    }
    
    for (ll v : ans) {
        cout << v + 1 << "\n";
    }

    return 0;
}