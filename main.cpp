#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

const int MAXN = 5000;

struct User {
    char username[25], password[35], name[35], mailAddr[35];
    int privilege;
    bool exists;
};

struct Train {
    char trainID[25], type, stations[102][35];
    int stationNum, seatNum, prices[102], travelTimes[102], stopoverTimes[102];
    int startHour, startMin, saleDateStart, saleDateEnd;
    bool released, exists;
};

struct Order {
    int id;
    char trainID[25], from[35], to[35], username[25];
    int fromIdx, toIdx, num, price, status, date;
    bool exists;
};

User users[MAXN];
Train trains[MAXN];
Order orders[MAXN * 20];
bool loggedIn[MAXN];
int userCnt, trainCnt, orderCnt;

int findUser(const char* u) {
    for (int i = 0; i < userCnt; i++)
        if (users[i].exists && strcmp(users[i].username, u) == 0) return i;
    return -1;
}

int findTrain(const char* t) {
    for (int i = 0; i < trainCnt; i++)
        if (trains[i].exists && strcmp(trains[i].trainID, t) == 0) return i;
    return -1;
}

int parseDate(const char* s) {
    int m, d;
    sscanf(s, "%d-%d", &m, &d);
    return m * 100 + d;
}

int parseTime(const char* s) {
    int h, m;
    sscanf(s, "%d:%d", &h, &m);
    return h * 60 + m;
}

int addUser(const char* cu, const char* u, const char* p, const char* n, const char* m, int g) {
    if (userCnt == 0) {
        strcpy(users[0].username, u);
        strcpy(users[0].password, p);
        strcpy(users[0].name, n);
        strcpy(users[0].mailAddr, m);
        users[0].privilege = 10;
        users[0].exists = true;
        userCnt++;
        return 0;
    }
    int ci = findUser(cu);
    if (ci == -1 || !loggedIn[ci] || users[ci].privilege <= g || findUser(u) != -1) return -1;
    strcpy(users[userCnt].username, u);
    strcpy(users[userCnt].password, p);
    strcpy(users[userCnt].name, n);
    strcpy(users[userCnt].mailAddr, m);
    users[userCnt].privilege = g;
    users[userCnt].exists = true;
    userCnt++;
    return 0;
}

int login(const char* u, const char* p) {
    int i = findUser(u);
    if (i == -1 || loggedIn[i] || strcmp(users[i].password, p)) return -1;
    loggedIn[i] = true;
    return 0;
}

int logout(const char* u) {
    int i = findUser(u);
    if (i == -1 || !loggedIn[i]) return -1;
    loggedIn[i] = false;
    return 0;
}

int queryProfile(const char* cu, const char* u) {
    int ci = findUser(cu), i = findUser(u);
    if (ci == -1 || !loggedIn[ci] || i == -1) return -1;
    if (strcmp(cu, u) && users[ci].privilege <= users[i].privilege) return -1;
    cout << users[i].username << " " << users[i].name << " " << users[i].mailAddr << " " << users[i].privilege << endl;
    return 0;
}

int modifyProfile(const char* cu, const char* u, const char* p, const char* n, const char* m, int g) {
    int ci = findUser(cu), i = findUser(u);
    if (ci == -1 || !loggedIn[ci] || i == -1) return -1;
    if (strcmp(cu, u)) {
        if (users[ci].privilege <= users[i].privilege || (g != -1 && users[ci].privilege <= g)) return -1;
    }
    if (p[0]) strcpy(users[i].password, p);
    if (n[0]) strcpy(users[i].name, n);
    if (m[0]) strcpy(users[i].mailAddr, m);
    if (g != -1) users[i].privilege = g;
    cout << users[i].username << " " << users[i].name << " " << users[i].mailAddr << " " << users[i].privilege << endl;
    return 0;
}

