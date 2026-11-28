#define _CRT_SECURE_NO_WARNINGS
#include "heist.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
using namespace std;
int main() {
    srand((unsigned)time(0));
    LeaderBoard board("leader/scoreBoard.txt");
    GameEngine& engine = GameEngine::getInstance();
    engine.attachLeaderBoard(&board);
    engine.showWelcome();
    while (!engine.isFinished()) {
        try {
            engine.step();
        }
        catch (const GameException& e) {
            engine.handleException(e);
        }
    }
    GameEngine::destroyInstance();
    cout << "\nGoodbye, Detective.\n";
    return 0;
}
