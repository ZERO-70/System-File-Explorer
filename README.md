# File System Simulation in C++

This project is a C++ implementation of a basic hierarchical file system. It allows users to create, manage, and interact with files and directories using a simulated Command Line Interface (CLI).

## Features

- **Directory and File Management**:
  - Create directories (`mkdir`) and files (`touch`).
  - Navigate the directory structure (`cd`, `pwd`).
  - View directory contents (`ls`).

- **File Operations**:
  - Read file contents (`cat`).
  - Update file contents.
  - Delete files (`rm`) and directories (`rmdir`).

- **Advanced Operations**:
  - Copy and move files and directories (`cp`, `mv`).
  - Create symbolic links (`ln_s`).
  - Change file permissions and ownership (`chmod`, `chown`).

- **Search and View**:
  - Search files by name or content (`grep`, `search`).
  - View detailed file information (`stat`).
  - Display the directory structure as a tree.

- **Persistence**:
  - Save and load the file system state to/from a file.

- **Command Execution**:
  - Interactive CLI for executing various file system commands.

## Installation

1. Clone this repository:
   ```bash
   git clone <repository-url>
   ```
2. Compile the project using a C++ compiler (e.g., g++).
   ```bash
   g++ -o filesystem BSCS23034_FS_Assingment_Phase1.cpp
   ```
3. Run the program:
   ```bash
   ./filesystem
   ```

## Usage

- Start the CLI and use commands like:
  - `mkdir /home/user` to create a directory.
  - `touch /home/user/file.txt "File content"` to create a file with content.
  - `ls /home` to list contents of a directory.
  - `cat /home/user/file.txt` to view a file's content.
  - `mv /home/user/file.txt /docs` to move a file.
  - `cp /docs /backup` to copy a directory.

- Type `help` in the CLI for available commands and their syntax.

## File Structure

- **System Class**: Core logic for managing nodes (directories and files).
- **Node Structure**: Represents files and directories with metadata like size, permissions, owner, etc.
- **CLI Functionality**: Provides an interactive interface for executing file system commands.

## Future Improvements

- Add support for concurrent access using multithreading.
- Implement advanced file attributes (e.g., hidden, read-only).
- Enhance search functionality with regex support.
- Improve error handling and reporting.

## License

This project is open-source and available under the [MIT License](LICENSE).

## Author

[Muhammad Zair]