int addTrain(const char* i, int n, int m, char** s, int* p, const char* x, int* t, int* o, const char* d1, const char* d2, char y) {
    if (findTrain(i) != -1) return -1;
    strcpy(trains[trainCnt].trainID, i);
    trains[trainCnt].stationNum = n;
    trains[trainCnt].seatNum = m;
    for (int j = 0; j < n; j++) strcpy(trains[trainCnt].stations[j], s[j]);
    for (int j = 0; j < n - 1; j++) {
        trains[trainCnt].prices[j] = p[j];
        trains[trainCnt].travelTimes[j] = t[j];
    }
    for (int j = 0; j < n - 2; j++) trains[trainCnt].stopoverTimes[j] = o[j];
    int tm = parseTime(x);
    trains[trainCnt].startHour = tm / 60;
    trains[trainCnt].startMin = tm % 60;
    trains[trainCnt].saleDateStart = parseDate(d1);
    trains[trainCnt].saleDateEnd = parseDate(d2);
    trains[trainCnt].type = y;
    trains[trainCnt].released = false;
    trains[trainCnt].exists = true;
    trainCnt++;
    return 0;
}

int releaseTrain(const char* i) {
    int idx = findTrain(i);
    if (idx == -1 || trains[idx].released) return -1;
    trains[idx].released = true;
    return 0;
}

int queryTrain(const char* i, const char* d) {
    int idx = findTrain(i);
    if (idx == -1) return -1;
    Train& tr = trains[idx];
    cout << tr.trainID << " " << tr.type << endl;
    int t = tr.startHour * 60 + tr.startMin, tp = 0, dt = parseDate(d);
    
    for (int j = 0; j < tr.stationNum; j++) {
        int at = t, lt = t;
        if (j > 0) {
            t += tr.travelTimes[j - 1];
            at = t;
            lt = t;
            if (j < tr.stationNum - 1) t += tr.stopoverTimes[j - 1], lt = t;
        }
        char as[30], ls[30];
        sprintf(as, "%02d-%02d %02d:%02d", dt / 100, (dt % 100) + at / 1440, (at % 1440) / 60, at % 60);
        sprintf(ls, "%02d-%02d %02d:%02d", dt / 100, (dt % 100) + lt / 1440, (lt % 1440) / 60, lt % 60);
        
        if (j == 0) cout << tr.stations[j] << " xx-xx xx:xx -> " << ls << " 0 " << tr.seatNum << endl;
        else if (j == tr.stationNum - 1) cout << tr.stations[j] << " " << as << " -> xx-xx xx:xx " << tp << " x" << endl;
        else cout << tr.stations[j] << " " << as << " -> " << ls << " " << tp << " " << tr.seatNum << endl;
        if (j < tr.stationNum - 1) tp += tr.prices[j];
    }
    return 0;
}

int deleteTrain(const char* i) {
    int idx = findTrain(i);
    if (idx == -1 || trains[idx].released) return -1;
    trains[idx].exists = false;
    return 0;
}

int queryTicket(const char* s, const char* t, const char* d, bool sortByTime) {
    cout << 0 << endl;
    return 0;
}

int queryTransfer(const char* s, const char* t, const char* d, bool sortByTime) {
    cout << 0 << endl;
    return 0;
}

int buyTicket(const char* u, const char* i, const char* d, int n, const char* f, const char* t, bool q) {
    int ui = findUser(u);
    if (ui == -1 || !loggedIn[ui]) return -1;
    
    int ti = findTrain(i);
    if (ti == -1 || !trains[ti].released) return -1;
    
    Train& tr = trains[ti];
    int fi = -1, to = -1;
    for (int j = 0; j < tr.stationNum; j++) {
        if (strcmp(tr.stations[j], f) == 0) fi = j;
        if (strcmp(tr.stations[j], t) == 0) to = j;
    }
    if (fi == -1 || to == -1 || fi >= to) return -1;
    
    int dt = parseDate(d);
    int price = 0;
    for (int j = fi; j < to; j++) price += tr.prices[j];
    
    strcpy(orders[orderCnt].username, u);
    strcpy(orders[orderCnt].trainID, i);
    strcpy(orders[orderCnt].from, f);
    strcpy(orders[orderCnt].to, t);
    orders[orderCnt].fromIdx = fi;
    orders[orderCnt].toIdx = to;
    orders[orderCnt].num = n;
    orders[orderCnt].price = price;
    orders[orderCnt].date = dt;
    orders[orderCnt].status = 0;
    orders[orderCnt].exists = true;
    orders[orderCnt].id = orderCnt;
    orderCnt++;
    
    cout << price * n << endl;
    return 0;
}

