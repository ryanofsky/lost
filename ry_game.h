#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"

class Game
{
public:
  void parse(const string data);
  void describe();  
public:  
  NormalStack myhand;
  ProbStack hands[GameConstants::NUM_PLAYERS];
  NormalStack discards[GameConstants::NUM_COLORS];
  NormalStack campaigns[2][GameConstants::NUM_COLORS];
  ProbStack drawpile;
  Turn lastturns[GameConstants::NUM_PLAYERS];  
};

#endif