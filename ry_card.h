#ifndef ry_card_h
#define ry_card_h

#include <vector>
#include "ry_gameconstants.h"
#include "ry_help.h"

using std::vector;
using std::cerr;
using std::endl;

class Card
{
public:  
  
  char color;
  char points;
  
  Card() : color(GameConstants::NONE), points(0) { }

  Card(char color_, char points_) : 
    color(color_), points(points_) { }
  
  void describe();
  
  static bool Parse(char c1, char c2, Card & c);
  int getIndex();
  
  operator bool()
  {
    return color != GameConstants::NONE;
  }
  

};

struct CardInfo
{
  Card card;
  short freq;
 
  CardInfo(Card card_, short freq_)
    : card(card_), freq(freq_)
  { }
};

class Stack // abstract class representing a stack of cards
{
public:
  virtual void addCard(Card c) = 0;
  virtual void setNumCards(int n) = 0;
  virtual void describe() = 0;
};

class NormalStack : public Stack // normal stack of cards
{
public:
  vector<Card> cards;

  NormalStack() : cards() { }
  
  virtual void addCard(Card c)
  {
    cards.push_back(c);
  }
  
  virtual void setNumCards(int) // not neccesary
  {
  }
  
  virtual void describe()
  {
    vector<Card>::iterator vi;     // construct iterator for an array of int

    bool first = true;
    
    
    if (cards.size() > 0)
    for (vi = cards.begin(); vi != cards.end(); ++vi)
    { 
      if (first) first = false; else cerr << ", ";
      vi->describe();
    }
    else
      cerr << "None";
    cerr << endl;
  }
};

class ProbStack : public Stack // probablistic stack :)
{
public:
  ProbStack() : numcards(0) { }
  
  void addCard(Card)
  {
  }
  
  void setNumCards(int n)
  {
    numcards = n;
  }

  virtual void describe()
  {
    cerr << numcards << " cards" << endl;
  }
private:
  int numcards;
};

#endif