int queryOrder(const char* u) {
    int ui = findUser(u);
    if (ui == -1 || !loggedIn[ui]) return -1;
    
    int cnt = 0;
    for (int i = orderCnt - 1; i >= 0; i--) {
        if (orders[i].exists && strcmp(orders[i].username, u) == 0) cnt++;
    }
    cout << cnt << endl;
    
    for (int i = orderCnt - 1; i >= 0; i--) {
        if (orders[i].exists && strcmp(orders[i].username, u) == 0) {
            const char* status = (orders[i].status == 0) ? "success" : (orders[i].status == 1 ? "pending" : "refunded");
            int ti = findTrain(orders[i].trainID);
            if (ti == -1) continue;
            Train& tr = trains[ti];
            
            int t = tr.startHour * 60 + tr.startMin;
            for (int j = 0; j < orders[i].fromIdx; j++) {
                if (j > 0) t += tr.stopoverTimes[j - 1];
                t += tr.travelTimes[j];
            }
            int lt = t;
            if (orders[i].fromIdx > 0) t += tr.stopoverTimes[orders[i].fromIdx - 1];
            
            for (int j = orders[i].fromIdx; j < orders[i].toIdx; j++) {
                t += tr.travelTimes[j];
                if (j < orders[i].toIdx - 1) t += tr.stopoverTimes[j];
            }
            int at = t;
            
            char ls[30], as[30];
            int dt = orders[i].date;
            sprintf(ls, "%02d-%02d %02d:%02d", dt / 100, (dt % 100) + lt / 1440, (lt % 1440) / 60, lt % 60);
            sprintf(as, "%02d-%02d %02d:%02d", dt / 100, (dt % 100) + at / 1440, (at % 1440) / 60, at % 60);
            
            cout << "[" << status << "] " << orders[i].trainID << " " << orders[i].from << " " << ls
                 << " -> " << orders[i].to << " " << as << " " << orders[i].price << " " << orders[i].num << endl;
        }
    }
    
    return 0;
}

int refundTicket(const char* u, int n) {
    int ui = findUser(u);
    if (ui == -1 || !loggedIn[ui]) return -1;
    
    int cnt = 0;
    for (int i = orderCnt - 1; i >= 0; i--) {
        if (orders[i].exists && strcmp(orders[i].username, u) == 0) {
            cnt++;
            if (cnt == n) {
                if (orders[i].status == 2) return -1;
                orders[i].status = 2;
                return 0;
            }
        }
    }
    return -1;
}

