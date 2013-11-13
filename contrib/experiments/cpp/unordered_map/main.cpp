#include<iostream>
#include<stdexcept>
#include<vector>
#include<sstream>
#include<boost/shared_array.hpp>
#include<boost/unordered_map.hpp>

const int LINE_SIZE = 1024;

using namespace std;
using namespace boost;

string help() {
    ostringstream os;
    os << "help #displays this help menu" << endl
            << "clear #clear map<string,string> strMap object" << endl
            << "put <key> <val> #put the key value in the map<string,string> strMap object" << endl
            << "get <key> #Get value in map map<string,string> strMap object" << endl
            << "keys #List keys and values map<string,string> strMap object" << endl
            << "del <key>#Delete the key from the map<string,string> strMap object" << endl
            << "exit #terminates this program" << endl;
    return os.str();
}

int string2vector(const string& strIn, vector<string>& strVector, char delim) {
    const int STRBUFFSIZE = 1024;
    int nStrings = 0;
    char buff[STRBUFFSIZE + 1];
    buff[0] = '\0';
    int ci = 0;
    int cb = 0;
    int cl;
    int li = strIn.size();
    for (ci = 0; ci <= li; ci++) {
        cl = ci - cb;
        if (strIn[ci] == delim || strIn[ci] == '\0' || cl >= STRBUFFSIZE) {
            if (cl <= 0) {
                cb = ci + 1;
                buff[0] = '\0';
                continue;
            }
            buff[ci - cb] = '\0';
            cb = ci + 1;
            strVector.push_back(string(buff));
            nStrings++;
            buff[0] = '\0';
            continue;
        }
        buff[ci - cb] = strIn[ci];
    }
    return nStrings;
}

int main(int argc, char **argv) {
    const string prompt("Silly Shell> ");
    boost::shared_array<char> cmd(new char[LINE_SIZE + 1]);
    vector<string> cmdArgs;
    unordered_map<string, string> strMap;
    int nArgs;

    do {
        try {
            cout << prompt;
            cmdArgs.clear();
            cin.getline(cmd.get(), LINE_SIZE);
            nArgs = string2vector(cmd.get(), cmdArgs, ' ');
            if (nArgs >= 1 && cmdArgs[0].compare("help") == 0) {
                cout << help() << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("exit") == 0) {
                break;
            } else if (nArgs >= 3 && cmdArgs[0].compare("put") == 0) {
                string key = cmdArgs[1];
                string val = cmdArgs[2];
                cout << "setting map[" << key << "] = " << val << endl;
                strMap[key] = val;
            } else if (nArgs >= 2 && cmdArgs[0].compare("get") == 0) {
                string key = cmdArgs[1];
                unordered_map<string, string>::iterator iter = strMap.find(key);
                if (strMap.find(key) == strMap.end()) {
                    cout << "Key " << key << " not found" << endl;
                } else {
                    cout << "key " << key << " = " << iter->second << endl;
                }

            } else if (nArgs >= 1 && cmdArgs[0].compare("keys") == 0) {
                unordered_map<string, string>::iterator mi;
                unordered_map<string, string>::iterator beg = strMap.begin();
                unordered_map<string, string>::iterator end = strMap.end();
                for (mi = beg; mi != end; mi++) {
                    cout << "(" << mi->first << "," << mi->second << ")" << endl;
                }
            } else if (nArgs >= 2 && cmdArgs[0].compare("del") == 0) {
                string key = cmdArgs[1];
                cout << "Deleting key " << key << endl;
                strMap.erase(key);
            } else if (nArgs >= 2 && cmdArgs[0].compare("clear") == 0) {
                cout << "Clearing str map" << endl;
                strMap.clear();
            } else {
                cout << "Unrecognized command  use help command for help" << endl;
            }

        } catch (std::exception& ex) {
            cout << "Exception: " << ex.what() << endl;
        }
    } while (!cin.eof());
    return 0;
}

