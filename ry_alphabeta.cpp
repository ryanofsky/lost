#include "ry_alphabeta.h"
#include "ry_gameconstants.h"

#include <algorithm>
#include <assert.h>
using std::min;
using std::max;

void ABsearch::go(Game & game, int depth, bool pbackedUp)
{
  // terminating condition
  if (depth >= _maxdepth)
  {
    game.backedUp = game.eval();
    return;
  }

  // generate successor nodes      
  game.findSuccessors();
    
  // another reason to terminate
  if (game.moves.size() == 0)
  {
    game.backedUp = game.eval();
    return;
  }

  // true if game has a valid backed up value    
  bool backedUp = false;
  
  while(game.moves.size() > 0) // loop through successors
  {
    Turn t = game.moves.top();
    game.moves.pop();

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
  } // while
}