#include <iostream>
#include <fstream>
#include <string>
using namespace std;
bool fileExists(const string& filename) {
    fstream file(filename);
    return file.is_open();
}

int main(int argc, char const *argv[])
{
    string mode=fileExists("yash.txt")?"r+":"w+";
    cout << "Opening file in mode: " << mode << endl;
    return 0;
}
