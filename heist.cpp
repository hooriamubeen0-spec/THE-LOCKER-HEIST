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

// ===========leaderboard ================
LeaderBoard::LeaderBoard(const string& path) : filePath(path) {}

void LeaderBoard::addScore(const string& name, const string& result, int cred, int turnsLeft, const string& culprit) const {
    ifstream check(filePath.c_str());
    if (!check)
        { cout << "\n[!] " << filePath + " not found (the 'leader' folder and file must exist)." << "\n"; return; }
    check.close();

    ofstream out(filePath.c_str(), ios::app);                            // append at the end of the file
    if (!out)
        { cout << "\n[!] " << "Cannot write to " + filePath + "." << "\n"; return; }
    time_t now = time(0);
    tm* t = localtime(&now);
    int day = t->tm_mday, month = t->tm_mon + 1, year = (t->tm_year + 1900) % 100;
    out << name << "|" << result << "|" << cred << "|" << (turnsLeft < 10 ? "0" : "") << turnsLeft << "|" << culprit << "|"
        << (day < 10 ? "0" : "") << day << "-" << (month < 10 ? "0" : "") << month << "-" << (year < 10 ? "0" : "") << year << "\n";
}

void LeaderBoard::display() const {
    ifstream in;
    if (!(openFile(in, filePath, true))) return;
    cout << "\n=====LEADER/SCOREBOARD======\n";
    cout << "Detective | Result | Credibility | Turns_Left | Culprit | Date\n";
    string f[MAX_FIELDS];
    while (readRow(in, f, 6, filePath))
        cout << f[0] << " | " << f[1] << " | " << f[2] << " | " << f[3] << " | " << f[4] << " | " << f[5] << "\n";
}

// ================= GameEngine =================
GameEngine::GameEngine()
    : state(STATE_MENU), maxTurns(25), startingCredibility(100), wrongTurnPenalty(5), wrongWordPenalty(10),
    winCondition("Accuse_Correct_Culprit"), loseCondition("Credibility_Zero_Or_Turns_Exhausted"),
    leaderBoard(NULL), culprit(NULL), solved1(false), solved2(false) {}

GameEngine::~GameEngine() { clearCharacters(); }

void GameEngine::attachLeaderBoard(LeaderBoard* b) { leaderBoard = b; }
bool GameEngine::isFinished() const { return state == STATE_EXIT; }

void GameEngine::clearCharacters() {            // free every character created with new
    for (int i = 0; i < characters.size(); i++) delete characters[i];
    characters.clear();
    culprit = NULL;
}

Character* GameEngine::createCharacter(const string& name, const string& role, const string& nat) {
    if (role == "Victim") return new Victim(name, role, nat);
    if (role == "Cook") return new Cook(name, role, nat);
    if (role == "Guard") return new Guard(name, role, nat);
    if (role == "Servant") return new Servant(name, role, nat);
    if (role == "Partner_1" || role == "Partner_2") return new Partner(name, role, nat);
    if (role == "PA") return new PA(name, role, nat);
    if (role == "Child") return new Child(name, role, nat);
    throw FileException("characters.txt has an unknown role: " + role);
}

// Reads rules.txt; falls back to default values if it is missing or corrupt.
void GameEngine::loadRules() {
    maxTurns = 25; startingCredibility = 100; wrongTurnPenalty = 5; wrongWordPenalty = 10;
    winCondition = "Accuse_Correct_Culprit"; loseCondition = "Credibility_Zero_Or_Turns_Exhausted";

    ifstream in("rules.txt");
    if (!in) { cout << "[!] rules.txt missing - using default rules.\n"; return; }
    int turns = maxTurns, cred = startingCredibility, turnPen = wrongTurnPenalty, wordPen = wrongWordPenalty;
    string win = winCondition, lose = loseCondition, line;
    while (getline(in, line)) {
        if (line.size() > 0 && line[line.size() - 1] == '\r') line.erase(line.size() - 1, 1);
        if (line.size() == 0) continue;
        int eq = -1;
        for (int i = 0; i < (int)line.size() && eq == -1; i++) if (line[i] == '=') eq = i;
        if (eq == -1) { cout << "[!] rules.txt corrupted - using default rules.\n"; return; }
        string key = "", value = "";
        for (int i = 0; i < eq; i++) key += line[i];
        for (int i = eq + 1; i < (int)line.size(); i++) value += line[i];
        int number = textToInt(value);
        if (number < 0) number = -number;       // penalties are written as negative numbers
        if (key == "Max_Turns") turns = number;
        else if (key == "Starting_Credibility") cred = number;
        else if (key == "Every_Wrong_Turn") turnPen = number;
        else if (key == "Every_Wrong_Word") wordPen = number;
        else if (key == "Win_Condition") win = value;
        else if (key == "Lose_Condition") lose = value;
    }
    if (turns < 1 || cred < 1) { cout << "[!] rules.txt has invalid values - using default rules.\n"; return; }
    maxTurns = turns; startingCredibility = cred; wrongTurnPenalty = turnPen; wrongWordPenalty = wordPen;
    winCondition = win; loseCondition = lose;
}

