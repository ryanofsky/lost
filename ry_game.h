#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"

class Game
{
public:
  
  Game * parent;
  int backedUp;
  
  int findfirstsuccessors()
  {
    return 0;  
  }

  int findsuccessors(int whoseturn)
  {
    return whoseturn;  
  }
  
  void getsuccessor(int index, Game & succ)
  {
    succ.backedUp = index;
  }

  void getsuccessor(int index, Game & succ, Turn & turn, double & pscore)
  {
    Turn t(turn);
    succ.backedUp = index;
    pscore = 0.0;
    
  }
  
  int eval()
  {
    return 0;
  }
  
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
