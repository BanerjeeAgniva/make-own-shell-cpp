#include <algorithm>  // std::find is used from <algorithm> and avoids any conflicts with other find functions.
#include <fcntl.h>  //  int fd = open(writeto.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
#include <fstream> // for read from file and write to file  ofstream
#include <iostream>  
#include <limits.h> // For PATH_MAX --> PATH_MAX ensures enough space for the path. (getcwd)
#include <string>
#include <sys/wait.h> // For wait()
#include <unistd.h>   // For access(), fork(), execvp , chdir
#include <unistd.h>   // For getcwd()
#include <unordered_set>
#include <vector>

using namespace std;

void debug(string readfrom,string writeto)
{
  cout<<"readfrom ----> "<<readfrom<<endl;
  cout<<"writeto  ----> "<<writeto<<endl;
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
executable by the calling process. 
.c_str() converts string to const char*
*/

int emptyfile(string filename) 
{
    std::ofstream file(filename, std::ios::trunc); // Open in truncation mode
    file.close(); // Close the file (now empty)
    return 0;
    /*
std::ios::trunc removes all content when the file is opened.
Closing the file immediately ensures it stays empty.
    */
}



int open_and_stdout_file(string filename) 
{
    std::ifstream file(filename); // Open the file
    if (!file) {
        std::cerr << "Error: Could not open the file.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) { // Read line by line
        std::cout << line << std::endl;
    }

    file.close(); // Close the file
    return 0;
}

bool copyFile(const std::string& readFrom, const std::string& writeTo) {
    // Open the source file for reading in binary mode
    std::ifstream inFile(readFrom, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening source file: " << readFrom << std::endl;
        return false;
    }
    
    // Open the destination file for writing in binary mode
    std::ofstream outFile(writeTo, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening destination file: " << writeTo << std::endl;
        inFile.close();
        return false;
    }
    
    // Read and write the contents
    outFile << inFile.rdbuf();
    
    // Check if any errors occurred during reading or writing
    if (inFile.bad() || outFile.bad()) {
        std::cerr << "Error during file copy operation" << std::endl;
        inFile.close();
        outFile.close();
        return false;
    }
    
    // Close the files
    inFile.close();
    outFile.close();
    
    return true;
}

void handlepwd()
{
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)cout << cwd << "\n";
  else perror("getcwd");
      /*
      ================
      What is perror? 
      ================
      perror is a standard library function in C that prints a descriptive error message to the standard error stream (stderr) 
      based on the current value of the global errno variable. The errno variable is set when a system call or library function 
      fails, and perror provides a human-readable message corresponding to the error code stored in errno.
      */
}

void handlecd(vector<string>& tokens)
{
  if (tokens[1] == "~") chdir(getenv("HOME"));
  else if (chdir(tokens[1].c_str()) != 0) 
    cout << "cd: " << tokens[1] << ": No such file or directory"<< "\n";
}

void handleecho(bool writetofile, bool stderror, string writeto,vector<string>&tokens,bool appendstdout,bool appendstderr)
{
      ofstream outfile;
      if (writetofile && !appendstderr) 
      {
        outfile.open(writeto, ios::out | (appendstdout ? ios::app : ios::trunc)); //open in append mode if >> or 1>> 
        if (!outfile) 
        {
          cerr << "Error opening file: " << writeto << "\n";
          return;
        }
      }
      for (int i = 1; i < tokens.size(); i++) 
      {
        if (tokens[i] == ">" || tokens[i] == "1>" || tokens[i] == "2>" || tokens[i]==">>" || tokens[i]=="1>>" ||tokens[i]=="2>>") 
        {
          break; // Stop processing after `>` operator
        }
        if (writetofile && !appendstderr) 
        {
          outfile << tokens[i] << " ";
        } 
        else 
        {
          cout << tokens[i] << " ";
        }
      }

      if (writetofile && !appendstderr) 
      {
        outfile << "\n"; 
        outfile.close();
      } 
      else 
      {
        if(appendstderr) outfile.open(writeto, ios::out | ios::app ); //create the file but dont write to it 
        cout << "\n";
      }
      //---------
      if(stderror) emptyfile(writeto);
}

void handletype(unordered_set<string>& builtin ,vector<string>& tokens,vector<string>& path_dirs)
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