// Reads characters.txt, clue.txt and locations.txt and prepares a new game.
void GameEngine::loadGameData() {
    clearCharacters();
    cottage.clearLocations();
    solved1 = false; solved2 = false;
    string f[MAX_FIELDS];
    string rawLines[8];                         // keeps each character line so letters can be read later

    ifstream cf;
    openFile(cf, "characters.txt", true);
    while (readRow(cf, f, 12, "characters.txt")) {
        if (characters.size() >= 8)
            throw FileException("characters.txt must list exactly 8 people.");
        Character* made = createCharacter(f[0], fixRole(f[1]), f[2]);
        characters.add(made);
        string joined = "";
        for (int i = 0; i < MAX_FIELDS; i++) joined += f[i] + "|";
        rawLines[characters.size() - 1] = joined;
    }
    if (characters.size() != 8)
        throw FileException("characters.txt must list exactly 8 people.");

    int suspects = 0;                           // choose the culprit randomly among the 7 suspects
    for (int i = 0; i < characters.size(); i++) if (characters[i]->isSuspect()) suspects++;
    if (suspects != 7)
        throw FileException("characters.txt needs 7 suspects and 1 victim.");
    int pick = rand() % 7, seen = 0;
    for (int i = 0; i < characters.size(); i++) {
        if (characters[i]->isSuspect()) {
            if (seen == pick) culprit = characters[i];
            seen++;
        }
    }
    culprit->setGuilty(true);
    culpritRole = culprit->getRole();
    int col = roleColumn(culpritRole);
    if (col < 0)
        throw FileException("Unknown culprit role: " + culpritRole);

    ifstream kf;                                // the two words that match the culprit's role
    openFile(kf, "clue.txt", true);
    bool found = false;
    while (readRow(kf, f, 3, "clue.txt")) {
        if (fixRole(f[0]) == culpritRole) { word1 = toUpperString(f[1]); word2 = toUpperString(f[2]); found = true; }
    }
    if (!found)
        throw FileException("clue.txt has no profile for " + culpritRole + ".");
    scrabble.setTargets(word1, word2);

    for (int i = 0; i < characters.size(); i++) {   // letter and position of every person (Word 2)
        string raw = rawLines[i], cell = "";
        int index = 0;
        for (int k = 0; k < (int)raw.size(); k++) {
            if (raw[k] != '|') { cell += raw[k]; continue; }
            if (index == 3) characters[i]->setBoardPos(textToInt(cell));
            if (index == 4 + col && cell.size() > 0) characters[i]->setLetter(toUpperChar(cell[0]));
            index++;
            cell = "";
        }
    }

    ifstream lf;                                // letter and position of every location (Word 1)
    openFile(lf, "locations.txt", true);
    while (readRow(lf, f, 9, "locations.txt")) {
        Location l;
        l.locName = f[0];
        l.boardPosLoc = textToInt(f[1]);
        l.letterClueLoc = (f[2 + col].size() > 0) ? toUpperChar(f[2 + col][0]) : ' ';
        l.locRow = WORD_1;
        l.locSearchFlag = false;
        cottage.addLocation(l);
    }
    if (cottage.getLocationCount() != 7)
        throw FileException("locations.txt must list exactly 7 locations.");
}

void GameEngine::showWelcome() const {
    cout << "\n==================================================\n";
    cout << "            T H E   L O C K E R   H E I S T\n";
    cout << "==================================================\n";
}

// One pass of the game loop: finish the game if it is over, otherwise show the right menu.
void GameEngine::step() {
    if (state == STATE_PLAYING && (detective.getCredibility() <= 0 || detective.getTurns() >= maxTurns)) state = STATE_LOST;
    if (state == STATE_WON || state == STATE_LOST) concludeGame();
    else if (state == STATE_MENU) runMainMenu();
    else if (state == STATE_PLAYING) runGameTurn();
}

