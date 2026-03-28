#include <bits/stdc++.h>
using namespace std;

struct edge{
    int u, v, w;
};

void dfs(int cur, vector<vector<pair<int, int> > >& adj, vector<bool>& vis){
    vis[cur] = true;
    for (auto [v, w] : adj[cur]){
        if (!vis[v]){
            dfs(v, adj, vis);
        }
    }
}

void cal_dis(int cur, int fa, int u, vector<int>& dis, vector<vector<pair<int, int> > >& adj){
    dis[u] = cur;
    for (auto [v, w] : adj[u]){
        if (v != fa){
            cal_dis(cur + w, u, v, dis, adj);
        } 
    }
} 

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin>>n;
    vector<vector<int> > dis(n, vector<int> (n,0));
    for (int i = 0; i < n; ++i){
        for (int j = i+1; j < n; ++j){
            cin>>dis[i][j];
            dis[j][i] = dis[i][j];
        }
    }

    

    // prim推边
    vector<edge> edges;
    vector<int> best(n, INT_MAX);
    vector<int> fa(n, -1);
    vector<bool> used(n, 0);


    best[0] = 0;
    for (int i = 0; i < n; ++i){
        int v = -1;
        for (int j = 0; j < n; ++j){
            if (!used[j] && (v == -1 || best[j] < best[v])){
                v = j;
            }
        }

        if (v == -1){
            cout<<"No"<<endl;
            return 0;
        }

        used[v] = 1;
        if (fa[v] != -1){
            edges.push_back({v, fa[v], dis[v][fa[v]]});
        }

        for (int j = 0; j < n; ++j){
            if (!used[j] && dis[v][j] < best[j]){
                best[j] = dis[v][j];
                fa[j] = v;
            }
        }
    }

    if (edges.size() != n-1){
        cout<<"No"<<endl;
        return 0;
    }

    vector<vector<pair<int, int> > > adj(n);
    for (int i = 0; i < edges.size(); ++i){
        adj[edges[i].u].push_back({edges[i].v,edges[i].w});
        adj[edges[i].v].push_back({edges[i].u,edges[i].w});
    }
    
    vector<bool> vis(n, false);
    dfs(0, adj, vis);
    for (int i = 0 ; i < n; ++i){
        if (!vis[i]){
            cout<<"No"<<endl;
            return 0;
        }
    }
    
    for (int i = 0; i < n; ++i){
        vector<int> __dis(n);
        cal_dis(0, -1, i, __dis, adj);
        for (int j = 0; j < n; ++j){
            if (__dis[j] != dis[i][j]){
                cout<<"No"<<endl;
                return 0;
            }
        }
    }

    cout<<"Yes"<<endl;

    return 0;
}

