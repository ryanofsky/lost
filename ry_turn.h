#ifndef ry_turn_h
#define ry_turn_h

#include <iostream>
using std::cerr;
using std::cout;

class Turn
{
public:
  int move;
  Card card1, card2;
  
  Turn() : move(0), card1(), card2() { } 
  
  void output()
  {
    cout << "turn:";
    int m = move & ~GameConstants::RANDOMDRAW;
    if (m == GameConstants::PLAY)
      cout << "campaign-home-";
    else if (m == GameConstants::DROP)
      cout << "discard-";
    
    if (m == GameConstants::PLAY || m == GameConstants::DROP)
    {
      cout << GameConstants::GetLongColor(card1.color) << ';'
           << GameConstants::GetValue(card1.points) 
           << GameConstants::GetShortColor(card1.color) << ';';
      if ((move & GameConstants::RANDOMDRAW) != 0)
        cout << "drawpile;";
      else
      {
        cout << "discard-"
             << GameConstants::GetLongColor(card2.color) << ';'
             << GameConstants::GetValue(card2.points) 
             << GameConstants::GetShortColor(card2.color) << ';';            
      }  
    }
    else if (move == GameConstants::PASS)
    {
      cout << "pass;"
           << GameConstants::GetValue(card1.points) 
           << GameConstants::GetShortColor(card1.color) << ';'
           << GameConstants::GetValue(card2.points) 
           << GameConstants::GetShortColor(card2.color) << ';';
    }
    cout << endl;
  }

  void describe()
  {
    int m = move & ~GameConstants::RANDOMDRAW;
    if (m == GameConstants::PLAY)
    {
      cerr << "Played a ";
      card1.describe();
    }
    else if (m == GameConstants::DROP)
    {
      cerr << "Discarded a ";
      card1.describe();
    };
    
    if (m == GameConstants::PLAY || m == GameConstants::DROP)
    {
      cerr << " and picked up a ";
      if (card2)
        card2.describe();
      else
        cerr << "card";
      
      if ((move & GameConstants::RANDOMDRAW) != 0)
      {
         cerr << " from the draw pile";
      }
      else
      {
        cerr << " from a discard stack";
      }
    }
    else if (move == GameConstants::PASS)
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
};


#endif