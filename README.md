# list_directory

**list_directory** is a command-line utility written in C that allows you to list the contents of a directory with various options for customization. It provides features similar to the `ls` command in Unix-like systems but with additional functionality such as sorting, filtering, and colored output.

---

## Features

- **Directory Listing**: List files and directories in a specified directory.
- **Custom Sorting**: Sort files by modification time, access time, or change time.
- **Filtering**: Display only files, only directories, or hidden files.
- **Colored Output**: Differentiate between files, directories, and executable files using colors.
- **Detailed Information**: Display file permissions, owner, group, size, and modification time (similar to `ls -l`).
- **Inode Numbers**: Optionally display inode numbers for files and directories.
- **One Per Line**: Display one file or directory per line.
- **Cross-Platform**: Works on Unix-like systems (Linux, macOS, etc.).

---

## Usage

### Compilation

To compile the program, use the following command:

```bash
gcc -o ls ls.c
```
---
### Running the Program
Run the program with the following syntax:

```bash
./ls [OPTIONS] [DIRECTORY]
```
If no directory is specified, the program will list the contents of the current directory (.).
------------------------------
### Options
* `-a`: Show hidden files (files starting with `.`).

* `-l`: Display detailed information about each file (similar to` ls -l`).

* `-t`: Sort files by modification time (newest first).

* `-u`: Sort files by access time (newest first).

* `-c`: Sort files by change time (newest first).

* `-f`: Display only files (exclude directories).

* `-d`: Display only directories (exclude files).

* `-i`: Display inode numbers for files and directories.

* `-1`: Display one file or directory per line.
----------------------------------
### Examples

1- List files in the current directory:
```bash
./ls 
```
2- List files in a specific directory with detailed information:
```bash
./ls -l /path/to/directory
```
3- List only directories sorted by modification time:
```bash
./ls -d -t /path/to/directory
```
4- List files with inode numbers and one per line:
```bash
./ls -i -1 /path/to/directory
```

------------------------
### Output Format 

* **Files**: Displayed in **pink**.

* **Directories**: Displayed in **blue**.

* **Executable Files**: Displayed in **green**.

* **Detailed Information**: Includes permissions, owner, group, size, and modification time.
----

### Code Structure

* `do_ls`: The main function that lists directory contents based on the provided options.



* `print_file_info`: Prints detailed information about a file or directory.

* `print_permissions`: Prints file permissions in a human-readable format.

* **Sorting Functions**: `compare_mod_time`, `compare_acc_time`, and `compare_chg_time` for sorting files by time.

* **Helper Functions**: `is_directory`, `is_elf`, and `greatest_strlen` for utility purposes.
---

### Dependencies

* Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, `dirent.h`, `sys/stat.h`, `unistd.h`, `pwd.h`, `grp.h`, `time.h`, `errno.h`, `limits.h`).

