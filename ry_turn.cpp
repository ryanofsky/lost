#include "ry_turn.h"
#include "ry_gameconstants.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

void Turn::output()
{
  cout << "turn:";
  int m = move & ~GC.RANDOMDRAW;
  if (m == GC.PLAY)
    cout << "campaign-home-";
  else if (m == GC.DROP)
    cout << "discard-";
  
  if (m == GC.PLAY || m == GC.DROP)
  {
    cout << GC.colorInfo[card1.color].name << ';'
         << GC.pointSymbol(card1.points) 
         << GC.colorInfo[card1.color].symbol << ';';
    if ((move & GC.RANDOMDRAW) != 0)
      cout << "drawpile;";
    else
    {
      cout << "discard-"
           << GC.colorInfo[card2.color].name << ';'
           << GC.pointSymbol(card2.points) 
           << GC.colorInfo[card2.color].symbol << ';';            
    }  
  }
  else if (move == GC.PASS)
  {
    cout << "pass;"
         << GC.pointSymbol(card1.points) 
         << GC.colorInfo[card1.color].symbol << ';'
         << GC.pointSymbol(card2.points) 
         << GC.colorInfo[card2.color].symbol << ';';
  }
  cout << endl;
}

void Turn::describe()
{
  int m = move & ~GC.RANDOMDRAW;
  if (m == GC.PLAY)
  {
    cerr << "Played a ";
    card1.describe();
  }
  else if (m == GC.DROP)
  {
    cerr << "Discarded a ";
    card1.describe();
  };
  
  if (m == GC.PLAY || m == GC.DROP)
  {
    cerr << " and picked up a ";
    if (card2)
      card2.describe();
    else
      cerr << "card";
    
    if ((move & GC.RANDOMDRAW) != 0)
    {
       cerr << " from the draw pile";
    }
    else
    {
      cerr << " from a discard stack";
    }
  }
  else if (move == GC.PASS)
  {
    if (!card1 && !card2) 
    {
      cerr << "Passed two cards";
    }
    else
    {
      cerr << "Passed a ";
      card1.describe();
      cerr << " and a ";
      card2.describe();
    }
  }
  else
  {
    cerr << "Unknown";
  }
}
