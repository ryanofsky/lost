#ifndef ry_probstacks_h
#define ry_probstacks_h

#include "ry_gameconstants.h"

class ProbStacks
{
  enum { DRAWPILE = 0 };
  enum { DISCARD = 1 };
  enum { CAMPAIGNS = 2 };
  enum { HANDS = CAMPAIGNS + GameConstants::NUM_TEAMS };
  enum { NUM_STACKS = HANDS + GameConstants::NUM_PLAYERS };

  typedef bool const IsKnown[NUM_STACKS];

  float prob[NUM_STACKS][GameConstants::NUM_UCARDS];
  float stacksum[NUM_STACKS];
  float stackisum[NUM_STACKS];
  
  
  struct DiscardNode
  {
    short ordinal;
    short cardno;

    DiscardNode(short ordinal_, short cardno_)
    : ordinal(ordinal_), cardno(cardno_)
    { }
    
    bool operator < (DiscardNode const & dn)
    {
      return this->ordinal < dn.ordinal;
    }
  }
  
  priority_queue<DiscardNode> discardq[GameConstants::NUM_COLORS];
  
  int campaignmin[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  
  void clear();
  void addCard(int stack, int card);
  void setNumCards(int stack, int num);
  void normalize(const IsKnown isknown);
  void applySkew(ProbSkew & ps, IsKnown isknown);
  void passCard(int cardno, int source, int dest)
  {
    --stackisum[source];
    ++stackisum[dest];
    float maxmove = 1.0;
    if (cardno != GC.NONE)
    {
      maxmove = 
        min
        (
          min((float) 1, prob[source][cardno]),    
          (float)GC.cardInfo[cardno].freq - prob[dest][cardno])
        );
      
      prob[source][cardno] -= maxmove;
      prob[dest][cardno] += maxmove;
    }
    stacksum[source] -= maxmove;
    stacksum[dest] += maxmove;  
  }
};

class ProbSkew
{
  float skew[ProbStacks::NUM_STACKS][GameConstants::NUM_UCARDS];
  ProbTransform();
};

#endif