/*
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⠤⠖⠚⠛⠉⠉⠉⠉⠉⠛⠓⠲⠦⣤⣀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠛⠁⢀⣠⣴⣶⣿⠿⠿⠿⣿⣿⣿⣿⣿⣶⣤⣄⣀⠈⠛⠶⣄
⠀⠀⠀⠀⠀⠀⠀⠀⡠⠊⠀⣠⣶⡿⠛⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠙⠻⢶⣭⣛⠿⣷⣤⡀⠉⠳⣄
⠀⠀⠀⠀⠀⠀⣠⠋⠀⣴⡿⢋⡄⢠⡆⠀⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣿⣿⠿⣷⡀⠈⠛⣷⣄⠀⠳⡄
⠀⠀⠀⠀⠀⡞⠀⣠⣿⠋⣾⠋⠀⢻⡄⣆⠀⠉⠉⠛⠿⣦⡀⢀⣴⣿⣿⣿⠉⠀⠀⠀⠻⣄⠀⠀⠙⣿⡀⠈⢦
⠀⠀⠀⢀⠟⠀⣼⡿⢀⣿⠁⢀⣾⠋⣿⡈⢷⣄⠀⠀⠀⣠⣿⣿⣿⠉⠀⢻⡆⠀⠀⣀⣤⣿⣦⠀⠀⠈⢿⣆⠀⢷
⠀⠀⢀⠏⠀⣾⡟⠀⣿⠁⢠⡿⠀⠀⠀⠙⠷⣭⣿⣶⣾⣿⠟⠀⠙⣷⠀⣸⣷⣿⣿⡿⠟⠋⣿⡀⠀⠀⠀⣿⡄⠀⣧
⠀⠀⡾⠀⣼⡟⠀⣾⠃⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠛⠷⠶⣼⣿⣿⣿⠉⠀⠀⠀⠀⠘⣷⠀⠀⣠⠘⣿⠀⠸⡀
⠀⢰⠁⢠⣿⠀⢰⡏⠀⣾⠃⠀⠀⣴⣦⠀⠀⠀⠀⠀⣀⣀⣀⠀⠀⠀⠈⣿⣿⠀⠀⠀⠀⠀⠀⣿⠀⠀⠘⣧⣿⡇⠀⣇
⠀⡟⠀⣿⡏⠀⣿⠀⢀⣿⠀⠀⣿⠁⢹⡇⠀⠀⢠⣿⣿⣿⣿⣿⣾⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⣿⠀⠀⠀⣿⢸⣿⠀⣿
⢀⠇⠀⣿⠀⣼⣿⠀⢸⡇⠀⢰⣿⣿⣿⡇⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⣿⠀⠀⠀⣿⣿⡇⠀⡟
⢸⠀⢸⣿⢠⣿⣿⠀⣸⡇⠀⢸⣿⣿⣿⠇⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⣀⠀⠀⣿⠀⠀⠀⣿⣿⠁⢠⠇
⢸⠀⢸⣿⣼⣿⣿⠀⣿⣿⠀⢹⣿⣿⣿⠀⠀⠀⠀⠈⢿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⢀⣿⠀⢸⣇⠀⠀⢸⣿⡟⠀⡾
⢸⠀⠸⣿⣿⢿⣿⠀⢹⣿⣄⣈⣿⡿⠃⠀⠀⠀⠀⠀⠀⠉⠿⣿⠿⣛⣁⣿⠀⠀⠀⣸⠇⠀⣿⠹⣷⠀⣿⡿⠀⣼
⠀⡆⠀⣿⣿⠸⣿⠀⢸⣿⠿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⢿⡿⠀⠀⠀⣿⠀⢰⡏⣸⡿⢀⣿⠀⢰⠁
⠀⢻⠀⢻⣿⣷⣿⡄⠘⣿⣄⠀⠀⠀⠀⠀⠀⠟⠿⠀⠀⠀⠀⠀⠀⠀⣾⠃⠀⠀⣰⡿⠀⣿⣿⠟⠀⢸⣿⠀⢸
⠀⠀⡇⠀⢿⣿⠿⣷⠀⣿⣿⣷⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⡟⠀⠀⢠⣿⠃⣼⣿⠀⠀⠀⠘⣿⠀⠸⡀
⢠⠞⠀⠀⠀⠛⣿⣿⣇⠸⣿⠀⠉⣿⠿⣶⣤⣤⣄⣀⣀⣀⣤⣤⣴⣿⠀⠀⢀⣿⡿⢰⡟⢻⣄⠀⠀⠀⢻⣷⠀⠙⠦⣤⠤⠖⠒⠲⣤
⣿⠀⣿⣿⣦⡀⠀⠈⣿⣆⢿⡀⣼⠃⠀⠀⠀⣾⠃⠀⣸⡿⠉⣰⣿⠀⠀⢀⣿⣿⢁⣿⣦⠀⠻⣄⠀⠀⠀⠙⢿⣶⣤⣀⣤⣴⣶⣆⠀⣷
⢻⠀⠙⣷⡉⠻⠿⠿⠛⠙⣿⣿⡁⠀⢀⣤⠟⠁⣀⣾⠟⠀⣴⡿⠀⠀⣀⣿⣿⣷⠿⠀⠻⣿⣤⡉⠿⣤⣀⠀⠀⠀⠈⠉⢁⣤⣿⠃⢀⠏
⠀⠻⣄⠀⠛⠿⠿⣿⠿⠟⠻⠿⢿⠿⠿⣶⠿⠿⠋⠀⠀⢾⣿⠿⠟⠛⠋⠁⠀⠙⠻⢿⣶⣶⡿⠟⠛⠛⠛⠛⠿⠿⠿⠟⠛⠁⢀⡴⠋
⠀⠀⠀⠉⠲⠤⣤⣤⡤⠴⠦⣤⣄⣤⢤⣤⣤⠴⠒⠓⠶⣄⣠⣤⠶⠒⠛⠛⠛⠒⠦⣤⣀⣀⣤⠤⠶⠶⠒⠲⠦⠤⠤⠶⠒⠋⠁

*/