void GameEngine::runMainMenu() {
    cout << "\n+------------ THE LOCKER HEIST - MAIN MENU ------------+\n";
    cout << "  [1] New Game\n  [2] View Leader/Score Board\n  [3] About\n  [4] Rules\n  [5] Exit\n";
    int number = readInt("Choice: ", 1, 5);
    MenuChoice choice = (MenuChoice)number;
    if (choice == MENU_NEW_GAME) startNewGame();
    else if (choice == MENU_LEADERBOARD) {
        if (leaderBoard == NULL)
            throw FileException("No leader board attached.");
        leaderBoard->display();
    }
    else if (choice == MENU_ABOUT) {
        ifstream in;
        openFile(in, "about.txt", false);
        string line;
        cout << "\n";
        while (getline(in, line)) cout << line << "\n";
    }
    else if (choice == MENU_RULES) {
        loadRules();
        cout << "\n=============== RULES ===============\n";
        cout << "Maximum turns        : " << maxTurns << "\n";
        cout << "Starting credibility : " << startingCredibility << "\n";
        cout << "Every wrong turn     : -" << wrongTurnPenalty << "\n";
        cout << "Every wrong word     : -" << wrongWordPenalty << "\n";
        cout << "Win condition        : " << winCondition << "\n";
        cout << "Lose condition       : " << loseCondition << "\n";
        cout << "Viewing the board is free. Interview limits: Owner>=80, Child>=75, Partners>=50, PA>=35.\n";
    }
    else if (choice == MENU_EXIT) state = STATE_EXIT;
}

void GameEngine::startNewGame() {
    cout << "\n============ THE LOCKER HEIST - NEW GAME ============\n";
    string name = readLine("Enter detective name: ");
    if (name.size() == 0)
        throw InvalidInputException("Name cannot be empty.");
    bool hasSpace = false;
    for (int i = 0; i < (int)name.size(); i++) if (name[i] == ' ') hasSpace = true;
    if (hasSpace) {                             // spaces are replaced by '_' or '.'
        string r = readLine("Name contains spaces. Replace them with '_' or '.' ? Enter _ or . : ");
        if (r != "_" && r != ".")
            throw InvalidInputException("Please choose '_' or '.'.");
        for (int i = 0; i < (int)name.size(); i++) if (name[i] == ' ') name[i] = r[0];
    }
    loadRules();
    loadGameData();
    detective = Detective(name, startingCredibility);
    state = STATE_PLAYING;
    cout << "A culprit has been secretly chosen. The game begins!\n";
}

void GameEngine::runGameTurn() {
    cout << "\n";
    detective.display();
    cout << "Rack Word 1: "; detective.getRack(WORD_1).display();
    cout << "\nRack Word 2: "; detective.getRack(WORD_2).display();
    cout << "\n\n==== Game Menu ====\n";
    cout << "[1] Visit a Location\n[2] Interview a Person\n[3] View Scrabble Board (free - no turn)\n";
    cout << "[4] Form a Word\n[5] Accuse a Suspect\n[6] Quit\n";
    cout << "--- TURN " << detective.getTurns() + 1 << " ---\n";
    int choice = readInt("Choice: ", 1, 6);
    if (choice == 1) {
        cottage.displayLocations();
        int pick = readInt("Pick: ", 1, cottage.getLocationCount());
        int i = pick - 1;
        collect(cottage.at(i).locSearchFlag, cottage.at(i).locName, cottage.getClue(i), 0, "searched", "Search");
    }
    else if (choice == 2) interviewPerson();
    else if (choice == 3) {
        cout << "\n";
        scrabble.display();
    }
    else if (choice == 4) formWord();
    else if (choice == 5) accuseSuspect();
    else { cout << "You abandon the case. (Quit games are not recorded.)\n"; state = STATE_MENU; }
}

// Same steps for a location and for a person: first time = collect and place the letter,
// later = ask and show the letter again. 'done' is the searched/interviewed flag.
void GameEngine::collect(bool& done, const string& name, const Clue& c, int needCred, const string& past, const string& action) {
    int wordNo = (c.clueRow == WORD_1) ? 1 : 2;
    if (!done) {
        cout << "You have NOT " << past << " " << name << " yet.\n";
        if (!readYesNo(action + "? (Y/N): ")) return;               // answering N costs no turn
        if (detective.getCredibility() < needCred)
            throw LowCredibilityException(name + " refuses to talk: needs credibility of at least " + intToText(needCred) + ".", wrongTurnPenalty);
        detective.collectClue(c);
        scrabble.placeClue(c);
        done = true;
        cout << "Letter " << c.clueLetter << " collected at position " << c.cluePosition << " of Word " << wordNo << ".\n";
        cout << "Letter has been placed.\n";
    }
    else {
        cout << "You have already " << past << " " << name << ".\n";
        if (!readYesNo(action + " again? (Y/N): ")) return;
        cout << "Letter " << c.clueLetter << " is at position " << c.cluePosition << " of Word " << wordNo << ".\n";
    }
    detective.incrementTurns();
}

