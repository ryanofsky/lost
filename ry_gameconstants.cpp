/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_gameconstants.cpp                  *
* Lost Cities Player Constants          *
****************************************/

#include "ry_gameconstants.h"
#include "ry_card.h"

#include <map>
#include <string>
#include <assert.h>

using std::map;
using std::string;

// FRIENDLY GLOBAL VARIABLE
const GameConstants GC;

const GameConstants::CardInfo GameConstants::cardInfo[NUM_UCARDS] = 
{
  CardInfo( Card(RED,KICKER),    3 ),  // 0 
  CardInfo( Card(RED,2),         2 ),  // 1 
  CardInfo( Card(RED,3),         2 ),  // 2 
  CardInfo( Card(RED,4),         2 ),  // 3 
  CardInfo( Card(RED,5),         1 ),  // 4 
  CardInfo( Card(RED,6),         1 ),  // 5 
  CardInfo( Card(RED,7),         1 ),  // 6 
  CardInfo( Card(RED,8),         1 ),  // 7 
  CardInfo( Card(RED,9),         1 ),  // 8 
  CardInfo( Card(RED,10),        1 ),  // 9 
  CardInfo( Card(YELLOW,KICKER), 3 ),  // 10 
  CardInfo( Card(YELLOW,2),      2 ),  // 11 
  CardInfo( Card(YELLOW,3),      2 ),  // 12 
  CardInfo( Card(YELLOW,4),      2 ),  // 13 
  CardInfo( Card(YELLOW,5),      1 ),  // 14 
  CardInfo( Card(YELLOW,6),      1 ),  // 15 
  CardInfo( Card(YELLOW,7),      1 ),  // 16 
  CardInfo( Card(YELLOW,8),      1 ),  // 17 
  CardInfo( Card(YELLOW,9),      1 ),  // 18 
  CardInfo( Card(YELLOW,10),     1 ),  // 19 
  CardInfo( Card(GREEN,KICKER),  3 ),  // 20 
  CardInfo( Card(GREEN,2),       2 ),  // 21 
  CardInfo( Card(GREEN,3),       2 ),  // 22 
  CardInfo( Card(GREEN,4),       2 ),  // 23 
  CardInfo( Card(GREEN,5),       1 ),  // 24 
  CardInfo( Card(GREEN,6),       1 ),  // 25 
  CardInfo( Card(GREEN,7),       1 ),  // 26 
  CardInfo( Card(GREEN,8),       1 ),  // 27 
  CardInfo( Card(GREEN,9),       1 ),  // 28 
  CardInfo( Card(GREEN,10),      1 ),  // 29 
  CardInfo( Card(BLUE,KICKER),   3 ),  // 30 
  CardInfo( Card(BLUE,2),        2 ),  // 31 
  CardInfo( Card(BLUE,3),        2 ),  // 32 
  CardInfo( Card(BLUE,4),        2 ),  // 33 
  CardInfo( Card(BLUE,5),        1 ),  // 34 
  CardInfo( Card(BLUE,6),        1 ),  // 35 
  CardInfo( Card(BLUE,7),        1 ),  // 36 
  CardInfo( Card(BLUE,8),        1 ),  // 37 
  CardInfo( Card(BLUE,9),        1 ),  // 38 
  CardInfo( Card(BLUE,10),       1 ),  // 39 
  CardInfo( Card(WHITE,KICKER),  3 ),  // 40 
  CardInfo( Card(WHITE,2),       2 ),  // 41 
  CardInfo( Card(WHITE,3),       2 ),  // 42 
  CardInfo( Card(WHITE,4),       2 ),  // 43 
  CardInfo( Card(WHITE,5),       1 ),  // 44 
  CardInfo( Card(WHITE,6),       1 ),  // 45 
  CardInfo( Card(WHITE,7),       1 ),  // 46 
  CardInfo( Card(WHITE,8),       1 ),  // 47 
  CardInfo( Card(WHITE,9),       1 ),  // 48 
  CardInfo( Card(WHITE,10),      1 ),  // 49 
};

const GameConstants::ColorInfo GameConstants::colorInfo[NUM_COLORS] = 
{
  { 'R', "red"    },
  { 'Y', "yellow" },
  { 'G', "green"  },
  { 'B', "blue"   },
  { 'W', "white"  }
};

const GameConstants::PlayerInfo GameConstants::playerInfo[NUM_PLAYERS] =
{
  { HOMETEAM, "home",  "My" },
  { AWAYTEAM, "away",  "My Opponent's" },
  { HOMETEAM, "homeP", "My Partner's"  },
  { AWAYTEAM, "awayP", "My Opponent's Partner's" }
};

char GameConstants::pointSymbol(int p)
{
  if (CARD_MIN <= p && p <= 9)
    return p + 0x30;
  else if (p == GameConstants::KICKER)
    return 'K';
  else if (p == 10)
    return 'T';
  else
  {
    assert(false);
    return 0;
  }  
}

GameConstants::GameConstants()
{
  // initialize nonstatic lookup tables
 
  cardChar[pointSymbol(KICKER) - CARDCHAR_S].points = KICKER;
  for(int i = CARD_MIN; i <= CARD_MAX; ++i)
    cardChar[pointSymbol(i) - CARDCHAR_S].points = i;
  for(int i = 0; i < NUM_COLORS; ++i)
    cardChar[colorInfo[i].symbol - CARDCHAR_S].color = i;

  for(int i = 0; i < NUM_PLAYERS; ++i)
    players[playerInfo[i].name] = i;

  for(int i = 0; i < NUM_COLORS; ++i)
    colors[colorInfo[i].name] = i;
    
  for(int i = 0; i < NUM_COLORS; ++i)
    colors[colorInfo[i].name] = i;

  tokens["hand"] = HAND;
  tokens["discard"] = DISCARD;
  tokens["campaign"] = CAMPAIGN;
  tokens["lastturn"] = TURN;
  tokens["drawpile"] = DRAWPILE;
  tokens["handsize"] = HANDSIZE;
    
  moves["campaign"] = PLAY;
  moves["discard"] = DROP;
  moves["pass"] = PASS;
  moves["drawpile"] = RANDOMDRAW;

};
