#ifndef ry_alphabeta_h
#define ry_alphabeta_h

#include "ry_gameconstants.h"
#include "ry_card.h"
#include "ry_turn.h"
#include "ry_game.h"

class ABsearch
{
public:
  ABsearch(int maxdepth) : _maxdepth(maxdepth)
  { }
  
  Turn go(Game & game);
  void go(Game & game, int depth, int whoseturn);

private:
  int _maxdepth;

};

#endif
