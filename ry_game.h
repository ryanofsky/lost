#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"
#include "ry_cardstacks.h"
#include <algorithm>
#include <queue>

using std::min;
using std::priority_queue;

// Game Class
// Holds the state of the game at a single moment in time

class Game
{
public:

  // Locations of all the cards in the game
  CardStacks stacks;
  
  // Players' last turns, indexed by player number
  Turn lastturns[GameConstants::NUM_PLAYERS];  

  // Whose turn is it? (holds a player number)
  int whoseturn;

  // default constructor
  Game();

  // load the game state from a state string
  void parse(const string data);

  // this method is called after the initial game state is loaded
  // it analyses the past moves and modifies the card distributions
  // accordingly. For example, if a player's last move was to play
  // a green investment card, then the it is likely that player
  // has more green cards in his / her hand
  void countCards();

  // print a description of the game state to the standard error output
  // (just used for debugging)
  void describe();

  // The next few variables and methods are used in the alpha beta search

  // pointer to the game state from one move ago
  Game const * parent;

  // backed up value during the AB search
  float backedUp;
  
  // Result of the AB search, what is the the best move to make.
  Turn newturn;
 
  // list of possible moves that can be made by the
  // current player in the game
  priority_queue<Turn> moves;
  
  // which card to be picked up at the end of the turn.
  // ignored for pass moves. If card color is NONE, it indicates
  // that the player should pick up from a draw pile. otherwise
  // this card will be one which is currently at the top of
  // one of the four discard piles.
  Card pickup;
  
  // generates and evaluates possible moves that can be made by the
  // current player, stores results in the priority queue.
  // also determines the best pickup card
  void findSuccessors();
 
  // constructor. constructs a new game state that describes what the
  // game would look like after a the current player in game "g" 
  // makes the move "t"
  Game(Game const & g, Turn & t);

  // evaluation function for this game state. used to evaluate the
  // tip nodes in the alpha beta search
  float eval();
 
protected:
  // protected members are used internally by the public methods

  // helps calculate the score for a campaign of a certain color
  //
  // points is the sum of the values the campaign cards
  // investments is the number of investment cards
  // num is the total number of cards
  float getScore(float points, float investments, float num);

  // cached (redundant) information about campaigns used to calculate scores
  float points[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  float investments[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];
  float counts[GameConstants::NUM_TEAMS][GameConstants::NUM_COLORS];

  // fills the three previous arrays with data about the campaigns
  void examineCampaigns();

  // just an array of booleans, used as an argument to CardStacks::Normalize.
  // value is true for card stacks whose cards are fed to us in the game state
  // such as the current players hand, the discard piles etc. false for stacks
  // whose contents will be determined by what is left over, such as the other
  // players' hands and the drawpile
  static const CardStacks::IsKnown isknown;
  
};

#endif
