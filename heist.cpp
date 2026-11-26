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

// ================= Character hierarchy =================
Character::Character(const string& n, const string& r, const string& nat)
    : characterName(n), characterRole(r), nationality(nat), isCulprit(false), interviewed(false),
      clueRowChar(WORD_2), letterClueChar(' '), boardPosChar(0) {}

Character::~Character() {}

void Character::display() const { cout << characterName << "\n"; }

int Character::requiredCredibility() const { return 0; }
bool Character::isSuspect() const { return false; }
string Character::getName() const { return characterName; }
void Character::setClue(char letter, int pos) { letterClueChar = letter; boardPosChar = pos; }

Clue Character::getClue() const {
    Clue c;
    c.clueLetter = letterClueChar;
    c.cluePosition = boardPosChar;
    c.clueRow = clueRowChar;
    return c;
}

string Character::getRole() const { return characterRole; }
string Character::getNationality() const { return nationality; }
bool Character::isGuilty() const { return isCulprit; }
void Character::setGuilty(bool value) { isCulprit = value; }
bool Character::isInterviewed() const { return interviewed; }
void Character::setInterviewed(bool value) { interviewed = value; }
void Character::setBoardPos(int pos) { boardPosChar = pos; }
void Character::setLetter(char letter) { letterClueChar = letter; }
string Character::describe() const { return characterName + " (" + characterRole + ", " + nationality + ")"; }

Victim::Victim(const string& n, const string& r, const string& nat) : Character(n, r, nat) {}
void Victim::display() const { cout << "Victim: " << describe() << "\n"; }
int Victim::requiredCredibility() const { return 80; }

Suspect::Suspect(const string& n, const string& r, const string& nat) : Character(n, r, nat) {}
void Suspect::display() const { cout << "Suspect: " << describe() << "\n"; }
bool Suspect::isSuspect() const { return true; }

BusinessRelations::BusinessRelations(const string& n, const string& r, const string& nat) : Suspect(n, r, nat) {}
void BusinessRelations::display() const { cout << "Business relation: " << describe() << "\n"; }
int BusinessRelations::requiredCredibility() const { return 50; }

CloseRelations::CloseRelations(const string& n, const string& r, const string& nat) : Suspect(n, r, nat) {}
void CloseRelations::display() const { cout << "Close relation: " << describe() << "\n"; }
int CloseRelations::requiredCredibility() const { return 75; }

HouseHelp::HouseHelp(const string& n, const string& r, const string& nat) : Suspect(n, r, nat) {}
void HouseHelp::display() const { cout << "House help: " << describe() << "\n"; }

// The virtual base class Suspect is initialised by the most derived class.
Partner::Partner(const string& n, const string& r, const string& nat) : Suspect(n, r, nat), BusinessRelations(n, r, nat) {}
Child::Child(const string& n, const string& r, const string& nat) : Suspect(n, r, nat), CloseRelations(n, r, nat) {}

PA::PA(const string& n, const string& r, const string& nat)
    : Suspect(n, r, nat), BusinessRelations(n, r, nat), CloseRelations(n, r, nat) {}
void PA::display() const { cout << "Personal Assistant: " << describe() << "\n"; }
int PA::requiredCredibility() const { return 35; }

Cook::Cook(const string& n, const string& r, const string& nat) : HouseHelp(n, r, nat) {}
Guard::Guard(const string& n, const string& r, const string& nat) : HouseHelp(n, r, nat) {}
Servant::Servant(const string& n, const string& r, const string& nat) : HouseHelp(n, r, nat) {}

Detective::Detective(const string& n, int cred)
    : Character(n, "Detective", "Unknown"), credibility(cred), turns(0)
{}

void Detective::display() const { cout << "Detective " << characterName << " | Credibility: " << credibility << " | Turns used: " << turns << "\n"; }
int Detective::getCredibility() const { return credibility; }
void Detective::loseCredibility(int amount) { credibility = credibility - amount; if (credibility < 0) credibility = 0; }
int Detective::getTurns() const { return turns; }
void Detective::incrementTurns() { turns++; }

// ================= Cottage =================
Cottage::Cottage() {
    locations = new Location[7];
    locationCount = 0;
}

Cottage::Cottage(const Cottage& other) { // deep copy: new array, copy every location
    locations = new Location[7];
    locationCount = other.locationCount;
    for (int i = 0; i < locationCount; i++) locations[i] = other.locations[i];
}

Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) {
        locationCount = other.locationCount;
        for (int i = 0; i < locationCount; i++) locations[i] = other.locations[i];
    }
    return *this;
}

Cottage::~Cottage() { delete[] locations; }

void Cottage::addLocation(const Location& l) {
    if (locationCount < 7) { locations[locationCount] = l; locationCount++; }
}

void Cottage::clearLocations() { locationCount = 0; }
int Cottage::getLocationCount() const { return locationCount; }
Location& Cottage::at(int i) { return locations[i]; }

Clue Cottage::getClue(int i) const {
    Clue c;
    c.clueLetter = locations[i].letterClueLoc;
    c.cluePosition = locations[i].boardPosLoc;
    c.clueRow = locations[i].locRow;
    return c;
}

void Cottage::displayLocations() const {
    cout << "\n===== Location Menu =====\n";
    for (int i = 0; i < locationCount; i++) {
        cout << "[" << i + 1 << "] " << locations[i].locName;
        if (locations[i].locSearchFlag) cout << " - Searched\n";
        else cout << " - NOT Searched\n";
    }
}


// ================= TileRack =================
TileRack::TileRack(int cap) {
    capacity = cap;
    count = 0;
    letters = new Clue[capacity];
}

