#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"

class Game
{
public:
  
  Game * parent;
  int backedUp;

  void countCards()
  {
    emptyProbStacks(myhand);
    
    for(int i = 1; i < GameConstants::NUM_COLORS + 1; ++i)
      emptyProbStacks(discards[i]);
    
    for(int i = 1; i < GameConstants::NUM_COLORS + 1; ++i)
    {
      emptyProbStacks(campaigns[0][i]);
      emptyProbStacks(campaigns[1][i]);
    }
  }  

  int findFirstSuccessors()
  {
    return 0;  
  }

  int findSuccessors(int whoseturn)
  {
    return whoseturn;  
  }
  
  void getSuccessor(int index, Game & succ)
  {
    succ.backedUp = index;
  }

  void getSuccessor(int index, Game & succ, Turn & turn, double & pscore)
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

private:  
  void inline emptyProbStacks(NormalStack & n)
  {
    NormalStack::iterator it;
    for (it = n.cards.begin(); it != n.cards.end(); ++it)
    { 
      drawpile.setprob(*it, 0);
      for(int i = 0; i < GameConstants::NUM_PLAYERS; ++i)
        hands[i].setprob(*it, 0);
    }
  }
};

#endif
