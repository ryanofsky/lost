/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_gameconstants.h                    *
* Lost Cities Player Constants          *
****************************************/

#ifndef ry_gameconstants_h
#define ry_gameconstants_h

#include "ry_card.h"

#include <map>
#include <string>

using std::map;
using std::string;

class GameConstants
{
public:

  // counts
  enum { NUM_COLORS=5, NUM_PLAYERS=4, NUM_CARDS=75, NUM_UCARDS=50, NUM_TEAMS=2 };

  // all purpose constant
  enum { NONE = -1 }; 
  
  // colors 
  enum { RED=0, YELLOW, GREEN, BLUE, WHITE };
  
  // players
  enum { HOME = 0, AWAY, HOMEP, AWAYP };
  
  // teams
  enum { HOMETEAM = 0, AWAYTEAM };  
  
  // card values
  enum { KICKER=0, CARD_MIN=2, CARD_MAX=10 };
  
  // move types (RANDOMDRAW is treated like a flag)
  enum { PLAY = 1, DROP = 2, PASS = 3, RANDOMDRAW = 4 };

  // header tokens
  enum { HAND = 0, CAMPAIGN, DISCARD, TURN, DRAWPILE, HANDSIZE };

  // Lookup table for to find card values and frequencies using card indices
  struct CardInfo
  {
    Card card;
    short freq;
    CardInfo(Card card_, short freq_) : card(card_), freq(freq_) { }
  };
  static const CardInfo cardInfo[NUM_UCARDS];

  // Lookup table to find color names and symbols 
  struct ColorInfo
  {
    char symbol;
    char const * name;
  };
  static const ColorInfo colorInfo[NUM_COLORS];

  // Lookup table to find player names
  struct PlayerInfo
  {
    int team;
    const char * name;
    const char * possesive;
  };
  static const PlayerInfo playerInfo[NUM_PLAYERS];
  
  // Function to return point symbols
  static char pointSymbol(int p);

  // Lookup table to find card values using card symbols
  enum { CARDCHAR_S = '0', CARDCHAR_E = 'Z' };
  Card cardChar[CARDCHAR_E - CARDCHAR_S];

  // More lookup tables
  typedef map<string,int> SymTable;
  SymTable colors;
  SymTable tokens;
  SymTable players;
  SymTable moves;

  // Constructor
  GameConstants();
};

// The GameConstants global variable declaration
// (defined in ry_gameconstants.cpp)
extern const GameConstants GC;

#endif
