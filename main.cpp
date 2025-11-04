#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>

using namespace std;

const int MAXN = 100005;

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

struct Ticket {
    char trainID[25], from[35], to[35];
    int seat, price, fromIdx, toIdx, date;
    bool pending;
};

struct Order {
    int id;
    char trainID[25], from[35], to[35];
    int fromIdx, toIdx, num, price, status, date;
    char username[25];
    bool exists;
};

class TicketSystem {
private:
    User users[MAXN];
    Train trains[MAXN];
    Order orders[MAXN * 10];
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
    
public:
    TicketSystem() : userCnt(0), trainCnt(0), orderCnt(0) {
        memset(loggedIn, 0, sizeof(loggedIn));
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
};

TicketSystem sys;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    string cmd;
    while (cin >> cmd) {
        if (cmd == "exit") {
            cout << "bye" << endl;
            break;
        } else if (cmd == "add_user") {
            char cu[25] = "", u[25], p[35], n[35], m[35];
            int g = 10;
            string k, v;
            while (cin >> k && k[0] == '-') {
                if (k == "-c") cin >> cu;
                else if (k == "-u") cin >> u;
                else if (k == "-p") cin >> p;
                else if (k == "-n") cin >> n;
                else if (k == "-m") cin >> m;
                else if (k == "-g") cin >> g;
            }
            cout << sys.addUser(cu, u, p, n, m, g) << endl;
        } else if (cmd == "login") {
            char u[25], p[35];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
                else if (k == "-p") cin >> p;
            }
            cout << sys.login(u, p) << endl;
        } else if (cmd == "logout") {
            char u[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-u") cin >> u;
            }
            cout << sys.logout(u) << endl;
        } else if (cmd == "query_profile") {
            char cu[25], u[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-c") cin >> cu;
                else if (k == "-u") cin >> u;
            }
            if (sys.queryProfile(cu, u) == -1) cout << -1 << endl;
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
            if (sys.modifyProfile(cu, u, p, n, m, g) == -1) cout << -1 << endl;
        } else if (cmd == "add_train") {
            char i[25], s[102][35], x[10], d1[10], d2[10], y;
            int n, m, p[102], t[102], o[102];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
                else if (k == "-n") cin >> n;
                else if (k == "-m") cin >> m;
                else if (k == "-s") for (int j = 0; j < n; j++) cin >> s[j];
                else if (k == "-p") for (int j = 0; j < n - 1; j++) cin >> p[j];
                else if (k == "-x") cin >> x;
                else if (k == "-t") for (int j = 0; j < n - 1; j++) cin >> t[j];
                else if (k == "-o") {
                    if (n == 2) { string tmp; cin >> tmp; }
                    else for (int j = 0; j < n - 2; j++) cin >> o[j];
                }
                else if (k == "-d") { cin >> d1 >> d2; }
                else if (k == "-y") cin >> y;
            }
            char* ss[102];
            for (int j = 0; j < n; j++) ss[j] = s[j];
            cout << sys.addTrain(i, n, m, ss, p, x, t, o, d1, d2, y) << endl;
        } else if (cmd == "release_train") {
            char i[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
            }
            cout << sys.releaseTrain(i) << endl;
        } else if (cmd == "query_train") {
            char i[25], d[10];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
                else if (k == "-d") cin >> d;
            }
            if (sys.queryTrain(i, d) == -1) cout << -1 << endl;
        } else if (cmd == "delete_train") {
            char i[25];
            string k;
            while (cin >> k && k[0] == '-') {
                if (k == "-i") cin >> i;
            }
            cout << sys.deleteTrain(i) << endl;
        } else {
            string line;
            getline(cin, line);
            cout << 0 << endl;
        }
    }
    
    return 0;
}
