
#define CRT_SECURE_NO_WARNING
#include "heist.h"
#include <cstdlib>
#include<iostream>
#include<string>
#include <ctime>
int main() {

  srand((unsigned)time(0));                   // makes the culprit random in every game

  LeaderBoard board("leader/scoreBoard.txt"); // association: the engine uses it but does not own it
  GameEngine engine;
  engine.attachLeaderBoard(&board);
  engine.showWelcome();

  while (!engine.isFinished()) {
      try {
          engine.step();
      }
      catch (const GameException& e) {        // all custom exceptions are caught here
          engine.handleException(e);
      }
  }              
  cout << "\nGoodbye, Detective.\n";

    return 0;
}
