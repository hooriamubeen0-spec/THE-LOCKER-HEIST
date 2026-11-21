// heist.h - THE LOCKER HEIST: declarations of all classes
#ifndef HEIST_H
#define HEIST_H
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// ================= MODULE 1: Enums and Structs =================
enum WordRow { 
	WORD_1,
	WORD_2 
};
enum GameState { STATE_MENU,
	STATE_PLAYING,
	STATE_WON,
	STATE_LOST,
	STATE_EXIT
};
enum MenuChoice {
	MENU_NEW_GAME = 1,
	MENU_LEADERBOARD,
	MENU_ABOUT,
	MENU_RULES,
	MENU_EXIT
};
// Structs keep related data together and are reused by many classes.
struct Tile { 
	char tileLetter;
    bool filled; 
};
struct Clue { 
	char clueLetter;
	int cluePosition; 
	WordRow clueRow; 
};
struct Location { 
	string locName; 
	char letterClueLoc;
	int boardPosLoc;
	WordRow locRow; 
	bool locSearchFlag;
};

// ================= Helper functions =================
const int MAX_FIELDS = 12;                      // Most columns any data file row can have
char toUpperChar(char c);
string toUpperString(string s);
string intToText(int n);
int textToInt(const string& s);
string readLine(const string& prompt);
int readInt(const string& prompt, int low, int high);
bool readYesNo(const string& prompt);
bool openFile(ifstream& in, const string& name, bool skipHeader);
bool readRow(ifstream& in, string fields[], int minColumns, const string& fileName);
string fixRole(string role);
int roleColumn(const string& role);
bool sameClue(const Clue& a, const Clue& b);    // Compares letter, position and row

// ================= MODULE 3: Character hierarchy =================
class Character {
protected:
    string characterName;
    string characterRole;
    string nationality;
    bool isCulprit;
    bool interviewed;
    WordRow clueRowChar;
    char letterClueChar;
    int boardPosChar;
public:
    Character(const string& n, const string& r, const string& nat);
    virtual ~Character();
    virtual void display() const;                              // virtual: derived classes override it
    virtual int requiredCredibility() const;
    virtual bool isSuspect() const;
    string getName() const;
    void setClue(char letter, int pos);
    Clue getClue() const;
    string getRole() const;
    string getNationality() const;
    bool isGuilty() const;
    void setGuilty(bool value);
    bool isInterviewed() const;
    void setInterviewed(bool value);
    void setBoardPos(int pos);
    void setLetter(char letter);
    string describe() const;
};

class Victim : public Character {
public:
    Victim(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class Suspect : public Character {
public:
    Suspect(const string& n, const string& r, const string& nat);
    void display() const;
    bool isSuspect() const;
};

// Virtual inheritance: PA gets only ONE Suspect part (solves the diamond problem).
class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class CloseRelations : virtual public Suspect {
public:
    CloseRelations(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class HouseHelp : public Suspect {
public:
    HouseHelp(const string& n, const string& r, const string& nat);
    void display() const;
};

class Partner : public BusinessRelations {
public:
    Partner(const string& n, const string& r, const string& nat);
};

class Child : public CloseRelations {
public:
    Child(const string& n, const string& r, const string& nat);
};

class PA : public BusinessRelations, public CloseRelations {   // the diamond
public:
    PA(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class Cook : public HouseHelp { public: Cook(const string& n, const string& r, const string& nat); };
class Guard : public HouseHelp { public: Guard(const string& n, const string& r, const string& nat); };
class Servant : public HouseHelp { public: Servant(const string& n, const string& r, const string& nat); };

class Detective : public Character {
    int credibility;
    int turns;                                  // the engine increments, the detective only stores
public:
    Detective(const string& n = "Detective", int cred = 100);
    void display() const;
    int getCredibility() const;
    void loseCredibility(int amount);
    int getTurns() const;
    void incrementTurns();
};

#endif