// heist.cpp - THE LOCKER HEIST: implementation of all classes
#include "heist.h"
#include <cstdlib>
#include <ctime>

// ================= Helper functions =================
char toUpperChar(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
    return c;
}

string toUpperString(string s) {
    for (int i = 0; i < (int)s.size(); i++) s[i] = toUpperChar(s[i]);
    return s;
}

string intToText(int n) {                       // number -> text
    if (n == 0) return "0";
    string s = "";
    bool negative = (n < 0);
    if (negative) n = -n;
    while (n > 0) { s = char('0' + n % 10) + s; n = n / 10; }
    if (negative) s = "-" + s;
    return s;
}

int textToInt(const string& s) {                // text -> number (used for file values)
    int value = 0;
    bool negative = false;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '-') negative = true;
        else if (s[i] >= '0' && s[i] <= '9') value = value * 10 + (s[i] - '0');
    }
    if (negative) value = -value;
    return value;
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) {
        cin.clear();
        return "";
    }
    return s;
}

int readInt(const string& prompt, int low, int high) {
    string s = readLine(prompt);
    if (s.size() == 0 || s.size() > 6)
    {
        cout << "\n[!] " << "Please enter a valid number." << "\n"; return -1;
    }
    int value = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] < '0' || s[i] > '9')
        {
            cout << "\n[!] " << "Please enter a number, not text." << "\n"; return -1;
        }
        value = value * 10 + (s[i] - '0');
    }
    if (value < low || value > high)
    {
        cout << "\n[!] " << "Choice is out of range (" + intToText(low) + " to " + intToText(high) + ")." << "\n"; return -1;
    }
    return value;
}

bool readYesNo(const string& prompt) {
    string s = toUpperString(readLine(prompt));
    if (s == "Y") return true;
    if (s == "N") return false;
    { cout << "\n[!] " << "Please answer with Y or N." << "\n"; return false; }
}

bool openFile(ifstream& in, const string& name, bool skipHeader) {
    in.open(name.c_str());
    if (!in)
    {
        cout << "\n[!] " << name + " not found." << "\n"; return false;
    }
    if (skipHeader) { string header; getline(in, header); }
    return true;
}

// Reads the next non-empty line of a '|' separated file into fields. Returns false at end of file.
bool readRow(ifstream& in, string fields[], int minColumns, const string& fileName) {
    string line;
    while (getline(in, line)) {
        if (line.size() > 0 && line[line.size() - 1] == '\r') line.erase(line.size() - 1, 1);
        if (line.size() == 0) continue;
        int count = 0;
        string current = "";
        for (int i = 0; i < (int)line.size(); i++) {
            if (line[i] == '|') {
                if (count < MAX_FIELDS) { fields[count] = current; count++; }
                current = "";
            }
            else current += line[i];
        }
        if (count < MAX_FIELDS) { fields[count] = current; count++; }
        if (count < minColumns)
        {
            cout << "\n[!] " << fileName + " is corrupted (too few columns)." << "\n"; return false;
        }
        return true;
    }
    return false;
}

string fixRole(string role) {                   // "Partner 1" -> "Partner_1"
    for (int i = 0; i < (int)role.size(); i++) if (role[i] == ' ') role[i] = '_';
    return role;
}

int roleColumn(const string& role) {            // which profile column belongs to this role
    const char* roles[7] = { "Cook", "Guard", "Servant", "Partner_1", "Partner_2", "PA", "Child" };
    for (int i = 0; i < 7; i++) if (role == roles[i]) return i;
    return -1;
}

bool sameClue(const Clue& a, const Clue& b) {
    return toUpperChar(a.clueLetter) == toUpperChar(b.clueLetter) && a.cluePosition == b.cluePosition && a.clueRow == b.clueRow;
}