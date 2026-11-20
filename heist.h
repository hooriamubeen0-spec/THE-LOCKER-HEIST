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

#endif