/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_alphabeta.cpp                      *
* Alpha Beta Search Class               *
****************************************/

#include "ry_alphabeta.h"
#include "ry_gameconstants.h"

#include <algorithm>
#include <assert.h>
#include <time.h>
#include <iostream>

using std::min;
using std::max;
using std::cerr;
using std::endl;

// maximum number of branches to search.
// since moves are on an ordered heap,
// this gives preference to the moves
// that the strategy algorithm says
// are better.
enum { BRANCHES = 4 };

// Alpha beta searches usually return
// in around 1 second, but sometimes
// they can go on for as much as 30.
// The maxtime parameter helps limit
// this. After the specified number
// of seconds has elapsed, the search
// is cut short. 
enum { MAXTIME = 10 };

void ABsearch::go(Game & game, int depth, bool pbackedUp)
{
  static time_t start_time;
  static int time_check(0);
  time_t t(0);
  if (time_check & 255 == 0)
  {
    time_check = 0;
    time(&t);
    
    if (depth == 0)
      start_time = t; 
  } 

  // terminating condition
  if (depth >= _maxdepth || (time_check == 0 && t - start_time > MAXTIME))
  {
    game.backedUp = game.eval();
    return;
  }

  // generate successor nodes      
  game.findSuccessors();
    
  // another reason to terminate
  if (game.moves_size() == 0)
  {
    game.backedUp = game.eval();
    return;
  }

  // true if game has a valid backed up value    
  bool backedUp = false;
  
  int i = 0;
  while(game.moves_size() > 0 && i < BRANCHES) // loop through successors
  {
    Turn t = game.moves_pop();

    // put a successor game in succ, takes a big chunk of stack space
    Game succ(game,t);
      
    // search this successor (calculate its backed up value)
    go(succ, depth+1, backedUp);

    int whoseteam = GC.playerInfo[game.whoseturn].team;

    // normal minimax logic here;
    bool update = !backedUp
      || (whoseteam == GC.HOMETEAM && game.backedUp < succ.backedUp)
      || (whoseteam == GC.AWAYTEAM && game.backedUp > succ.backedUp);
      
    if (update)
    {
      game.backedUp = succ.backedUp;
      backedUp = true;
      if (depth == 0) game.newturn = t;
    }

    // alpha-beta pruning logic here
    if (pbackedUp)
    {
      assert(game.parent != NULL);
      int parentteam = GC.playerInfo[game.parent->whoseturn].team;
      
      if (whoseteam != parentteam)
      {
        if (parentteam == GC.HOMETEAM && game.backedUp <= game.parent->backedUp)
          return;
         
        if (parentteam == GC.AWAYTEAM && game.backedUp >= game.parent->backedUp)
          return;
      }
    }
    ++i;
  } // while
}