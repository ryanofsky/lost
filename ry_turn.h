/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_turn.h                             *
* Game Turn representation              *
****************************************/

#ifndef ry_turn_h
#define ry_turn_h

#include "ry_card.h"

// Turn class, describes a turn made by (any) player
class Turn
{
public:
  // possible moves are PLAY, DROP, PASS, RANDOMDRAW (defined in gameconstants)
  // randomdraw is treated like a flag. It can be ORed with DROP or PASS moves
  // randomdraw is set when at the end of the turn, the player picks up from
  // the drawpile instead of a discard pile
  int move;
  
  // If move is PASS, then card1 and card2 are set to the two cards that are
  // being passed. If the cards passed are unknown, then the card1 and card2
  // have color = NONE
  
  // If the move is PLAY or DROP, then card1 contains the card that was 
  // played on a campaign, or dropped on a discard stack. card2 contains
  // the card that was picked up at the end of the turn from either the
  // drawpile, or a discard pile. If card2 is unknown, it has color = NONE
  Card card1, card2;

  // used by the Game::FindSuccessors heuristic. Each potential move generated
  // by that function is given a value that is higher when the move is perceived
  // to be better for the current player
  float eval;

  // comparison operator that compares moves based on their eval values.
  // used when moves are stored in containers like Game::moves
  bool operator<(Turn const & t) const
  {
    return move != 0 ? this->eval < t.eval : t.move == 0 ? false : true;
  }

  // default constructor
  Turn() : move(0), card1(), card2(), eval(0.0) { } 
  
  // print a description of the move that can be read by the master program
  // to the standard output 
  void output();
  
  // print a desciption of the move that is useful for debugging 
  // to the standard error output
  void describe();
};


#endif
