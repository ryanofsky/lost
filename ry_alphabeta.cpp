#include "ry_alphabeta.h"

#include <algorithm>
using std::min;
using std::max;

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
  Game succ;
  Turn turn;
  int mx(0);
  
  int n = game.findfirstsuccessors();
  for (int i = 0; i < n; ++i)
  {
    Turn t; double pscore;
    
    game.getsuccessor(i, succ, turn, pscore);
    go(succ,1,GameConstants::AWAY);
    double score = succ.backedUp + pscore * 2;
    if (i == 0 || score > mx)
    {
      mx = score;
      turn = t;
    }
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
    // variable to hold successor games
    Game succ;
    
    // generate successor nodes      
    int n = game.findsuccessors(whoseturn);

    // figure out which player goes next
    int nextturn = (whoseturn + 1) % GameConstants::NUM_PLAYERS;

    // -1 denotes a backed up value that hasn't been calculated yet
    game.backedUp = -1;

    for(int i = 0; i < n; ++i) // loop through successors
    {
      game.getsuccessor(i, succ); // put a successor game in succ
      
      // search this successor
      go(succ, depth+1, nextturn);

      if (game.backedUp < -1) // first successor
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
       
        switch(whoseturn)
        {
          case GameConstants::HOME:
          case GameConstants::HOMEP:
            if (game.backedUp > game.parent->backedUp) 
              return;
          break;
          case GameConstants::AWAY:
          case GameConstants::AWAYP:
            if (game.backedUp < game.parent->backedUp) 
              return;
          break;          
        }
      } 
    } // for
  } // if (depth < maxdepth)
}