TileRack::TileRack(const TileRack& o) {         // deep copy
    capacity = o.capacity;
    count = o.count;
    letters = new Clue[capacity];
    for (int i = 0; i < count; i++) letters[i] = o.letters[i];
}

TileRack& TileRack::operator=(const TileRack& o) {
    if (this != &o) {
        delete[] letters;
        capacity = o.capacity;
        count = o.count;
        letters = new Clue[capacity];
        for (int i = 0; i < count; i++) letters[i] = o.letters[i];
    }
    return *this;
}

TileRack::~TileRack() { delete[] letters; }

int TileRack::getCount() const { return count; }

void TileRack::addClue(const Clue& c) {
    if (getCount() >= capacity)
        { cout << "\n[!] " << "The rack is full." << "\n"; return; }
    letters[count] = c;
    count++;
}

Clue TileRack::removeLast() {
    if (getCount() == 0)
        { cout << "\n[!] " << "The rack is empty." << "\n"; return Clue(); }
    Clue last = letters[getCount() - 1];
    count--;
    return last;
}

void TileRack::display() const {
    if (getCount() == 0) { cout << "(empty)"; return; }
    for (int i = 0; i < getCount(); i++) {
        if (letters[i].clueLetter == ' ') cout << "  ";                  // separator between merged racks
        else cout << "[" << letters[i].clueLetter << "@" << letters[i].cluePosition << "] ";
    }
}

TileRack TileRack::merge(const TileRack& o) const {                      // merge both racks as one clue
    TileRack merged(capacity + o.capacity + 1);
    for (int i = 0; i < getCount(); i++) merged.addClue(letters[i]);
    Clue space;
    space.clueLetter = ' ';
    space.cluePosition = 0;
    space.clueRow = WORD_1;
    merged.addClue(space);
    for (int i = 0; i < o.getCount(); i++) merged.addClue(o.letters[i]);
    return merged;
}
// ================= Scrabble =================
Scrabble::Scrabble() {
    row1 = new Tile[7];
    row2 = new Tile[8];
    clearBoard();
}

Scrabble::Scrabble(const Scrabble& o) { // deep copy: new rows, copy every tile
    row1 = new Tile[7];
    row2 = new Tile[8];
    target1 = o.target1;
    target2 = o.target2;
    for (int i = 0; i < 7; i++) row1[i] = o.row1[i];
    for (int i = 0; i < 8; i++) row2[i] = o.row2[i];
}

Scrabble& Scrabble::operator=(const Scrabble& o) {
    if (this != &o) {
        target1 = o.target1;
        target2 = o.target2;
        for (int i = 0; i < 7; i++) row1[i] = o.row1[i];
        for (int i = 0; i < 8; i++) row2[i] = o.row2[i];
    }
    return *this;
}

Scrabble::~Scrabble() {
    delete[] row1;
    delete[] row2;
}

void Scrabble::clearBoard() { // every slot becomes empty
    for (int i = 0; i < 7; i++) { row1[i].tileLetter = ' '; row1[i].filled = false; }
    for (int i = 0; i < 8; i++) { row2[i].tileLetter = ' '; row2[i].filled = false; }
}

void Scrabble::setTargets(const string& w1, const string& w2) {
    if (w1.size() != 7 || w2.size() != 8)
        { cout << "\n[!] " << "clue.txt is corrupted: Word_1 needs 7 letters and Word_2 needs 8." << "\n"; return; }
    target1 = toUpperString(w1);
    target2 = toUpperString(w2);
    clearBoard(); // fresh empty board
}

void Scrabble::placeClue(const Clue& c) {
    Tile* row = row2;
    int size = 8;
    if (c.clueRow == WORD_1) { row = row1; size = 7; }
    const string& target = (c.clueRow == WORD_1) ? target1 : target2;

    if (c.cluePosition < 1 || c.cluePosition > size)
        { cout << "\n[!] " << "That position does not exist on the board." << "\n"; return; }
    Tile& slot = row[c.cluePosition - 1];
    if (slot.filled)
        { cout << "\n[!] " << "That position is already filled." << "\n"; return; }

    Clue expected; // the letter this slot should contain
    expected.clueLetter = target[c.cluePosition - 1];
    expected.cluePosition = c.cluePosition;
    expected.clueRow = c.clueRow;
    if (!sameClue(c, expected))
        { cout << "\n[!] " << "That letter does not belong at that position." << "\n"; return; }

    slot.tileLetter = toUpperChar(c.clueLetter);
    slot.filled = true;
}

bool Scrabble::isRowComplete(WordRow r) const {
    const Tile* row = (r == WORD_1) ? row1 : row2;
    int size = (r == WORD_1) ? 7 : 8;
    for (int i = 0; i < size; i++) if (!row[i].filled) return false;
    return true;
}

bool Scrabble::checkWord(WordRow r, const string& w) const {
    if (!isRowComplete(r)) return false;
    if (r == WORD_1) return toUpperString(w) == target1;
    return toUpperString(w) == target2;
}

void Scrabble::display() const {
    cout << "Scrabble:\n Word 1: ";
    for (int i = 0; i < 7; i++) {
        if (row1[i].filled) cout << "[" << row1[i].tileLetter << "]";
        else cout << "[ ]";
    }
    if (isRowComplete(WORD_1)) cout << " -> COMPLETE";
    cout << "\n Word 2: ";
    for (int i = 0; i < 8; i++) {
        if (row2[i].filled) cout << "[" << row2[i].tileLetter << "]";
        else cout << "[ ]";
    }
    if (isRowComplete(WORD_2)) cout << " -> COMPLETE";
    cout << "\n";
}