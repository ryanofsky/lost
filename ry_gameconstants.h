#ifndef ry_gameconstants_h
#define ry_gameconstants_h

#include <STDDEF.H>
#include <map>
#include <string>

using std::map;
using std::string;
class Card;

class GameConstants // constants which are used all over the place
{

public:

  enum { RED=0, YELLOW, GREEN, BLUE, WHITE }; // colors
  enum { HAND = 0, CAMPAIGN, DISCARD, TURN, DRAWPILE, HANDSIZE }; // header tokens
  enum { HOME = 0, AWAY, HOMEP, AWAYP }; // players
  enum { PLAY = 1, DROP = 2, PASS = 3, RANDOMDRAW = 4 }; // move types, RANDOMDRAW is a flag
  enum { KICKER = -1, NONE = -1 }; // other constants 
  enum { CARD_MIN = 2, CARD_MAX = 10, NUM_COLORS = 5, NUM_PLAYERS = 4 }; // counts

  typedef map<string,int> SymTable;
  SymTable colors;
  SymTable tokens;
  SymTable players;
  SymTable moves;
  
  static const Card cardcodes[];
  static const size_t cardcodes_s;
  static const size_t cardcodes_e;
  
  //Card allcards[(CARD_MAX - CARD_MIN + 2) * NUM_COLORS];
  
  GameConstants()
  {
    colors["blue"] = BLUE;
    colors["green"] = GREEN;
    colors["red"] = RED;
    colors["white"] = WHITE;
    colors["yellow"] = YELLOW;
    
    tokens["hand"] = HAND;
    tokens["campaign"] = CAMPAIGN;
    tokens["discard"] = DISCARD;
    tokens["lastturn"] = TURN;
    tokens["drawpile"] = DRAWPILE;
    tokens["handsize"] = HANDSIZE;
    
    players["away"] = AWAY;
    players["awayP"] = AWAYP;
    players["home"] = HOME;
    players["homeP"] = HOMEP;
    
    moves["campaign"] = PLAY;
    moves["discard"] = DROP;
    moves["drawpile"] = RANDOMDRAW;
    moves["pass"] = PASS;
    
  }
  
  static const char * GetColor(int c);
  static const char * GetTeam(int t);
  static const char * GetLongColor(int c);
  static char GetShortColor(int c);
  static char GetValue(int v);  
};

#endif
