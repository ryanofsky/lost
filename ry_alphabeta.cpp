#include "ry_alphabeta.h"

#include <algorithm>
#include <assert.h>
using std::min;
using std::max;

extern const GameConstants GC;

/*

  Alpha-Beta-like search.
  
  The program assumes this ordering of players

  HOME
  AWAY
  HOMEP
  AWAYP

  The ordering affects the workings of the algorithm, but probably does not
  have much of an impact on results.
  
  https://www1.columbia.edu/sec/bboard/013/coms4701-001/msg00036.html
  
*/

Turn ABsearch::go(Game & game)
{
  Turn turn;
  int i(0), mx(0);

  game.findSuccessors(GC.HOME,0);
  assert(game.moves.size() > 0);
  
  while(game.moves.size() > 0)
  {
    Turn t = game.moves.top();
    game.moves.pop();

    Game succ(game, t);
    go(succ,1,GameConstants::AWAY);
    
    double score = succ.backedUp * 2 + t.eval;
    if (i == 0 || score > mx)
    {
      mx = score;
      turn = t;
    }
    ++i;
  }
  return turn;
}

void ABsearch::go(Game & game, int depth, int whoseturn)
{
  if (depth >= _maxdepth)
  {
    game.backedUp = game.eval();
  }
  else // if (depth < maxdepth)
  {
    // figure out which player goes next
    int nextturn = (whoseturn + 1) % GameConstants::NUM_PLAYERS;
    
    // generate successor nodes      
    game.findSuccessors(whoseturn, depth);
    assert(game.moves.size() > 0);
    
    int i(0);
    while(game.moves.size() > 0) // loop through successors
    {
      Turn t = game.moves.top();
      game.moves.pop();

      Game succ(game,t); // put a successor game in succ
      
      // search this successor
      go(succ, depth+1, nextturn);

      if (i == 0) // first successor
        game.backedUp = succ.backedUp;
      else
      {
        // normal minimax logic here
        if (whoseturn == GameConstants::HOME || whoseturn == GameConstants::HOMEP)
          game.backedUp = max(game.backedUp, succ.backedUp);
        else
          game.backedUp = min(game.backedUp, succ.backedUp);
      }
      
      // alpha-beta-like pruning logic here
      if (game.parent != NULL && game.parent->backedUp >= 0)
      {
        if (whoseturn == GameConstants::HOME || whoseturn == GameConstants::HOMEP)
        {
          if (game.backedUp > game.parent->backedUp) 
            return;
        }
        else
        {
          if (game.backedUp < game.parent->backedUp) 
            return;
        }
      } 
    } // for
  } // if (depth < maxdepth)
}