/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_cardstacks.h                       *
* Card Stack and Stack Skew Classes     *
****************************************/

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
// is the number of cards of type "cardno" which are in stack "stackno". All of
// these numbers togther form a table of distributions. The simple 2d array is
// able to completely describe the locations of all cards and the contents of
// all the stacks in a way which is easy for all the other parts of the program
// to deal with.
//
// Of course, not all of this information is actually known to the program.
// After the game state has been parsed, the contents of the current player's
// hand, the discard piles, and the campaigns are known, but the contents of all
// other stacks are unknown. Even though it is impossible to know exactly what
// is in these stacks, it is useful for the program to have some general idea.
// To do this, it uses the schema descibed above, but now allows for
// *fractional* card distributions in addition to discrete ones. for example, if
// the program knows that the Red 5 card is not in any of the known stacks, it
// puts a piece of the card in the four unknown stacks. The distribution of the
// red 5 will be 0.25 in the 4 unknown stacks and 0.0 for the 4 known stacks.
// (These calculations are peformed in the normalize() method which is called
// after the game state is parsed.)
//
// After fractional distributions enter the picture, all the other parts of the
// program have to understand how to understand and use them. In most cases,
// dealing with fractional distributions is very straightforward. For example
// the routine which calculates the score value of a campaign composed of
// fractional cards uses the exact same formula used in the real game, except
// that it computes with floats instead of integers (see Game::getScore()).
//
// Other parts of the program are more complicated but similar. A good example
// of a situation like this occurs during in the alpha beta search where
// different players' moves are simulated in order to generate future game
// states. Suppose the program is generating possible moves for an opposing
// player who has campaigned a red investment card and has the 0.25 Red 5 card
// distribution in his stack. There is a good chance that the player might play
// the 5 card if he had it, so this move is worth simulating. The most
// straightforward way to do this is just to subtract the 0.25 distribution from
// his stack and to add it to his team's campaign stack. This is unsatisfactory
// because
//
//   1) There is only a 25% chance that he actually has the card, which makes
//   the results of the simulation less meaningful
//
//   2) Adding .25 of a card to a campaign doesn't have much of an impact on the
//   score.
//
// To avoid these problems, the simulation will not deal with individual cards
// but instead uses groups of nearby cards whose distribution values add up to
// 1. In this case, say the player also has .50 of a Red 3 and .25 of a Red 4.
// The program will subtract ALL of these probabilities from his stack and add
// them to the campaign stack. this produces results which are meaningful
// because:
//
//   1) There is a very good chance that the player has either a red 3, 4, or 5.
//   2) The score calculated by the fractional scoring formulas will give
//   results that are close to what the real score would be if he had played one
//   of these cards.
//
// This card grouping method is used in various places in the game class and
// also inside the CardStacks::moveCard() method.
//
// One place that interacts with the card distribution in a completely different
// way is the Game::countCards() method. This method examines the previous moves
// made by players and adjusts the distribution in the unknown stacks so they
// can be more useful to heuristics that are called later in the program. For
// example, when countCards() sees that an opposing player discarded a green
// investment card on his last move, it *decreases* the probabilities that that
// player had other green cards. If it sees a player campaign a green investment
// card, it instead *increases* the distribution of green cards in that players
// stack.
//
// It would be difficult for countCards() to modify the distribution table
// directly, because each change in any one number potentially means that all of
// the other numbers in the table need to be updated. There are two rules that
// govern the distribution of numbers in the table:
//
//   1) For each type of card, the sums of the distributions of that card across
//   all stacks will equal the number of instances of that card.
//
//   2) For each stack, the sum of the distributions of all cards in the stack
//   will add up to equal the number of cards in that stack.
//
// These are not easy restrictions for an imprecise algorithm like countCards()
// to meet. Finding numbers that lead to the right sums in the table is akin to
// finding a solution for an 8x50 magic square.
//
// Another problem is that the countCards() method has to combine the results of
// many observations, each made individually about a player's last move. Making
// changes directly to the table after each observation could favor the earlier
// observations over the later ones, or favor the later ones, or else completely
// skew the table so it wouldn't reflect any of the original observations at
// all.
//
// To solve all of these problems and make the code simple there is a layer of
// indirection. Instead of modifying the distribution table directly, there is a
// secondary table called a "skew table" which is modified instead. The skew
// table has a value for every value in the distribution table. All of the
// values are initially set to 0. They are then added to or subtracted from to
// denote increases or decreases in the final distribution table. For example,
// the countCards() function adds a small number like 0.5 to the skew table when
// it wants to increase slightly the distribution of a particular card in a
// particular stack. It would add a large number like 2 or 3 to increase
// substantially the distribution, subtract 0.5 to decrease slightly, and
// subtract 3 to decrease substantially. With the skew table, conflicting
// observations cancel out, and observations that agree add up.
//
// After all observations have been made the skew is applied with the
// CardStacks::applyskew() method. The applyskew() is quick and not very
// complicated. The first thing it does map the skew values into distribution
// values using an exponential function (2^-x) that asymptotically approaches
// the highest and lowest possibly skew values. (So 0 has no effect, positive
// infinity gives the highest possible value and negative infinity gives the
// lowest possible value). In the next step, the distribution values for each
// card across the 8 stacks are normalized so rule #1 above is satisfied. Also
// during this second step, the sum of the distributions in each stack is
// calculated and stored in an array called stacksum[]. When distributions are
// used later on in the program they are normalized again using the values in
// the stacksum[] array to satisfy rule #2 (this occurs inside the getnum()
// method). Because normalization occurs twice, the two rules mentioned above
// are never completely and simultaneously satisfied once a skew has been
// applied. But, the ultimate goal is to be able to come up with an approximate
// distribution that reflects all of the observations made. In the end, the
// results produced seem to come close to satisfying both conditions and they do
// clearly reflect the observations.

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
