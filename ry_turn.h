#ifndef ry_turn_h
#define ry_turn_h

#include "ry_card.h"

class Turn
{
public:
  int move;
  Card card1, card2;
  
  Turn() : move(0), card1(), card2() { } 
  
  void output();
  void describe();

};


#endif