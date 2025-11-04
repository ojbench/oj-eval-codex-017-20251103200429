#include <iostream>
#include <cstring>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    string cmd;
    while (cin >> cmd) {
        if (cmd == "exit") {
            cout << "bye" << endl;
            break;
        }
        // Read and discard rest of line
        string line;
        getline(cin, line);
        cout << 0 << endl;
    }
    
    return 0;
}
