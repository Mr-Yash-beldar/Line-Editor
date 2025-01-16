
# Line Editor in C++

## Project Description
This project is a line editor developed in C that allows you to open, read, modify, and save text files. The program supports CRUD (Create, Read, Update, Delete) operations on lines of text and individual words, along with search and navigation features. The editor uses a linked list data structure to store both lines and words, making it efficient for operations like insertion and deletion.

## Features
- **File Handling**: Open, read, and write to files in different modes (`r+` or `w+`).
- **Buffer Storage**: Stores lines in a linked list for ease of insertion and deletion.
- **Search Functionality**: Search for a word in the file and get the cursor position (line number and word position).
- **CRUD Operations**:
  - Insert a new line at the cursor position.
  - Insert a word at the cursor position in a specific line.
  - Update a word with another word.
  - Delete a line or a word in a line.
- **Undo/Redo**: Supports undo and redo functionality for the last three operations.

## Requirements
- A C compiler (e.g., GCC)
- Standard C libraries (stdio.h, stdlib.h, string.h)

## Setup Instructions

1. **Clone the Repository**
   If you are using Git, clone the repository:

   ```bash
   git clone https://github.com/Mr-Yash-beldar/Line-Editor.git
   cd Line-Editor
   ```

2. **Compile the Program**
   Use the following command to compile the program:

   ```bash
   gcc -o lineeditor line_editor.cpp
   ```

3. **Run the Program**

   You can run the program from the command line using different arguments to specify the file you want to edit.

   ### Command Line Syntax:
   **without Argument** (Creates or Read file.txt by default):
   ```bash
   ./line_editor
   ```

   ### Example Usage:

   - **One Argument** (Create a new file in the current directory or read it if present already):
     ```bash
     ./line_editor testfile.txt
     ```

     This will create a new file called `testfile.txt` in the current directory or open it if it already exists.

   - **Two Arguments** (Open an existing file or create a new file in the current directory):
     ```bash
     ./line_editor yash.txt
     ```

   - **Three Arguments** (Open an existing file or create a new file in a specific directory):
     ```bash
     ./line_editor yashodip.txt foldername
     ```

   - **More than 3 Arguments** (Error handling):
     ```bash
     Error: Too many arguments. Usage: ./line_editor <filename> [directory] 
     ```

4. **Command Line Operations**:
   After opening a file, you can interact with the file through different commands:
   
   - **Search**: Search for a word in the file and return the line number and word position.
   
   - **Insert Line**: Insert a new line at the cursor position.
   
   - **Insert Word**: Insert a new word at the cursor position within a line.
   
   - **Update Word**: Replace an existing word with another word.
   
   - **Delete Line**: Delete a line at the cursor position.
   
   - **Delete Word**: Delete a word at the cursor position within a line.
   
   - **Undo**: Undo the last 3 operations.
   
   - **Redo**: Redo the last 3 undone operations.

## File Structure

```bash
line_editor.c    # Main program file with logic
README.md        # Project documentation
```

## Example Commands to Test

- **Opening a file for the first time**:
  ```bash
  ./lineeditor myfile.txt
  ```

  This will open `myfile.txt` or create it if it doesn't exist.

- **Inserting a line at a specific position**:
  1. After the file is opened, the cursor is at the first line.
  2. Insert a new line:
     ```bash
     Insert a new line at line 1: "This is a new line."
     ```

- **Searching for a word**:
  1. Search for the word "line" in the file:
     ```bash
     Search for the word 'line'
     ```

     This will return the line number and the position of the word in that line.

- **Update a word**:
  1. Update the word "line" to "sentence":
     ```bash
     Update 'line' to 'sentence' in line 1
     ```

- **Undo the last operation**:
  ```bash
  Undo the last operation
  ```

- **Redo the last undone operation**:
  ```bash
  Redo the last undone operation
  ```

## Troubleshooting

- If you get an error about too many arguments:
  - Ensure you are using a maximum of 3 arguments (filename and optional directory).
  
- If the file does not open or cannot be read:
  - Check if the file exists and is accessible with the given permissions.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
