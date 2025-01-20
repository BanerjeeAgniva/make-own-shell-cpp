#include <iostream>
#include <string>
#include <sys/wait.h> // For wait()
#include <unistd.h>   // For access(), fork(), execvp , chdir 
#include <unordered_set>
#include <vector>
#include <unistd.h> // For getcwd()
#include <limits.h> // For PATH_MAX --> PATH_MAX ensures enough space for the path. (getcwd)
using namespace std;

vector<string> split(string &str, char delimiter) {
  vector<string> tokens;
  string token="";
  bool singlequoteopen=false;
  bool doublequoteopen=false;
  bool preservenext=false;
  for (char ch : str) 
  {
    if(!doublequoteopen  && preservenext)
    {
      token+=ch;
      preservenext=false;
    }
    else if(ch=='\\')
    {
        preservenext=true;
    }
    else if(!doublequoteopen && ch=='\'') 
    {
        singlequoteopen=!singlequoteopen;
    } 
    else if(ch=='\"')
    {
        doublequoteopen=!doublequoteopen;
    }
    else if (ch == delimiter && !singlequoteopen && !doublequoteopen) 
    {
      if (!token.empty()) 
      {
        tokens.push_back(token);
        token.clear();
      }
    } 
    else 
    {
      token += ch;
    }
  }
  if (!token.empty()) 
  { 
    tokens.push_back(token);
  }
  return tokens;
}

string search_command_in_path(string command,vector<string> &PATH_directories) 
{
  for (string PATH_directory : PATH_directories) {
    string fullpath = PATH_directory + "/" + command;
    if (access(fullpath.c_str(), X_OK) == 0)
      return fullpath;
  }
  return "";
}

/*
int	 access(const char *, int);
access() is a system call that Returns 0 if the file exists and has the specified permission(X_OK = executable_okay?).
X_OK checks if the file is executable by the calling process.
.c_str() converts string to const char*
*/


int main() {
  vector<string> tokens;
  unordered_set<string> builtin = {"exit", "type", "echo","pwd"};
  string path_env = getenv("PATH");
  vector<string> path_dirs = !path_env.empty() ? split(path_env, ':') : vector<string>(); // Store Path directories  

  while (true) 
  {
    cout << unitbuf; 
    tokens.clear();
    cout << "$ ";
    string input; 
    getline(cin, input);
    tokens = split(input, ' ');
    if(tokens.size()==1 && tokens[0]=="pwd")
    {
       char cwd[PATH_MAX];
       if(getcwd(cwd,sizeof(cwd))!=NULL) cout<<cwd<<"\n";
       else perror("getcwd");
    }
    else if(tokens.size()==2 && tokens[0]=="cd")
    {
        if(tokens[1]=="~") chdir(getenv("HOME")) ; 
        else if(chdir(tokens[1].c_str())!=0)
        {
            cout<<"cd: "<<tokens[1]<<": No such file or directory"<<"\n";
        }
    }
    else if (tokens.size() >= 2 && tokens[0] == "exit" && tokens[1] == "0") return 0;
    else if (tokens.size() > 0 && tokens[0] == "echo") 
    {
      for (int i = 1; i < tokens.size(); i++) cout << tokens[i] << " ";
      cout << "\n";
      continue;
    } 
    else if (tokens.size() > 0 && tokens[0] == "type") 
    {
      for (int i = 1; i < tokens.size(); i++) 
      {
        if (builtin.count(tokens[i])) cout << tokens[i] << " is a shell builtin"<< "\n";
        else 
        {
          string found_path = search_command_in_path(tokens[i], path_dirs);
          if (!found_path.empty()) cout << tokens[i] << " is " << found_path << "\n";
          else cout << tokens[i] << ": not found"<< "\n";
        }
      }
    } 
    else 
    {
      string command_path = search_command_in_path(tokens[0], path_dirs); // store 
      if (!command_path.empty()) 
      {
        pid_t pid = fork();
        if (pid == 0) // Child process
        { 
          vector<char *> args;
          for (string &arg : tokens) args.push_back(&arg[0]);
          args.push_back(nullptr); // Null-terminate the array
          execvp(command_path.c_str(), args.data()); // what is execvp? ---> execvp.md 
          //  /usr/local/bin/custom_exe_1234 alice 

          perror("execvp"); // Print error if execvp fails
          exit(EXIT_FAILURE); // Exit the child process with failure status

        } 
        else if (pid > 0)  
        { // Parent process
          int status;
          waitpid(pid, &status, 0); // Wait for child to finish
          /*
          pid > 0 means we're in the parent process.
          waitpid(pid, &status, 0) makes the parent wait for the child to finish execution.
          This prevents the parent from continuing execution before the child completes.
          */
         /*
         status is not initialized because waitpid() fills it with the child's exit status.
         */
        } 
        else 
        {
          perror("fork");
        }
      } 
      else 
      {
        cout << tokens[0] << ": command not found"<< "\n";
      }
    }
  }
}
