#ifndef ry_turn_h
#define ry_turn_h

#include "ry_card.h"

class Turn
{
public:
  int move;
  Card card1, card2;
  float eval;
  
  Turn() : move(0), card1(), card2(), eval(0) { } 
  
  void output();
  void describe();
  
  bool operator<(Turn const & t)
  {
    return this->eval < t.eval;
  }

};


#endif
