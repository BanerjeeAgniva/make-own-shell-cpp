#include <algorithm>
#include <fcntl.h>
#include <fstream> // for read from file and write to file
#include <iostream>
#include <limits.h> // For PATH_MAX --> PATH_MAX ensures enough space for the path. (getcwd)
#include <string>
#include <sys/wait.h> // For wait()
#include <unistd.h>   // For access(), fork(), execvp , chdir
#include <unistd.h>   // For getcwd()
#include <unordered_set>
#include <vector>

using namespace std;

vector<string> split(string &str, char delimiter) 
{
  vector<string> tokens;
  string token = "";
  bool singlequoteopen = false, doublequoteopen = false, escaped = false;
  for (int i = 0; i < str.size(); i++) 
  {
    char ch = str[i];
    if (singlequoteopen) 
    {
      if (ch != '\'')
        token += ch;
      else
        singlequoteopen = !singlequoteopen;
      /*
      previously -->
      '/tmp/foo/"f 47"' ---> tmp/foo/f 47
      corrected -----------> tmp/foo/"f 47"
      */
    } 
    else if (escaped) // If the previous character was '\', treat this char normally
    {
      if (doublequoteopen && ch != '"' && ch != '\\') //   - - - - - - - - > important !!
      {
        // if doublequotes are open and they are not being closed right now
        token += '\\'; // Preserve extra backslash inside double quotes
      } 
      else if (singlequoteopen && ch != '\'' && ch != '\\')
      {
        token += '\\';
      }
      token += ch;
      escaped = false;
    } 
    else if (ch == '\\') 
    {
      escaped = true; // Set flag to escape next character
      if (singlequoteopen)
        token += ch; // ----> maintain character if single quotes are open
    } 
    else if (ch == '\'') 
    {
      if (!doublequoteopen)
        singlequoteopen = !singlequoteopen;
      else
        token += ch; // Treat single quote as normal inside double quotes
    } 
    else if (ch == '"') 
    {
      doublequoteopen = !doublequoteopen;
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
    // cout<<token<<endl;
  }

  if (!token.empty()) {
    tokens.push_back(token);
  }

  return tokens;
}

string search_command_in_path(string command,vector<string> &PATH_directories) {
  for (string PATH_directory : PATH_directories) {
    string fullpath = PATH_directory + "/" + command;
    if (access(fullpath.c_str(), X_OK) == 0) {
      return fullpath;
    }
  }
  return "";
}

/*
int	 access(const char *, int);
access() is a system call that Returns 0 if the file exists and has the
specified permission(X_OK = executable_okay?). X_OK checks if the file is
executable by the calling process. .c_str() converts string to const char*
*/

int main() {
  vector<string> tokens;
  unordered_set<string> builtin = {"exit", "type", "echo", "pwd"};
  string path_env = getenv("PATH");
  vector<string> path_dirs = !path_env.empty()? split(path_env, ':'): vector<string>(); // Store Path directories

  while (true) {
    cout << unitbuf;
    tokens.clear();
    cout << "$ ";
    string input;
    getline(cin, input);
    tokens = split(input, ' ');
    // cout << "Final command tokens:";
    // for (const auto& token : tokens) cout << " [" << token << "]";
    // cout << endl;
    bool writetofile = false;
    string writefrom = "";
    string writeto = "";
    bool nonexistent = false;

    // Iterate through tokens to find ">" or "1>"
    for (auto it = tokens.begin(); it != tokens.end(); ++it) 
    {
     if (*it == ">" || *it == "1>") 
     {
        writetofile = true;
        auto prev = it - 1;  // Element before ">" or "1>"

        if (prev != tokens.begin() && *prev == "nonexistent") 
        {
            nonexistent = true;
            writefrom = *(prev - 1); // Get the command before "nonexistent"
            tokens.erase(prev); // Remove "nonexistent"
            --it; // Adjust iterator since we erased an element
        } 
        else 
        {
          writefrom = *prev; // Normal case
        }

        writeto = *(it + 1); // File to write to
        break;
     }
   }

    if (tokens.size() == 1 && tokens[0] == "pwd") 
    {
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL)
        cout << cwd << "\n";
      else
        perror("getcwd");
    } 
    else if (tokens.size() == 2 && tokens[0] == "cd") 
    {
      if (tokens[1] == "~") 
      {
        chdir(getenv("HOME"));
      } 
      else if (chdir(tokens[1].c_str()) != 0) 
      {
        cout << "cd: " << tokens[1] << ": No such file or directory"<< "\n";
      }
    } 
    else if (tokens.size() >= 2 && tokens[0] == "exit" && tokens[1] == "0") {
      return 0;
    } 
    else if (tokens.size() > 0 && tokens[0] == "echo") 
    {
      ofstream outfile;
      if (writetofile) 
      {
        outfile.open(writeto, ios::out | ios::trunc);
        if (!outfile) 
        {
          cerr << "Error opening file: " << writeto << "\n";
          continue;
        }
      }
      for (int i = 1; i < tokens.size(); i++) 
      {
        if (tokens[i] == ">" || tokens[i] == "1>") 
        {
          break; // Stop processing after `>` operator
        }
        if (writetofile) 
        {
          outfile << tokens[i] << " ";
        } 
        else 
        {
          cout << tokens[i] << " ";
        }
      }

      if (writetofile) 
      {
        outfile << "\n"; // Ensure newline in the file
        outfile.close();
      } 
      else 
      {
        cout << "\n";
      }
    } 
    else if (tokens.size() > 0 && tokens[0] == "type") 
    {
      for (int i = 1; i < tokens.size(); i++) 
      {
        if (builtin.count(tokens[i])) 
        {
          cout << tokens[i] << " is a shell builtin"<< "\n";
        }
        else 
        {
          string found_path = search_command_in_path(tokens[i], path_dirs);
          if (!found_path.empty()) 
          {
            cout << tokens[i] << " is " << found_path << "\n";
          }
          else 
          {
            cout << tokens[i] << ": not found"<< "\n";
          }
        }
      }
    } 
    else // Handles ls,cat, etc
    {
      string command_path = search_command_in_path(tokens[0], path_dirs); // store
      // cout<<" command_path   "<<command_path<<endl;
      if (!command_path.empty()) 
      {
        if (nonexistent) 
        {
         cerr << "cat: nonexistent: No such file or directory\n";
         //continue; // Skip execution
        }
        pid_t pid = fork();
        if (pid == 0) 
        { // Child process
          if (writetofile) 
          {
            int fd = open(writeto.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) 
            {
              perror("Error opening file");
              exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Redirect stdout to file
            close(fd);
            // Remove '>' and the filename from tokens before executing
            auto it = find_if(tokens.begin(), tokens.end(), [](const string &s) { return s == ">" || s == "1>"; });
            if (it != tokens.end() && (it + 1) != tokens.end()) 
            {
              writeto = *(it + 1);  // Extract the filename
              tokens.erase(it, it + 2); // Remove `1>` and filename from the command
            }
          }
          cout.flush(); 

          // Convert tokens to char* array for execvp
          vector<char *> args;
          for (string &arg : tokens) args.push_back(&arg[0]);
          args.push_back(nullptr);
          execvp(command_path.c_str(), args.data());
          cout << "\n";
          perror("execvp");
          exit(EXIT_FAILURE);
        } 
        else if (pid > 0) 
        { // Parent process
          int status;
          waitpid(pid, &status, 0);
        } 
        else 
        {
          perror("fork");
        }
      } 
      else cout << tokens[0] << ": command not found"<< "\n";
    }
  }
  return 0;
}
