#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"

class Game
{
public:
  void parse(const string data);
  void describe()
  {
    cerr << "My Cards:  ";
    myhand.describe();
    cerr << endl << endl;
    
    
    for(int i = 0; i < 2; ++i)
    {
      if (i == 0)
        cerr << "My Campaigns:" << endl << endl;
      else
        cerr << "Opposing Campaigns:" << endl << endl;
    
      for(int j = 0; j < GameConstants::NUM_COLORS; ++j)
      {
        cerr << "  " << GameConstants::GetColor(j) << " ";
        cerr << "Campaign: ";
        campaigns[i][j].describe();
        cerr << endl;
      }
      cerr << endl;
      
      cerr << "Drawpile: ";
      drawpile.describe();
      cerr << endl << endl;
    }  
  }
  
private:  
  NormalStack myhand;
  ProbStack hands[GameConstants::NUM_PLAYERS];
  NormalStack discards[GameConstants::NUM_COLORS];
  NormalStack campaigns[2][GameConstants::NUM_COLORS];
  ProbStack drawpile;
  Turn lastturns[GameConstants::NUM_PLAYERS];  
};

#endif