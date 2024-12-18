#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unistd.h>   // For access()
using namespace std;

vector<string> split(string &str, char delimiter) {
    vector<string> tokens;
    string token;
    for (char ch : str) {
        if (ch == delimiter) {
            if (!token.empty()) 
            {
                tokens.push_back(token); 
                token.clear();           
            }
        } 
        else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token); 
    }
    return tokens;
}

string search_command_in_path(string command,vector<string>& PATH_directories){
    for(string PATH_directory : PATH_directories){
      string fullpath=PATH_directory+"/"+command;
      if(access(fullpath.c_str(),X_OK)==0) return fullpath;
    }
    return "";
}
        
        /*
        int	 access(const char *, int);
        access() is a system call that:
        Returns 0 if the file exists and has the specified permission.
        Here, X_OK checks if the file is executable by the calling process.
        // .c_str() converts to const char*
        */

int main() {
    vector<string> v;
    unordered_set<string> builtin;
    builtin.insert("exit");
    builtin.insert("type");
    builtin.insert("echo");

    string path_env = getenv("PATH");
    vector<string> path_dirs = !path_env.empty() ? split(path_env, ':') : vector<string>();
    while (true) {
        cout << unitbuf;
        v.clear();
        cout << "$ ";  
        string input;
        getline(cin, input);
        v=split(input,' ');

        if(v.size()>=2 && v[0]=="exit" && v[1]=="0") return 0;
        else if (v.size() > 0 && v[0] == "echo") {
            for (int i = 1; i < v.size(); i++) cout << v[i] << " ";
            cout << "\n";
            continue;
        }
        else if (v.size() > 0 && v[0] == "type") 
        {
            for (int i = 1; i < v.size(); i++) {
                if (builtin.count(v[i])) 
                {
                    cout << v[i] << " is a shell builtin" << "\n";
                } 
                else 
                {
                    string found_path = search_command_in_path(v[i], path_dirs); // <---- IMP
                    if (!found_path.empty()) {
                        cout << v[i] << " is " << found_path << "\n";
                    } 
                    else {
                        cout << v[i] << ": not found" << "\n";
                    }
                }
            }
        } 
        else 
        {
            cout << input << ": command not found" << "\n";
        }
    }
}

