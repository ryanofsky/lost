// real work
// TODO: get rid of discard pile priority queue. it is instead a 
// vector. check discard pile input. first card inputted is on 
// bottom, last card (pickupable) is on top

// real work
// TODO: finish up Game psuedo-copy constructor

// administravia
// TODO: extract methods, rebuild makefile, compile, clue alphabeta in on the new Game interface

#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"
#include "ry_probstacks.h"
#include <algorithm>

using std::min;

class Game
{
public:
  
  Game const * parent;
  int backedUp;

  ProbStacks stacks;
  Turn lastturns[GameConstants::NUM_PLAYERS];  

  void countCards();
  float getScore(float points, float investments, float num);
  
  float points[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  float investments[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  float counts[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];

  void calcScores();
  
  priority_queue<Turn> moves;
  int depth;
  int me;
  Card pickup;
  
  void findSuccessors(int me, int depth);
  Game() {}
  Game(Game const & g, Turn & t);
  
  int eval();
 
  void parse(const string data);
  void describe();
  


private:  

  static const ProbStacks::IsKnown isknown;
  
  Turn firstturns[4];
  int firstturnc;
};

#endif
