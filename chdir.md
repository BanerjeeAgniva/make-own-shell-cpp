

---

### **📂 `chdir()` System Call Documentation**
#### **📌 Overview**
The **`chdir()`** system call is used to **change the current working directory** of a process.

---

## **1️⃣ Function Signature**
```cpp
#include <unistd.h>

int chdir(const char *path);
```
### **🔹 Parameters**
- `path`: A **C-string** representing the directory to switch to.
- The path can be:
  - **Absolute** (e.g., `/usr/local/bin`).
  - **Relative** (e.g., `../folder_name`).

### **🔹 Return Value**
- `0` → Success (directory changed).
- `-1` → Failure (invalid path, permissions issue, etc.).

### **🔹 Possible `errno` Errors**
Use `strerror(errno)` to print error messages:

| Error Code | Meaning |
|------------|---------|
| `ENOENT` | No such file or directory |
| `EACCES` | Permission denied |
| `ENOTDIR` | Path is not a directory |

---

## **2️⃣ Example Usage**
```cpp
#include <iostream>
#include <unistd.h>  // chdir()
#include <cstring>   // strerror()
#include <cerrno>    // errno
#include <limits.h>  // PATH_MAX

using namespace std;

int main() {
    const char *path = "/usr/local/bin"; // Change to this directory

    if (chdir(path) == 0) {
        char cwd[PATH_MAX]; // Buffer to store current directory
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            cout << "Current Directory: " << cwd << endl;
        } else {
            perror("getcwd");
        }
    } else {
        cerr << "chdir failed: " << strerror(errno) << endl;
    }

    return 0;
}
```
### **💡 Output (if successful)**
```
Current Directory: /usr/local/bin
```
### **💡 Output (if the directory doesn’t exist)**
```
chdir failed: No such file or directory
```

---

## **3️⃣ Behavior of `chdir()`**
- It **only affects the current process**.
- If a **child process** calls `chdir()`, it does **not** affect the parent.
- **After calling `chdir()`, all relative paths** will be interpreted from the new directory.

### **🔹 Example: Impact on File Paths**
```cpp
chdir("/usr/local/bin");
ifstream file("myfile.txt");  // Now looks for /usr/local/bin/myfile.txt
```

---

## **4️⃣ Common Mistakes**
1. **Using `std::string` directly**  
   ```cpp
   string path = "/home/user";
   chdir(path.c_str());  // ✅ Use .c_str() to convert to C-string
   ```
2. **Forgetting to handle errors**  
   ```cpp
   if (chdir("/invalid/path") != 0) {
       perror("chdir");
   }
   ```
3. **Assuming `chdir()` affects parent process**
   - If you `fork()`, the new process keeps the changed directory, but the parent does **not**.

---

## **5️⃣ Alternative: `std::filesystem::current_path()` (C++17)**
For **modern C++**, `std::filesystem::current_path()` provides a safer way:
```cpp
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    fs::current_path("/usr/local/bin");  // Change directory
    std::cout << "Current Directory: " << fs::current_path() << std::endl;
    return 0;
}
```

✅ **Better error handling**  
✅ **Safer than `chdir()`**  
❌ **Requires C++17 or later**

---

## **📌 Summary**
| Feature       | `chdir()` |
|--------------|---------|
| Changes directory? | ✅ Yes |
| Affects parent process? | ❌ No |
| Works with relative & absolute paths? | ✅ Yes |
| Needs error handling? | ✅ Yes (`errno`) |
| Modern alternative? | `std::filesystem::current_path()` (C++17+) |

🚀 **Use `chdir()` if you're writing a shell or low-level program. Otherwise, prefer `std::filesystem` in modern C++.**
