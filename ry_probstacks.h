#ifndef ry_probstacks_h
#define ry_probstacks_h

#include "ry_gameconstants.h"
#include <queue>


using std::priority_queue;

class ProbSkew;

class ProbStacks
{
public:
  enum { DRAWPILE = 0 };
  enum { DISCARD = 1 };
  enum { CAMPAIGNS = 2 };
  enum { HANDS = CAMPAIGNS + GameConstants::NUM_TEAMS };
  enum { NUM_STACKS = HANDS + GameConstants::NUM_PLAYERS };

  typedef float ProbMatrix[NUM_STACKS][GameConstants::NUM_UCARDS];
  typedef bool const IsKnown[NUM_STACKS];

  ProbMatrix prob;
  float stacksum[NUM_STACKS];
  float stackisum[NUM_STACKS];
  int discardOrder[GameConstants::NUM_CARDS]; // card indices
  int discardSize[GameConstants::NUM_COLORS]; // size of discard piles
  
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
  };
  
  int campaignmin[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  
  void clear();
  void addCard(int stack, int card);
  void setNumCards(int stack, int num);
  void normalize(IsKnown isknown);
  void applySkew(ProbSkew & ps, IsKnown isknown);
  void passCard(int cardno, int source, int dest, bool fractional = false);
  float getProb(int stack, int card);
  int discardTop(int color);
};

class ProbSkew
{
public:
  ProbStacks::ProbMatrix skew;
  ProbSkew();
  void ProbTransform();
};

#endif