void GameEngine::interviewPerson() {
    cout << "\n===== Character Menu =====\n";
    for (int i = 0; i < characters.size(); i++) {
        cout << "[" << i + 1 << "] " << characters[i]->describe() << " - ";
        if (characters[i]->isInterviewed()) cout << "Interviewed\n";
        else cout << "NOT Interviewed\n";
    }
    int pick = readInt("Pick: ", 1, characters.size());
    Character* p = characters[pick - 1];
    // requiredCredibility() is virtual: each class gives its own limit (polymorphism)
    bool done = p->isInterviewed();
    collect(done, p->getName(), p->getClue(), p->requiredCredibility(), "interviewed", "Interview");
    p->setInterviewed(done);
}

void GameEngine::formWord() {
    int which = readInt("Which row? [1] Word 1  [2] Word 2 : ", 1, 2);
    WordRow row = WORD_2;
    if (which == 1) row = WORD_1;
    bool& solved = (row == WORD_1) ? solved1 : solved2;
    if (!scrabble.isRowComplete(row))
        throw WordNotReadyException("That row is not complete yet.");
    if (solved) { cout << "You already solved that word.\n"; return; }

    string typed = readLine("Type the word: ");
    if (!scrabble.checkWord(row, typed))
        throw InvalidWordException("'" + toUpperString(typed) + "' is not the correct word.", wrongWordPenalty);
    solved = true;
    detective.incrementTurns();
    cout << "Correct! " << toUpperString(typed) << " is right.\n";
    if (solved1 && solved2) {
        TileRack evidence = detective.getRack(WORD_1).merge(detective.getRack(WORD_2));
        cout << "\nBoth words solved. Evidence rack: ";
        evidence.display();
        cout << "\n" << word1 << " + " << word2 << " = " << culprit->getNationality() << " -> the " << culpritRole << " is the culprit!\n";
    }
}

void GameEngine::accuseSuspect() {
    cout << "\n===== Accuse a Suspect =====\n";
    int number = 0;
    for (int i = 0; i < characters.size(); i++) {
        if (characters[i]->isSuspect()) { number++; cout << "[" << number << "] " << characters[i]->describe() << "\n"; }
    }
    int pick = readInt("Pick: ", 1, number);
    Character* accused = NULL;
    int seen = 0;
    for (int i = 0; i < characters.size(); i++) {
        if (characters[i]->isSuspect()) {
            seen++;
            if (seen == pick) accused = characters[i];
        }
    }
    cout << "Accuse: " << accused->getName() << " (" << accused->getRole() << ")\n";
    if (!accused->isGuilty())
        throw WrongAccusationException(accused->getName() + " is innocent!", wrongTurnPenalty);
    detective.incrementTurns();
    cout << accused->getName() << " is the culprit!\n";
    state = STATE_WON;
}

void GameEngine::concludeGame() {
    bool won = (state == STATE_WON);
    int turnsLeft = maxTurns - detective.getTurns();
    if (turnsLeft < 0) turnsLeft = 0;

    cout << "\n+-------------------------------------------------------+\n";
    cout << "  " << detective.getName() << (won ? " WIN!" : " LOSE!") << "\n";
    cout << "  Final credibility: " << detective.getCredibility() << " || Turns used: " << detective.getTurns()
        << " || Culprit: " << culpritRole << " " << culprit->getName() << "\n";
    cout << "+-------------------------------------------------------+\n";

    state = STATE_MENU;                         // back to the main menu, even if saving fails
    if (leaderBoard != NULL)
        leaderBoard->addScore(detective.getName(), won ? "WIN" : "LOSE", detective.getCredibility(), turnsLeft, culpritRole);
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
    system("cls");                              // clear the screen (Windows console)
    cout << "Screen Cleared!\n";
}

// Called by main() for every caught exception: prints it and applies the penalty.
void GameEngine::handleException(const GameException& e) {
    cout << "\n[!] " << e.getType() << ": " << e.getMessage() << "\n";
    if (state == STATE_PLAYING) {
        if (e.getPenalty() > 0) {
            detective.loseCredibility(e.getPenalty());
            cout << "    Credibility -" << e.getPenalty() << " (now " << detective.getCredibility() << ")\n";
        }
        if (e.consumesTurn()) detective.incrementTurns();
    }
}