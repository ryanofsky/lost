#ifndef ry_cardstacks_h
#define ry_cardstacks_h

#include "ry_gameconstants.h"

// forward declaration, see below for class definition
class StackSkew;

// The CardStacks class holds information about the locations of all cards in 
// the game. Each card must be in one of these "stacks":
//
//   1 Draw pile stack
//   1 Discard stack (all five colors are combined into one stack)
//   2 Campaign stacks (one stack for each team, different colors combined)
//   4 Hand stacks (one for each player)
//
// There are 8 stacks in all and each stack is given a number from 0-7.
// Stack numbers are determined from the constants defined at the top of
// the class.
//
// There are 75 cards total in the game, but only 50 unique cards. Each of the
// 50 cards has a unique index (given by Card::getIndex() or by the ordering
// of the GameConstants::cardInfo array). Card indices range from 0-49.
// 
// Information about the locations of cards in the game is given by a 2 two
// dimensional array in the "dist" member which is indexes by stack number
// and card number. The number at
//
//   dist[stackno][cardno]
//
// is the number of cards of type "cardno" which are in stack "stackno".
//
// When the game state loads at the beginning of the program's execution
// the Game::parse routine will be able to determine exactly what cards
// are in the current player's hand, the discard piles, and the campaign
// piles. There won't be exact information about the distribution of 
// cards in other piles such as the drawpile and other players hands.
// 

class CardStacks
{
public:
  // Give numbers to the stacks that can be used as indices
  // into ProbMatrix and IsKnown arrays
  enum { DRAWPILE = 0 };
  enum { DISCARD = 1 };
  enum { CAMPAIGNS = 2 };
  enum { HANDS = CAMPAIGNS + GameConstants::NUM_TEAMS };
  enum { NUM_STACKS = HANDS + GameConstants::NUM_PLAYERS };

  typedef float DistMatrix[NUM_STACKS][GameConstants::NUM_UCARDS];
  typedef bool const IsKnown[NUM_STACKS];

  DistMatrix dist;
  float stacksum[NUM_STACKS];
  float stackisum[NUM_STACKS];
  int discardOrder[GameConstants::NUM_CARDS]; // card indices
  int discardSize[GameConstants::NUM_COLORS]; // size of discard piles
  
  // highest campaign card. # of points, or NONE
  int campaignmin[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  
  void clear();
  void addCard(int stack, int card);
  void setNumCards(int stack, int num);
  void normalize(IsKnown isknown);
  void applySkew(StackSkew & ss, IsKnown isknown);
  void passCard(int cardno, int source, int dest, bool fractional = false);
  float getnum(int stack, int card);
  int discardTop(int color);
  bool isPlayable(int team, int color, int & cardno);
};

class StackSkew
{
public:
  CardStacks::DistMatrix skew;
  StackSkew();
  void StackTransform();
};

#endif