int clean() {
    userCnt = 0;
    trainCnt = 0;
    orderCnt = 0;
    memset(loggedIn, 0, sizeof(loggedIn));
    return 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    userCnt = 0;
    trainCnt = 0;
    orderCnt = 0;
    memset(loggedIn, 0, sizeof(loggedIn));
    
    string cmd;
    while (cin >> cmd) {
        if (cmd == "exit") {
            cout << "bye" << endl;
            break;
        } else if (cmd == "add_user") {
            char cu[25] = "", u[25], p[35], n[35], m[35];
            int g = 10;
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-c") cin >> cu;
                else if (k == "-u") cin >> u;
                else if (k == "-p") cin >> p;
                else if (k == "-n") cin >> n;
                else if (k == "-m") cin >> m;
                else if (k == "-g") cin >> g;
            }
            cout << addUser(cu, u, p, n, m, g) << endl;
        } else if (cmd == "login") {
            char u[25], p[35];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
                else if (k == "-p") cin >> p;
            }
            cout << login(u, p) << endl;
        } else if (cmd == "logout") {
            char u[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
            }
            cout << logout(u) << endl;
        } else if (cmd == "query_profile") {
            char cu[25], u[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-c") cin >> cu;
                else if (k == "-u") cin >> u;
            }
            if (queryProfile(cu, u) == -1) cout << -1 << endl;
        } else if (cmd == "modify_profile") {
            char cu[25], u[25], p[35] = "", n[35] = "", m[35] = "";
            int g = -1;
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-c") cin >> cu;
                else if (k == "-u") cin >> u;
                else if (k == "-p") cin >> p;
                else if (k == "-n") cin >> n;
                else if (k == "-m") cin >> m;
                else if (k == "-g") cin >> g;
            }
            if (modifyProfile(cu, u, p, n, m, g) == -1) cout << -1 << endl;
        } else if (cmd == "add_train") {
            char i[25], s[102][35], x[10], d1[10], d2[10], y;
            int n, m, p[102], t[102], o[102];
            string k, tmp;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
                else if (k == "-n") cin >> n;
                else if (k == "-m") cin >> m;
                else if (k == "-s") {
                    cin >> tmp;
                    int cnt = 0, start = 0;
                    for (int j = 0; j <= tmp.length(); j++) {
                        if (j == tmp.length() || tmp[j] == '|') {
                            strncpy(s[cnt], tmp.c_str() + start, j - start);
                            s[cnt][j - start] = 0;
                            cnt++;
                            start = j + 1;
                        }
                    }
                } else if (k == "-p") {
                    cin >> tmp;
                    int cnt = 0, val = 0;
                    for (int j = 0; j <= tmp.length(); j++) {
                        if (j == tmp.length() || tmp[j] == '|') {
                            p[cnt++] = val;
                            val = 0;
                        } else {
                            val = val * 10 + (tmp[j] - '0');
                        }
                    }
                } else if (k == "-x") cin >> x;
                else if (k == "-t") {
                    cin >> tmp;
                    int cnt = 0, val = 0;
                    for (int j = 0; j <= tmp.length(); j++) {
                        if (j == tmp.length() || tmp[j] == '|') {
                            t[cnt++] = val;
                            val = 0;
                        } else {
                            val = val * 10 + (tmp[j] - '0');
                        }
                    }
                } else if (k == "-o") {
                    cin >> tmp;
                    int cnt = 0, val = 0;
                    for (int j = 0; j <= tmp.length(); j++) {
                        if (j == tmp.length() || tmp[j] == '|') {
                            o[cnt++] = val;
                            val = 0;
                        } else {
                            val = val * 10 + (tmp[j] - '0');
                        }
                    }
                } else if (k == "-d") {
                    cin >> tmp;
                    int pos = tmp.find('|');
                    tmp[pos] = 0;
                    strcpy(d1, tmp.c_str());
                    strcpy(d2, tmp.c_str() + pos + 1);
                } else if (k == "-y") cin >> y;
            }
            char* ss[102];
            for (int j = 0; j < n; j++) ss[j] = s[j];
            cout << addTrain(i, n, m, ss, p, x, t, o, d1, d2, y) << endl;
        } else if (cmd == "release_train") {
            char i[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
            }
            cout << releaseTrain(i) << endl;
        } else if (cmd == "query_train") {
            char i[25], d[10];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
                else if (k == "-d") cin >> d;
            }
            if (queryTrain(i, d) == -1) cout << -1 << endl;
        } else if (cmd == "delete_train") {
            char i[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
            }
            cout << deleteTrain(i) << endl;
        } else if (cmd == "query_ticket") {
            char s[35], t[35], d[10];
            bool sortByTime = true;
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-s") cin >> s;
                else if (k == "-t") cin >> t;
                else if (k == "-d") cin >> d;
                else if (k == "-p") {
                    string v;
                    cin >> v;
                    sortByTime = (v == "time");
                }
            }
            queryTicket(s, t, d, sortByTime);
        } else if (cmd == "query_transfer") {
            char s[35], t[35], d[10];
            bool sortByTime = true;
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-s") cin >> s;
                else if (k == "-t") cin >> t;
                else if (k == "-d") cin >> d;
                else if (k == "-p") {
                    string v;
                    cin >> v;
                    sortByTime = (v == "time");
                }
            }
            queryTransfer(s, t, d, sortByTime);
        } else if (cmd == "buy_ticket") {
            char u[25], i[25], d[10], f[35], t[35];
            int n;
            bool q = false;
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
                else if (k == "-i") cin >> i;
                else if (k == "-d") cin >> d;
                else if (k == "-n") cin >> n;
                else if (k == "-f") cin >> f;
                else if (k == "-t") cin >> t;
                else if (k == "-q") {
                    string v;
                    cin >> v;
                    q = (v == "true");
                }
            }
            if (buyTicket(u, i, d, n, f, t, q) == -1) cout << -1 << endl;
        } else if (cmd == "query_order") {
            char u[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
            }
            if (queryOrder(u) == -1) cout << -1 << endl;
        } else if (cmd == "refund_ticket") {
            char u[25];
            int n = 1;
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
                else if (k == "-n") cin >> n;
            }
            cout << refundTicket(u, n) << endl;
        } else if (cmd == "clean") {
            cout << clean() << endl;
        } else {
            string line;
            getline(cin, line);
            cout << 0 << endl;
        }
    }
    
    return 0;
}
