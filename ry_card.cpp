#include "ry_card.h"
#include "ry_gameconstants.h"

#include <vector>
#include "ry_help.h"

using std::vector;
using std::cerr;
using std::endl;

Card::Card() :
  color(GameConstants::NONE), points(GameConstants::NONE)
{ }

Card(char color_, char points_) : 
  color(color_), points(points_)
{ }

void Card::describe()
{
  cerr << GameConstants::GetColor(color);
  if (points == GameConstants::KICKER)
    cerr << " Investment";
  else
    cerr << " " << (int)points;
}

bool Card::Parse(char c1, char c2, Card & c)
{
  const char s = GameConstants::cardcodes_s;
  const char e = GameConstants::cardcodes_e;
  
  if(isbetween(s,e,c1) && isbetween(s,e,c2))
  {
    const Card cd1 = GameConstants::cardcodes[c1 - s];
    const Card cd2 = GameConstants::cardcodes[c2 - s];
    
    if (cd1.color != GameConstants::NONE && cd2.points != 0 )
    {
      c.color = cd1.color;
      c.points = cd2.points;
      return true;
    }
    else if (cd2.color != GameConstants::NONE && cd1.points != 0 )
    {
      c.color = cd2.color;
      c.points = cd1.points;
      return true;
    }
  }
  return false;
}

int Card::getIndex()
{
  int p;

  if (color == GameConstants::NONE)
    return -1;

  if(isbetween((char)GameConstants::CARD_MIN,(char)GameConstants::CARD_MAX,points))
    p = points - GameConstants::CARD_MIN + 1;
  else if (points != GameConstants::KICKER)
    p = 0;
  else  
    return -1;
    
  return p+color*(GameConstants::CARD_MAX-GameConstants::CARD_MIN+2);
}

operator Card::bool()
{
  return color != GameConstants::NONE;
}

















class Stack // abstract class representing a stack of cards
{
public:
  virtual void addCard(Card c) = 0;
  virtual void setNumCards(int n) = 0;
  virtual void describe() = 0;
  virtual int getScore() = 0;
};

class NormalStack : public Stack // normal stack of cards
{
public:
  vector<Card> cards;
  typedef vector<Card>::iterator iterator;

  NormalStack() : cards() { }

  virtual int getScore()
  {
    int num[GameConstants::NUM_COLORS];
    int sum[GameConstants::NUM_COLORS];
    int mul[GameConstants::NUM_COLORS];
    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    {
      sum[i] = num[i] = 0;
      mul[i] = 1;
    }
    
    vector<Card>::iterator vi;     // construct iterator for an array of int
    for (vi = cards.begin(); vi != cards.end(); ++vi)
    { 
      if (*vi)
      {
        int c = vi->color;
        if (vi->points == GameConstants::KICKER)
          ++mul[c];
        else
          sum[c] += vi->points;
        ++num[c];
      }
    }
    
    int score = 0;
    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    {
      if (num[i] > 0)
      {
        score += (sum[i] - 20) * mul[i];
        if (num[i] >= 8)
          score += 20;
      }  
    }
    return score;
  }
  
  virtual void addCard(Card c)
  {
    cards.push_back(c);
  }
  
  virtual void setNumCards(int) // not neccesary
  {
  }
  
  virtual void describe()
  {
    iterator vi;     // construct iterator for an array of int

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
  
  NormalStack & operator= (NormalStack const & ns)
  {
    this->cards = ns.cards;
    return *this;
  }
};

class ProbStack : public Stack // probablistic stack :)
{
public:
  
  ProbStack() : numcards(0), probsum(0)
  {
    for(int i = 0; i < DIM(probs); ++i)
    {
      probs[i] = GameConstants::deckinfo[i].freq / GameConstants::NUM_CARDS;
      probsum += probs[i];
      GameConstants::NUM_CARDS;
    }
  }
  
  void inline setprob(int i, double prob)
  {
    prob /= numcards;
    probsum += prob - probs[i];
    probs[i] = prob;
  }

  void inline setprob(Card c, double prob)
  {
    int i = c.getIndex();
    setprob(i, prob);
  }

  double inline getprob(int i)
  {
    return probs[i]/probsum*(double)numcards;
  }

  virtual int getScore()
  {
    double num[GameConstants::NUM_COLORS];
    double sum[GameConstants::NUM_COLORS];
    double mul[GameConstants::NUM_COLORS];
    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    {
      sum[i] = num[i] = 0.0;
      mul[i] = 1.0;
    };
    
    for(int i = 0; i < DIM(probs); ++i)
    {
      int c = GameConstants::deckinfo[i].card.color;
      int p = GameConstants::deckinfo[i].card.points;
      double prob = getprob(i);
      if (p == GameConstants::KICKER)
        mul[c] += prob;
      else
        sum[c] += prob * p;
      num[c] += prob;
    };
    
    double score = 0;
    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    {
      if (num[i] > 1.5)
      {
        score += (sum[i] - 20.0) * mul[i];
        if (num[i] >= 7.0)
          score += 20.0;
      }  
    }
    return (int)score;
  }
  
  void copyNormal(NormalStack & n)
  {
    probsum = 0;
    for(int i = 0; i < DIM(probs); ++i)
      probs[i] = 0;

    NormalStack::iterator it;
    for (it = n.cards.begin(); it != n.cards.end(); ++it)
    { 
      p.setprob(*it, 1);
    }    
  }
  
  
  void addCard(Card c)
  {
    setprob(c, 1.0);
  }
  
  void setNumCards(int n)
  {
    numcards = n;
  }

  virtual void describe()
  {
    cerr << numcards << " cards" << endl;
  }
  
  ProbStack & operator= (ProbStack const & ns)
  {
    this->numcards = ns.numcards;
    this->probsum = ns.probsum;
    for(int i = 0; i < DIM(probs); ++i)
      this->probs[i] = ns.probs[i];
    return *this;
  }  
  
private:
  int numcards;
  int probsum;
  double probs[50]; // 50 = GameConstants::deckinfo_size
};






