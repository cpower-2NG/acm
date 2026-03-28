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