#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <cstdlib>
#include <stack>

using namespace std;

enum OperationType { INSERT_LINE, DELETE_LINE, UPDATE_LINE, INSERT_WORD, DELETE_WORD };

struct Operation {
    OperationType type;
    int lineNum;
    string oldData;
    string newData;
    int wordPosition;

    Operation(OperationType t, int l, const string& oldD = "", const string& newD = "", int pos = -1)
        : type(t), lineNum(l), oldData(oldD), newData(newD), wordPosition(pos) {}
};

struct Cursor {
    int lineNumber;
    int wordPosition;

    Cursor(int line = -1, int pos = -1) : lineNumber(line), wordPosition(pos) {}
};

// Define the structure for each node in the linked list
struct ListNode {
    string line;
    ListNode* next;
    ListNode(const string& lineData) : line(lineData), next(nullptr) {}
};

// Linked list class to manage lines
class LineBuffer {
public:
    ListNode* head;
    int lineCount;

public:
    LineBuffer() : head(nullptr), lineCount(0) {}

    ~LineBuffer() {
        clear();
    }

    void addLine(const string& line) {
        if (lineCount >= 25) {
            cout << "Buffer is full. Cannot add more lines." << endl;
            return;
        }

        ListNode* newNode = new ListNode(line);
        if (!head) {
            head = newNode;
        } else {
            ListNode* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        lineCount++;
    }

    void insertLine(int position, const string& line) {
        if (position < 0 || position > lineCount) {
            cout << "Invalid position." << endl;
            return;
        }

        if (lineCount >= 25) {
            cout << "Buffer is full. Cannot insert more lines." << endl;
            return;
        }

        ListNode* newNode = new ListNode(line);
        if (position == 0) {
            newNode->next = head;
            head = newNode;
        } else {
            ListNode* temp = head;
            for (int i = 1; i < position; ++i) {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
        lineCount++;
    }

    void updateLine(int position, const string& line) {
        if (position < 0 || position >= lineCount) {
            cout << "Invalid position." << endl;
            return;
        }

        ListNode* temp = head;
        for (int i = 0; i < position; ++i) {
            temp = temp->next;
        }
        temp->line = line;
    }

    void deleteLine(int position) {
        if (position < 0 || position >= lineCount) {
            cout << "Invalid position." << endl;
            return;
        }

        ListNode* temp = head;
        if (position == 0) {
            head = head->next;
            delete temp;
        } else {
            for (int i = 1; i < position; ++i) {
                temp = temp->next;
            }
            ListNode* toDelete = temp->next;
            temp->next = toDelete->next;
            delete toDelete;
        }
        lineCount--;
    }

    void printLines() const {
        ListNode* temp = head;
        int lineNum = 1;
        while (temp) {
            cout << "[Line No" << lineNum << "]: " << temp->line << endl;
            temp = temp->next;
            lineNum++;
        }
    }

    string getLine(int position) const {
        if (position < 0 || position >= lineCount) {
            return "";
        }
        ListNode* temp = head;
        for (int i = 0; i < position; ++i) {
            temp = temp->next;
        }
        return temp->line;
    }

    void clear() {
        ListNode* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
        lineCount = 0;
    }

    void writeBufferToFile(const string& fullPath) const {
        ofstream file(fullPath, ios::out | ios::trunc);
        if (!file.is_open()) {
            cerr << "Error: Could not open the file for writing." << endl;
            return;
        }

        ListNode* temp = head;
        while (temp) {
            file << temp->line << endl;
            temp = temp->next;
        }

        file.close();
        cout << "Changes saved to '" << fullPath << "' successfully." << endl;
    }
};

// Function to check if a file exists
bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Function to create or open a file
void createOrOpenFile(const string& fullPath, const string& mode) {
    ios_base::openmode openMode = ios::in | ios::out;
    if (mode == "w+") {
        openMode = ios::out | ios::trunc;
    }

    fstream file(fullPath, openMode);
    if (file.is_open()) {
        cout << "File '" << fullPath << "' opened/created successfully." << endl;
        file.close();
    } else {
        cerr << "Error: Could not open or create file '" << fullPath << "'." << endl;
    }
}

// Function to read the file line by line and store it in the buffer
bool readFileToBuffer(const string& fullPath, LineBuffer& buffer) {
    ifstream file(fullPath);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file." << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        buffer.addLine(line);
    }

    file.close();
    return true;
}

Cursor searchWord(LineBuffer& buffer, const string& word) {
    ListNode* temp = buffer.head;
    int lineNum = 0;
    
    while (temp) {
        size_t pos = temp->line.find(word);
        if (pos != string::npos) {
            return Cursor(lineNum, pos);
        }
        temp = temp->next;
        lineNum++;
    }
    return Cursor(-1, -1);  // Word not found
}

void insertWordAtCursor(LineBuffer& buffer, const Cursor& cursor, const string& newWord) {
    if (cursor.lineNumber == -1) {
        cout << "Invalid cursor position." << endl;
        return;
    }

    ListNode* temp = buffer.head;
    for (int i = 0; i < cursor.lineNumber; ++i) {
        temp = temp->next;
    }

    temp->line.insert(cursor.wordPosition, newWord + " ");
}

void replaceWordAtCursor(LineBuffer& buffer, const Cursor& cursor, const string& newWord, const string& oldWord) {
    if (cursor.lineNumber == -1) {
        cout << "Invalid cursor position." << endl;
        return;
    }

    ListNode* temp = buffer.head;
    for (int i = 0; i < cursor.lineNumber; ++i) {
        temp = temp->next;
    }

    size_t pos = temp->line.find(oldWord, cursor.wordPosition);
    if (pos != string::npos) {
        temp->line.replace(pos, oldWord.length(), newWord);
    }
}

class UndoRedo {
private:
    stack<Operation> undoStack;
    stack<Operation> redoStack;

public:
    void performUndo(LineBuffer& buffer) {
        if (undoStack.empty()) {
            cout << "Nothing to undo." << endl;
            return;
        }

        Operation op = undoStack.top();
        undoStack.pop();
        applyInverseOperation(buffer, op);
        redoStack.push(op);
    }

    void performRedo(LineBuffer& buffer) {
        if (redoStack.empty()) {
            cout << "Nothing to redo." << endl;
            return;
        }

        Operation op = redoStack.top();
        redoStack.pop();
        applyOperation(buffer, op);
        undoStack.push(op);
    }

    void recordOperation(const Operation& op) {
        if (undoStack.size() >= 3) {
            undoStack.pop();  // Maintain a limit of 3 operations
        }
        undoStack.push(op);
        // Clear redo stack on new operation
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

    void applyOperation(LineBuffer& buffer, const Operation& op) {
        switch (op.type) {
            case INSERT_LINE:
                buffer.insertLine(op.lineNum, op.newData);
                break;
            case DELETE_LINE:
                buffer.deleteLine(op.lineNum);
                break;
            case UPDATE_LINE:
                buffer.updateLine(op.lineNum, op.newData);
                break;
            case INSERT_WORD:
                insertWordAtCursor(buffer, Cursor(op.lineNum, op.wordPosition), op.newData);
                break;
            case DELETE_WORD:
                replaceWordAtCursor(buffer, Cursor(op.lineNum, op.wordPosition), "", op.oldData);
                break;
        }
    }

    void applyInverseOperation(LineBuffer& buffer, const Operation& op) {
        switch (op.type) {
            case INSERT_LINE:
                buffer.deleteLine(op.lineNum);
                break;
            case DELETE_LINE:
                buffer.insertLine(op.lineNum, op.oldData);
                break;
            case UPDATE_LINE:
                buffer.updateLine(op.lineNum, op.oldData);
                break;
            case INSERT_WORD:
                replaceWordAtCursor(buffer, Cursor(op.lineNum, op.wordPosition), "", op.newData);
                break;
            case DELETE_WORD:
                insertWordAtCursor(buffer, Cursor(op.lineNum, op.wordPosition), op.oldData);
                break;
        }
    }
};

void handleOperations(LineBuffer& buffer, const string& fullPath, UndoRedo& undoRedo) {
    int choice;
    while (true) {
        cout << "\nChoose an operation:" << endl;
        cout << "1. Insert Line" << endl;
        cout << "2. Update Line" << endl;
        cout << "3. Delete Line" << endl;
        cout << "4. Print Lines" << endl;
        cout << "5. Search Word" << endl;
        cout << "6. Insert Word at Cursor" << endl;  // New option for inserting word
        cout << "7. Delete Word at Cursor" << endl;  // New option for deleting word
        cout << "8. Undo" << endl;
        cout << "9. Redo" << endl;
        cout << "10. Save and Exit" << endl;
        cin >> choice;

        switch (choice) {
            case 1: {
                int position;
                string line;
                cout << "Enter position to insert: ";
                cin >> position;
                cin.ignore();
                cout << "Enter line: ";
                getline(cin, line);
                buffer.insertLine(position, line);
                undoRedo.recordOperation(Operation(INSERT_LINE, position, "", line));
                break;
            }
            case 2: {
                int position;
                string line;
                cout << "Enter position to update: ";
                cin >> position;
                cin.ignore();
                cout << "Enter new line: ";
                getline(cin, line);
                string oldLine = buffer.getLine(position);  // Assume getLine() fetches the line at position
                buffer.updateLine(position, line);
                undoRedo.recordOperation(Operation(UPDATE_LINE, position, oldLine, line));
                break;
            }
            case 3: {
                int position;
                cout << "Enter position to delete: ";
                cin >> position;
                string oldLine = buffer.getLine(position);  // Assume getLine() fetches the line at position
                buffer.deleteLine(position);
                undoRedo.recordOperation(Operation(DELETE_LINE, position, oldLine));
                break;
            }
            case 4:
                buffer.printLines();
                break;
            case 5: {
                string word;
                cout << "Enter word to search: ";
                cin >> word;
                Cursor cursor = searchWord(buffer, word);
                if (cursor.lineNumber != -1) {
                    cout << "Word found at line " << cursor.lineNumber + 1 << ", position " << cursor.wordPosition << endl;
                } else {
                    cout << "Word not found." << endl;
                }
                break;
            }
            case 6: {  // Insert word at cursor
                string word;
                cout << "Enter the word to insert: ";
                cin >> word;
                int lineNum, wordPos;
                cout << "Enter line number and word position: ";
                cin >> lineNum >> wordPos;
                Cursor cursor(lineNum - 1, wordPos);  // Adjusting line number for 0-based index
                insertWordAtCursor(buffer, cursor, word);
                undoRedo.recordOperation(Operation(INSERT_WORD, lineNum - 1, "", word, wordPos));  // Record operation
                break;
            }
            case 7: {  // Delete word at cursor
                string word;
                cout << "Enter the word to delete: ";
                cin >> word;
                int lineNum, wordPos;
                cout << "Enter line number and word position: ";
                cin >> lineNum >> wordPos;
                Cursor cursor(lineNum - 1, wordPos);  // Adjusting line number for 0-based index
                replaceWordAtCursor(buffer, cursor, "", word);
                undoRedo.recordOperation(Operation(DELETE_WORD, lineNum - 1, word, "", wordPos));  // Record operation
                break;
            }
            case 8:
                undoRedo.performUndo(buffer);
                break;
            case 9:
                undoRedo.performRedo(buffer);
                break;
            case 10:
                buffer.writeBufferToFile(fullPath);
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    LineBuffer buffer;
    UndoRedo undoRedo;

    string filename = "file.txt";  // Default filename
    string directory = "";
    string fullPath = filename;

    if (argc == 2) {
        filename = argv[1];
    } else if (argc == 3) {
        filename = argv[1];
        directory = argv[2];
    }

    if (!directory.empty()) {
        fullPath = directory + "/" + filename;
    }

    string mode = fileExists(fullPath) ? "r+" : "w+";

    createOrOpenFile(fullPath, mode);

    if (mode == "r+" && readFileToBuffer(fullPath, buffer)) {
        cout << "File content loaded into the buffer:" << endl;
        buffer.printLines();
    }

    handleOperations(buffer, fullPath, undoRedo);

    return 0;
}
