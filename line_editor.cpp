#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stack>
#include <limits>

using namespace std;

enum OperationType
{
    INSERT_LINE,
    DELETE_LINE,
    UPDATE_LINE,
    INSERT_WORD,
    DELETE_WORD
};

struct Operation
{
    OperationType type;
    int lineNum;
    string oldData;
    string newData;
    int wordPosition;

    Operation(OperationType t, int l, const string &oldD = "", const string &newD = "", int pos = -1)
        : type(t), lineNum(l), oldData(oldD), newData(newD), wordPosition(pos) {}
};

struct Cursor
{
    int lineNumber;
    int wordPosition;

    Cursor(int line = -1, int pos = -1) : lineNumber(line), wordPosition(pos) {}
};

struct WordNode
{
    string word;
    WordNode *next;
    WordNode(const string &worddata) : word(worddata), next(nullptr) {}
};

struct ListNode
{
    WordNode *wordHead; 
    ListNode *next;

    ListNode(const string &lineData) : wordHead(nullptr), next(nullptr)
    {
        istringstream iss(lineData);
        string word;
        WordNode **current = &wordHead;

        while (iss >> word)
        {
            *current = new WordNode(word);
            current = &((*current)->next);
        }
    }

    ~ListNode()
    {
        while (wordHead)
        {
            WordNode *temp = wordHead;
            wordHead = wordHead->next;
            delete temp;
        }
    }
};

class LineBuffer
{
public:
    ListNode *head;
    int lineCount;

public:
    LineBuffer() : head(nullptr), lineCount(0) {}

    ~LineBuffer()
    {
        clear();
    }

