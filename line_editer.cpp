#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <cstdlib>

using namespace std;

// Define the structure for each node in the linked list
struct ListNode {
    string line;
    ListNode* next;

    ListNode(const string& lineData) : line(lineData), next(nullptr) {}
};

// Linked list class to manage lines
class LineBuffer {
private:
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

void handleOperations(LineBuffer& buffer, const string& fullPath) {
    int choice;
    while (true) {
        cout << "\nChoose an operation:" << endl;
        cout << "1. Insert Line" << endl;
        cout << "2. Update Line" << endl;
        cout << "3. Delete Line" << endl;
        cout << "4. Print Lines" << endl;
        cout << "5. Save and Exit" << endl;
        cin >> choice;

        switch (choice) {
            case 1: {
                int position;
                string line;
                cout << "Enter position to insert: ";
                cin >> position;
                cin.ignore();  // Ignore newline character from previous input
                cout << "Enter line: ";
                getline(cin, line);
                buffer.insertLine(position, line);
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
                buffer.updateLine(position, line);
                break;
            }
            case 3: {
                int position;
                cout << "Enter position to delete: ";
                cin >> position;
                buffer.deleteLine(position);
                break;
            }
            case 4:
                buffer.printLines();
                break;
            case 5:
                buffer.writeBufferToFile(fullPath);
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    LineBuffer buffer;

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

    handleOperations(buffer, fullPath);

    return 0;
}
