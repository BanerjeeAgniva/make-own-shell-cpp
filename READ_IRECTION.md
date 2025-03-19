# Understanding Output Redirection in a Shell

## 💡 Basics of Redirection
- `command > file` → Redirects standard output **to a file** (overwriting it).
- `command 1> file` → **Same as above** (`1>` means standard output explicitly).
- `command >> file` → Appends standard output **to a file** instead of overwriting.

---

## 🔹 Example Walkthrough
Let's break down the provided test cases:

### **1️⃣ List directory contents and redirect output to a file**
```bash
ls /tmp/baz > /tmp/foo/baz.md
```
- `ls /tmp/baz` lists the contents of `/tmp/baz`.
- `>` redirects the output to `/tmp/foo/baz.md`.
- If `/tmp/foo/baz.md` **already exists, it is overwritten**.

#### ✅ Expected file output (`/tmp/foo/baz.md`):
```
apple
blueberry
```

---

### **2️⃣ Display file contents using `cat`**
```bash
cat /tmp/foo/baz.md
```
- Reads and prints the contents of `/tmp/foo/baz.md`.

#### ✅ Expected terminal output:
```
apple
blueberry
```

---

### **3️⃣ Echo text and redirect output**
```bash
echo 'Hello James' 1> /tmp/foo/foo.md
```
- `echo 'Hello James'` prints `"Hello James"`.
- `1>` explicitly redirects **standard output** to `/tmp/foo/foo.md`.
- **Same as:** `echo 'Hello James' > /tmp/foo/foo.md`.

#### ✅ Expected file output (`/tmp/foo/foo.md`):
```
Hello James
```

---

### **4️⃣ Handling errors while redirecting output**
```bash
cat /tmp/baz/blueberry nonexistent 1> /tmp/foo/quz.md
```
- `cat /tmp/baz/blueberry` **exists**, so its contents go to `/tmp/foo/quz.md`.
- `cat nonexistent` **does not exist**, so it prints an error **to standard error (stderr)**.
- `1>` redirects **only standard output (stdout)**, so **errors are NOT written to `/tmp/foo/quz.md`**.

#### ✅ Expected terminal output:
```
cat: nonexistent: No such file or directory
```

#### ✅ Expected file output (`/tmp/foo/quz.md`):
```
blueberry
```
_(Only the contents of `/tmp/baz/blueberry` were redirected, errors were not.)_

---

### **5️⃣ Displaying the redirected output**
```bash
cat /tmp/foo/quz.md
```
- Prints the contents of `/tmp/foo/quz.md`.

#### ✅ Expected terminal output:
```
blueberry
```

---

## 🚀 Summary
| Command | Expected Action |
|---------|----------------|
| `ls /tmp/baz > /tmp/foo/baz.md` | Writes `ls` output to `/tmp/foo/baz.md` |
| `cat /tmp/foo/baz.md` | Displays contents of `/tmp/foo/baz.md` |
| `echo 'Hello James' > /tmp/foo/foo.md` | Writes `"Hello James"` to `/tmp/foo/foo.md` |
| `cat /tmp/baz/blueberry nonexistent 1> /tmp/foo/quz.md` | Writes `blueberry` to `/tmp/foo/quz.md`, but errors are shown in terminal |
| `cat /tmp/foo/quz.md` | Displays contents of `/tmp/foo/quz.md` |