    void addLine(const string &line)
    {
        if (lineCount >= 25)
        {
            cout << "Buffer is full. Cannot add more lines." << endl;
            return;
        }

        ListNode *newNode = new ListNode(line);
        if (!head)
        {
            head = newNode;
        }
        else
        {
            ListNode *temp = head;
            while (temp->next)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        lineCount++;
    }

    void insertLine(int position, const string &line)
    {
        position--;
        if (position < 0 || position > lineCount)
        {
            cout << "Invalid position." << endl;
            return;
        }

        if (lineCount >= 25)
        {
            cout << "Buffer is full. Cannot insert more lines." << endl;
            return;
        }

        ListNode *newNode = new ListNode(line);
        if (position == 0)
        {
            newNode->next = head;
            head = newNode;
        }
        else
        {
            ListNode *temp = head;
            for (int i = 1; i < position; ++i)
            {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
        lineCount++;
    }

    void updateLine(int position, const string &line)
    {
        position--;
        if (position < 0 || position >= lineCount)
        {
            cout << "Invalid position." << endl;
            return;
        }

        ListNode *temp = head;
        for (int i = 0; i < position; ++i)
        {
            temp = temp->next;
        }

       
        WordNode *current = temp->wordHead;
        while (current)
        {
            WordNode *nextNode = current->next;
            delete current;
            current = nextNode;
        }

       
        istringstream iss(line);
        string word;
        WordNode **newCurrent = &temp->wordHead;

        while (iss >> word)
        {
            *newCurrent = new WordNode(word);
            newCurrent = &((*newCurrent)->next);
        }
    }

    void deleteLine(int position)
    {
        position--;
        if (position < 0 || position >= lineCount)
        {
            cout << "Invalid position." << endl;
            return;
        }

        ListNode *temp = head;
        if (position == 0)
        {
            head = head->next;
            delete temp;
        }
        else
        {
            for (int i = 1; i < position; ++i)
            {
                temp = temp->next;
            }
            ListNode *toDelete = temp->next;
            temp->next = toDelete->next;
            delete toDelete;
        }
        lineCount--;
    }

    void printLines() const
    {
        ListNode *temp = head;
        int lineNum = 1;
        while (temp)
        {
            string line = "";
            WordNode *wordTemp = temp->wordHead;
            while (wordTemp)
            {
                line += wordTemp->word;
                if (wordTemp->next)
                    line += " "; // Add a space between words
                wordTemp = wordTemp->next;
            }
            cout << "[Line No" << lineNum << "]: " << line << endl;
            temp = temp->next;
            lineNum++;
        }
    }

    string getLine(int position) const
    {
        if (position < 0 || position >= lineCount)
        {
            return "";
        }

        ListNode *temp = head;
        for (int i = 0; i < position; ++i)
        {
            temp = temp->next;
        }

        ostringstream oss;
        WordNode *wordTemp = temp->wordHead;
        while (wordTemp)
        {
            oss << wordTemp->word;
            if (wordTemp->next)
            {
                oss << " "; // Add a space between words
            }
            wordTemp = wordTemp->next;
        }

        return oss.str(); // Return the concatenated string
    }

    void clear()
    {
        ListNode *temp;
        while (head)
        {
            temp = head;
            head = head->next;
            delete temp;
        }
        lineCount = 0;
    }

    void writeBufferToFile(const string &fullPath) const
    {
        ofstream file(fullPath, ios::out | ios::trunc);

        if (!file.is_open())
        {
            cerr << "Error: Could not open the file for writing." << endl;
            return;
        }

        ListNode *temp = head;
        while (temp)
        {
            WordNode *wordTemp = temp->wordHead;
            while (wordTemp)
            {
                file << wordTemp->word;
                if (wordTemp->next)
                {
                    file << " ";
                }
                wordTemp = wordTemp->next;
            }
            file << endl;
            temp = temp->next;
        }

        file.close();
        cout << "Changes saved to '" << fullPath << "' successfully." << endl;
    }
};

bool fileExists(const string &filename)
{
    fstream file(filename);
    return file.is_open();
}

void createOrOpenFile(const string &fullPath)
{
    string mode = fileExists(fullPath) ? "r+" : "w+";
    ios_base::openmode openMode = ios::in | ios::out;
    if (mode == "w+")
    {
        openMode = ios::out | ios::trunc;
    }

    fstream file(fullPath, openMode);
    if (file.is_open())
    {
        cout << "File '" << fullPath << "' opened/created successfully." << endl;
        file.close();
    }
    else
    {
        cerr << "Error: Could not open or create file '" << fullPath << "'." << endl;
    }
}

bool readFileToBuffer(const string &fullPath, LineBuffer &buffer)
{
    ifstream file(fullPath);
    if (!file.is_open())
    {
        cerr << "Error: Could not open the file." << endl;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        buffer.addLine(line);
    }

    file.close();
    return true;
}

Cursor searchWord(LineBuffer &buffer, const string &word){
    ListNode *temp = buffer.head;
    int lineNum = 0;

    while (temp)
    {
        WordNode *wordTemp = temp->wordHead;
        int wordPos = 0;

        while (wordTemp)
        {
            if (wordTemp->word == word)
            {
                return Cursor(lineNum, wordPos);
            }
            wordTemp = wordTemp->next;
            wordPos++;
        }

        temp = temp->next;
        lineNum++;
    }

    return Cursor(-1, -1); // Word not found
}

void insertWordAtCursor(LineBuffer &buffer, const Cursor &cursor, const string &newWord)
{
    if (cursor.lineNumber == -1)
    {
        cout << "Invalid cursor position." << endl;
        return;
    }

    ListNode *temp = buffer.head;
    for (int i = 0; i < cursor.lineNumber; ++i)
    {
        temp = temp->next;
    }

    WordNode *wordTemp = temp->wordHead;
    WordNode *prev = nullptr;
    int pos = 0;

    while (wordTemp && pos < cursor.wordPosition)
    {
        prev = wordTemp;
        wordTemp = wordTemp->next;
        pos++;
    }

    // Insert new word node
    WordNode *newWordNode = new WordNode(newWord);
    if (prev)
    {
        prev->next = newWordNode;
        newWordNode->next = wordTemp;
    }
    else
    {
        newWordNode->next = temp->wordHead;
        temp->wordHead = newWordNode;
    }

    cout << "Word inserted successfully." << endl;
}

void replaceWordAtCursor(LineBuffer &buffer, const Cursor &cursor, const string &newWord, const string &oldWord)
{
    if (cursor.lineNumber == -1)
    {
        cout << "Invalid cursor position." << endl;
        return;
    }

    ListNode *temp = buffer.head;
    for (int i = 0; i < cursor.lineNumber; ++i)
    {
        temp = temp->next;
    }

    WordNode *wordTemp = temp->wordHead;
    int pos = 0;

    while (wordTemp && pos < cursor.wordPosition)
    {
        wordTemp = wordTemp->next;
        pos++;
    }

    if (wordTemp && wordTemp->word == oldWord)
    {
        wordTemp->word = newWord; // Replace the old word with the new word
        cout << "Word replaced successfully." << endl;
    }
    else
    {
        cout << "Word not found at the specified position." << endl;
    }
}

class UndoRedo
{
private:
    stack<Operation> undoStack;
    stack<Operation> redoStack;

public:
    void performUndo(LineBuffer &buffer)
    {
        if (undoStack.empty())
        {
            cout << "Nothing to undo." << endl;
            return;
        }

        Operation op = undoStack.top();
        undoStack.pop();
        applyInverseOperation(buffer, op);
        redoStack.push(op);
    }

    void performRedo(LineBuffer &buffer)
    {
        if (redoStack.empty())
        {
            cout << "Nothing to redo." << endl;
            return;
        }

        Operation op = redoStack.top();
        redoStack.pop();
        applyOperation(buffer, op);
        undoStack.push(op);
    }

    void recordOperation(const Operation &op)
    {
        if (undoStack.size() >= 3)
        {
            undoStack.pop();
        }
        undoStack.push(op);

        while (!redoStack.empty())
        {
            redoStack.pop();
        }
    }

    void applyOperation(LineBuffer &buffer, const Operation &op)
    {
        switch (op.type)
        {
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

    void applyInverseOperation(LineBuffer &buffer, const Operation &op)
    {
        switch (op.type)
        {
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

void handleOperations(LineBuffer &buffer, const string &fullPath, UndoRedo &undoRedo)
{
    int choice;
    while (true)
    {
        cout << "\nChoose an operation:" << endl;
        cout << "1. Insert Line" << endl;
        cout << "2. Update Line" << endl;
        cout << "3. Delete Line" << endl;
        cout << "4. Print Lines" << endl;
        cout << "5. Search Word" << endl;
        cout << "6. Undo" << endl;
        cout << "7. Redo" << endl;
        cout << "8. Save and Exit" << endl;
        cin >> choice;
        // if choice is not int
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }
        switch (choice)
        {
        case 1:
        {
            int position;
            string line;
            cout << "Enter position to insert: ";
            cin >> position;
            cin.ignore();
            cout << "Enter line: ";
            getline(cin, line);
            buffer.insertLine(position, line);
            undoRedo.recordOperation(Operation(INSERT_LINE, position, "", line));
            buffer.writeBufferToFile(fullPath);
            system("cls");
            cout << "Line inserted successfully." << endl;
            break;
        }
        case 2:
        {
            int position;
            string line;
            cout << "Enter position to update: ";
            cin >> position;
            cin.ignore();
            cout << "Enter new line: ";
            getline(cin, line);
            string oldLine = buffer.getLine(position);
            buffer.updateLine(position, line);
            undoRedo.recordOperation(Operation(UPDATE_LINE, position, oldLine, line));
            buffer.writeBufferToFile(fullPath);
            system("cls");
            cout << "Line updated successfully." << endl;
            break;
        }
        case 3:
        {
            int position;
            cout << "Enter position to delete: ";
            cin >> position;
            string oldLine = buffer.getLine(position);
            buffer.deleteLine(position);
            undoRedo.recordOperation(Operation(DELETE_LINE, position, oldLine));
            buffer.writeBufferToFile(fullPath);
            system("cls");
            cout << "Line deleted successfully." << endl;
            break;
        }
        case 4:
            buffer.printLines();
            break;
        case 5:
        {
            string word;
            cout << "Enter word to search: ";
            cin >> word;
            Cursor cursor = searchWord(buffer, word);
            system("cls");
            if (cursor.lineNumber != -1)
            {
                cout << "Word found at line " << cursor.lineNumber + 1 << ", position " << cursor.wordPosition + 1 << endl;

                // more operations like insert, delete, replace want to make here
                int choice;
                cout << "1. Insert word at cursor" << endl;
                cout << "2. Delete word at cursor" << endl;
                cout << "3. Replace word at cursor" << endl;
                cout << "4. Go back" << endl;
                cin >> choice;
                switch (choice)
                {
                case 1:
                {
                    string newWord;
                    cout << "Enter the word to insert: ";
                    cin >> newWord;
                    insertWordAtCursor(buffer, cursor, newWord);
                    undoRedo.recordOperation(Operation(INSERT_WORD, cursor.lineNumber, "", newWord, cursor.wordPosition));
                    buffer.writeBufferToFile(fullPath);
                    system("cls");
                    cout << "Word inserted successfully." << endl;
                    break;
                }
                case 2:
                {
                    string oldWord = buffer.getLine(cursor.lineNumber).substr(cursor.wordPosition, word.length());

                   
                    replaceWordAtCursor(buffer, cursor, "", oldWord);

                    
                    undoRedo.recordOperation(Operation(DELETE_WORD, cursor.lineNumber, oldWord, "", cursor.wordPosition));

                    buffer.writeBufferToFile(fullPath);
                    system("cls");
                    cout << "Word deleted successfully." << endl;
                    break;
                }

                case 3:
                {
                    string newWord;
                    cout << "Enter the new word: ";
                    cin >> newWord;

                    string oldWord = buffer.getLine(cursor.lineNumber).substr(cursor.wordPosition, word.length());
                    cout << "Old word: " << oldWord << endl;

                    replaceWordAtCursor(buffer, cursor, newWord, oldWord);

                    undoRedo.recordOperation(Operation(DELETE_WORD, cursor.lineNumber, oldWord, "", cursor.wordPosition));
                    undoRedo.recordOperation(Operation(INSERT_WORD, cursor.lineNumber, "", newWord, cursor.wordPosition));

                    buffer.writeBufferToFile(fullPath);
                    // system("cls");
                    cout << "Word replaced successfully." << endl;
                    break;
                }
                case 4:
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                }
            }
            else
            {
                cout << "Word not found." << endl;
            }
            break;
        }
        case 6:
            undoRedo.performUndo(buffer);
            buffer.writeBufferToFile(fullPath);
            system("cls");
            buffer.printLines();
            break;
        case 7:
            undoRedo.performRedo(buffer);
            buffer.writeBufferToFile(fullPath);
            system("cls");
            buffer.printLines();
            break;
        case 8:
            buffer.writeBufferToFile(fullPath);
            return;
        default:
            system("cls");
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    LineBuffer buffer;
    UndoRedo undoRedo;

    string filename = "file.txt";
    string directory = "";

    if (argc == 2)
    {
        filename = argv[1];
    }
    else if (argc == 3)
    {
        filename = argv[1];
        directory = argv[2];
    }
    // cout << "Filename: " << filename << endl;
    string fullPath = filename;
    if (!directory.empty())
    {
        fullPath = directory + "/" + filename;
    }

    // cout << "Full path: " << fullPath << endl;

    string mode = fileExists(fullPath) ? "r+" : "w+";
    // cout << "Opening file in mode: " << mode << endl;

    createOrOpenFile(fullPath);

    if (mode == "r+" && readFileToBuffer(fullPath, buffer))
    {
        cout << "File content loaded into the buffer:" << endl;
        buffer.printLines();
    }
    handleOperations(buffer, fullPath, undoRedo);

    return 0;
}
