#include<iostream>
#include<stdexcept>
#include<vector>
#include<boost/shared_array.hpp>

const int LINE_SIZE = 1024;

using namespace std;

void help(ostream &os){
    os << "help #displays this help menu" << endl
       << "exit #terminates this program" << endl;
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

int main(int argc,char **argv){
    const string prompt("Silly Shell> ");
    boost::shared_array<char> cmd(new char[LINE_SIZE + 1]);
    vector<string> cmdArgs;
    int nArgs;

    do{
        try{
            cout << prompt;
            cmdArgs.clear();
            cin.getline(cmd.get(),LINE_SIZE);
            nArgs = string2vector(cmd.get(),cmdArgs,' ');
            if(nArgs >= 1 && cmdArgs[0].compare("help")==0){
                help(cout);
            }else if(nArgs >= 1 && cmdArgs[0].compare("exit")==0){
                break;
            }else{
                cout << "Unrecognized command  use help command for help" << endl;
            }

        }catch(std::exception& ex){
            cout << "Exception: " << ex.what() << endl;
        }
    }while(!cin.eof());
    return 0;
}

