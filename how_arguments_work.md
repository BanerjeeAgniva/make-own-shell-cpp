

```markdown
# Understanding Arguments in Command-Line Programs

## What is `alice`?

In the command:
```sh
$ custom_exe_1234 alice
```
**`alice` is an argument** passed to the executable `custom_exe_1234`.

---

## How Arguments Work

When you run a program from the command line, anything after the program name is passed as an **argument**.

### Example:
If you run:
```sh
ls -l /home/user
```
- `ls` is the command (executable).
- `-l` and `/home/user` are **arguments**.

---

## How It Works in the Test Program

In the sample C++ test program (`custom_exe_1234.cpp`):
```cpp
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    cout << "Program was passed " << argc << " args (including program name).\n";
    for (int i = 0; i < argc; i++) {
        cout << "Arg #" << i << ": " << argv[i] << "\n";
    }
    cout << "Program Signature: 5998595441\n";
    return 0;
}
```

### Command:
```sh
$ custom_exe_1234 alice
```

### Execution Details:
- `argc = 2` (argument count: program name + 1 argument).
- `argv[0] = "custom_exe_1234"` (the program name).
- `argv[1] = "alice"` (the first argument).

### Expected Output:
```plaintext
Program was passed 2 args (including program name).
Arg #0 (program name): custom_exe_1234
Arg #1: alice
Program Signature: 5998595441
```

---

## How It Relates to Your Shell

Your shell should **properly pass arguments** to external programs. For instance:

### Command:
```sh
$ custom_exe_1234 alice bob charlie
```

### Execution Details:
- `argc = 4` (program name + 3 arguments).
- `argv[0] = "custom_exe_1234"`
- `argv[1] = "alice"`
- `argv[2] = "bob"`
- `argv[3] = "charlie"`

This is why in your shell code, you must **convert the `vector<string>` of arguments into a `char*[]`** when using system calls like `execvp()`.

---

## TL;DR

- **`alice`** is just an example argument.
- Your shell should correctly handle arguments and pass them to external programs.
- Test with different arguments (e.g., `bob`, `test123`) to ensure your shell implementation behaves as expected.
```
