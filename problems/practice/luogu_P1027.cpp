#include <bits/stdc++.h>
using namespace std;

struct point{
public:
    int x,y;
};

double cal_dis(point& a, point& b){
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin>>n;
    while(n--){
        int s,t,a,b;
        cin>>s>>t>>a>>b;

        vector<vector<double> > adj(s*4 + 10, vector<double> (s*4 + 10, 1e9));
        vector<vector<point> > airport(s, vector<point> (4));
        for (int i = 0; i < s; ++i){
            
            for (int j = 0; j < 3; ++j){
                cin>>airport[i][j].x>>airport[i][j].y;
            }
            int tt;
            cin>>tt;
            int tangle;
            double mx = -1;
            for (int j = 0; j < 3; ++j){
                for (int k = 0; k < 3; ++k){
                    adj[i*4+j][i*4+k] = tt * cal_dis(airport[i][j], airport[i][k]);
                    adj[i*4+k][i*4+j] = adj[i*4+j][i*4+k];
                    if (mx < adj[i*4+j][i*4+k]){
                        mx = adj[i*4+j][i*4+k];
                        tangle = 3-j-k;
                    }
                }
            }

            airport[i][3].x = airport[i][(tangle+1)%3].x + airport[i][(tangle+2)%3].x - airport[i][tangle].x;
            airport[i][3].y = airport[i][(tangle+1)%3].y + airport[i][(tangle+2)%3].y - airport[i][tangle].y;
            for (int j = 0; j < 4; ++j){
                adj[i*4+j][i*4+3] = tt * cal_dis(airport[i][3], airport[i][j]);
                adj[i*4+3][i*4+j] = adj[i*4+j][i*4+3];
            }
        }

        for (int ai = 0; ai < s; ++ai){
            for (int bi = 0; bi < s; ++bi){
                for(int aj = 0; aj < 4; ++aj){
                    for (int bj = 0; bj < 4; ++bj){
                        adj[ai*4+aj][bi*4+bj] = min(adj[ai*4+aj][bi*4+bj], t * cal_dis(airport[ai][aj], airport[bi][bj]));
                    }
                }
            }
        }

        for (int k = 0; k < s*4; ++k){
            for (int i = 0; i < s*4; ++i){
                for (int j = 0; j < s*4; ++j){
                    adj[i][j] = min(adj[i][j], adj[i][k] + adj[k][j]);
                }
            }
        }

        double ans = 1e9;
        for (int i = 0; i < 4; ++i){
            for (int j = 0; j < 4; ++j){
                ans = min(adj[(a-1)*4+i][(b-1)*4+j], ans);
                //cout<<ans<<endl;
            }
        }

        cout<<fixed<<setprecision(1)<<ans<<endl;
    }
    
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