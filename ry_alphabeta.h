#ifndef ry_alphabeta_h
#define ry_alphabeta_h

#include "ry_turn.h"
#include "ry_game.h"

// Alpha Beta Search Class
class ABsearch
{
public:
  
  // Constructor, maxdepth is the depth of the search tree
  ABsearch(int maxdepth) : _maxdepth(maxdepth)
  { }
  
  // Do alpha beta search on game. Last two arguments are used by the recursive function
  void go(Game & game, int depth = 0, bool pbackedUp = false);

protected:

  // depth of the search tree (# of moves to look ahead before evaluating)
  int _maxdepth;
};

#endif