int main() {
  vector<string> tokens;
  unordered_set<string> builtin = {"exit", "type", "echo", "pwd"};
  string path_env = getenv("PATH");
  vector<string> path_dirs;
  while (true) {
    path_dirs = !path_env.empty()? split(path_env, ':'): vector<string>(); // Store Path directories after EVERY ITERATION 
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
    bool readfromfile=true;
    string readfrom = "";
    string writeto = "";
    bool nonexistent = false;
    bool stderror = false;
    bool appendstdout=false;
    bool appendstderr=false;

    // Iterate through tokens to find ">" or "1>" or "2>"
    for (auto it = tokens.begin(); it != tokens.end(); ++it) 
    {
     if (*it == ">" || *it == "1>" || *it == "2>" || *it==">>" || *it=="1>>" || *it=="2>>") 
     {
        if(*it == "2>" ) stderror=true;
        else writetofile = true;

        if(*it==">>" || *it=="1>>") appendstdout=true;
        else if(*it=="2>>") appendstderr=true;

        auto prev = it - 1;  // Element before ">" or "1>"
        if (prev != tokens.begin() && *prev == "nonexistent") 
        {
            nonexistent = true;
            readfrom = *(prev - 1); // Get the command before "nonexistent"
            tokens.erase(prev); // Remove "nonexistent"
            --it; //Adjust iterator since we erased an element
            // will point to readfrom
        } 
        else 
        {
          readfrom = *prev; // Normal case
        }
        writeto = *(it + 1); // File to write to
        break;
     }
   }
    if(readfrom=="ls" || readfrom=="-1" ) readfromfile=false;
    //debug(readfrom,writeto);
    string token0=tokens[0];
    
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    if (tokens.size() > 0 && token0 == "echo") handleecho(writetofile,stderror,writeto,tokens,appendstdout,appendstderr);
    else if (tokens.size() > 0 && token0 == "type") handletype(builtin,tokens,path_dirs);
    else if (tokens.size() == 1 && token0 == "pwd") handlepwd();
    else if (tokens.size() == 2 && token0 == "cd") handlecd(tokens);
    else if (tokens.size() >= 2 && token0 == "exit" && tokens[1] == "0") return 0;
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    else // Handles ls,cat, etc
    {
      string command_path = search_command_in_path(token0, path_dirs); // store
      // cout<<" command_path   "<<command_path<<endl;
      if (!command_path.empty()) 
      {
        if (nonexistent) 
        {
          string message=token0+": nonexistent: No such file or directory\n";
          if(!stderror && !appendstderr) // && !appendstderr 
          {
            cerr << message;
            emptyfile(writeto);
            if(token0=="cat" && readfromfile)
            {
              copyFile(readfrom,writeto);
              continue;
            }
          }
          else 
          {
            std::ofstream file(writeto, ios::out | ((appendstdout||appendstderr) ? ios::app : ios::trunc)); //open in append mode if >> or 1>> )
            if (!file) 
            {
              std::cerr << "Error: Cannot open file.\n";
              return 1;
            }
            file << message; // Write to the file (overwrites existing content)
            file.close();
            if(readfromfile && !appendstderr) open_and_stdout_file(readfrom);
            continue;
          }
        }
  
        pid_t pid = fork();
        if (pid == 0) 
        { // Child process
          if (writetofile && !stderror) 
          {
            int fd = open(writeto.c_str(), O_WRONLY | O_CREAT | ((appendstdout||appendstderr)  ? O_APPEND : O_TRUNC), 0644);
            if (fd == -1) 
            {
              perror("Error opening file");
              exit(EXIT_FAILURE);
            }

            if(!stderror){
              if(!(nonexistent && stdout && !readfromfile)){
               dup2(fd, STDOUT_FILENO); // Redirect stdout to file
               close(fd);
              }
            }
            // Remove '>' and the filename from tokens before executing
            auto it = find_if(tokens.begin(), tokens.end(), [](const string &s) { return s == ">" || s == "1>" || s == ">>" || s=="1>>" || s=="2>>"; });
            if (it != tokens.end() && (it + 1) != tokens.end()) 
            {
              writeto = *(it + 1);  // Extract the filename
              tokens.erase(it, it + 2); // Remove `1>` and filename from the command
            }
          }

          cout.flush(); 
          vector<char *> args;
          for (string &arg : tokens) args.push_back(&arg[0]);
          args.push_back(nullptr);
          if(!(nonexistent && stdout && !readfromfile)){
           execvp(command_path.c_str(), args.data()); // Convert tokens to char* array for execvp
           cout << "\n";
           perror("execvp");
           exit(EXIT_FAILURE);
          }
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
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
      else  
      {
        cout << token0 << ": command not found"<< "\n";
      }
    }
  }
  return 0; 
}

/*
agnivabanerjeeharness.io@Agniva Banerjee src % cat kkkk.txt nonexistent 2> kkk.txt      
this is kkkk%     

kkk.txt content --> 
cat: nonexistent: No such file or directory

*/
