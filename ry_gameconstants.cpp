#include "ry_gameconstants.h"
#include "ry_card.h"

GameConstants GC;
extern GameConstants GC;

const char * GameConstants::GetColor(int c)
{
  switch(c)
  {
    case GameConstants::RED:
      return "Red";
    break;
    case GameConstants::YELLOW:
      return "Yellow";
    break;
    case GameConstants::GREEN:
      return "Green";
    break;
    case GameConstants::BLUE:
      return "Blue";
    break;
    case GameConstants::WHITE:
      return "White";
    break;
    default:
     return "Unknown";
    break;
  }
}

const char * GameConstants::GetLongColor(int c)
{
  switch(c)
  {
    case GameConstants::RED:
      return "red";
    break;
    case GameConstants::YELLOW:
      return "yellow";
    break;
    case GameConstants::GREEN:
      return "green";
    break;
    case GameConstants::BLUE:
      return "blue";
    break;
    case GameConstants::WHITE:
      return "white";
    break;
    default:
      BARF("GameConstants::GetLongColor(int) was passed an invalid color.");
    break;
  }
  return NULL;
}

char GameConstants::GetShortColor(int c)
{
  switch(c)
  {
    case GameConstants::RED:
      return 'R';
    break;
    case GameConstants::YELLOW:
      return 'Y';
    break;
    case GameConstants::GREEN:
      return 'G';
    break;
    case GameConstants::BLUE:
      return 'B';
    break;
    case GameConstants::WHITE:
      return 'W';
    break;
    default:
      BARF("GameConstants::GetShortColor(int) was passed an invalid color.");
    break;
  }
  return 0;
}

char GameConstants::GetValue(int v)
{
  if (0 <= v && v <= 9)
    return v + 0x30;
  else if (v == GameConstants::KICKER)
    return 'K';
  else if (v == 10)
    return 'T';
  else  
    BARF("GameConstants::GetValue(int) was passed an invalid value.");
  return 0;  
}

const char * GameConstants::GetTeam(int t)
{
  switch (t)
  {
    case GameConstants::HOME:
      return "My";
    break;
    case GameConstants::AWAY:
      return "My opponent's";
    break;
    case GameConstants::HOMEP:
      return "My partner's";
    break;
    case GameConstants::AWAYP:
      return "My opponent's partner's";
    break;  
    default:
      return "Sombody's";
    break;
  }
}

const Card GameConstants::cardcodes[] = 
{
  Card( NONE,  0 ),        // 0x30 '0'
  Card( NONE,  0 ),        // 0x31 '1'
  Card( NONE,  2 ),        // 0x32 '2'
  Card( NONE,  3 ),        // 0x33 '3'
  Card( NONE,  4 ),        // 0x34 '4'
  Card( NONE,  5 ),        // 0x35 '5'
  Card( NONE,  6 ),        // 0x36 '6'
  Card( NONE,  7 ),        // 0x37 '7'
  Card( NONE,  8 ),        // 0x38 '8'
  Card( NONE,  9 ),        // 0x39 '9'
  Card( NONE,  0 ),        // 0x3A ':'
  Card( NONE,  0 ),        // 0x3B ';'
  Card( NONE,  0 ),        // 0x3C '<'
  Card( NONE,  0 ),        // 0x3D '='
  Card( NONE,  0 ),        // 0x3E '>'
  Card( NONE,  0 ),        // 0x3F '?'
  Card( NONE,  0 ),        // 0x40 '@'
  Card( NONE,  0 ),        // 0x41 'A'
  Card( BLUE,  0 ),        // 0x42 'B'
  Card( NONE,  0 ),        // 0x43 'C'
  Card( NONE,  0 ),        // 0x44 'D'
  Card( NONE,  0 ),        // 0x45 'E'
  Card( NONE,  0 ),        // 0x46 'F'
  Card( GREEN, 0 ),        // 0x47 'G'
  Card( NONE,  0 ),        // 0x48 'H'
  Card( NONE,  0 ),        // 0x49 'I'
  Card( NONE,  0 ),        // 0x4A 'J'
  Card( NONE,  KICKER ),   // 0x4B 'K'
  Card( NONE,  0 ),        // 0x4C 'L'
  Card( NONE,  0 ),        // 0x4D 'M'
  Card( NONE,  0 ),        // 0x4E 'N'
  Card( NONE,  0 ),        // 0x4F 'O'
  Card( NONE,  0 ),        // 0x50 'P'
  Card( NONE,  0 ),        // 0x51 'Q'
  Card( RED,   0 ),        // 0x52 'R'
  Card( NONE,  0 ),        // 0x53 'S'
  Card( NONE, 10 ),        // 0x54 'T'
  Card( NONE,  0 ),        // 0x55 'U'
  Card( NONE,  0 ),        // 0x56 'V'
  Card( WHITE, 0 ),        // 0x57 'W'
  Card( NONE,  0 ),        // 0x58 'X'
  Card( YELLOW,0 ),        // 0x59 'Y'
  Card( NONE,  0 ),        // 0x5A 'Z'
  Card( NONE,  0 ),        // 0x5B '['
  Card( NONE,  0 ),        // 0x5C '\'
  Card( NONE,  0 ),        // 0x5D ']'
  Card( NONE,  0 ),        // 0x5E '^'
  Card( NONE,  0 ),        // 0x5F '_'
  Card( NONE,  0 ),        // 0x60 '`'
  Card( NONE,  0 ),        // 0x61 'a'
  Card( BLUE,  0 ),        // 0x62 'b'
  Card( NONE,  0 ),        // 0x63 'c'
  Card( NONE,  0 ),        // 0x64 'd'
  Card( NONE,  0 ),        // 0x65 'e'
  Card( NONE,  0 ),        // 0x66 'f'
  Card( GREEN, 0 ),        // 0x67 'g'
  Card( NONE,  0 ),        // 0x68 'h'
  Card( NONE,  0 ),        // 0x69 'i'
  Card( NONE,  0 ),        // 0x6A 'j'
  Card( NONE,  KICKER ),   // 0x6B 'k'
  Card( NONE,  0 ),        // 0x6C 'l'
  Card( NONE,  0 ),        // 0x6D 'm'
  Card( NONE,  0 ),        // 0x6E 'n'
  Card( NONE,  0 ),        // 0x6F 'o'
  Card( NONE,  0 ),        // 0x70 'p'
  Card( NONE,  0 ),        // 0x71 'q'
  Card( RED,   0 ),        // 0x72 'r'
  Card( NONE,  0 ),        // 0x73 's'
  Card( NONE, 10 ),        // 0x74 't'
  Card( NONE,  0 ),        // 0x75 'u'
  Card( NONE,  0 ),        // 0x76 'v'
  Card( WHITE, 0 ),        // 0x77 'w'
  Card( NONE,  0 ),        // 0x78 'x'
  Card( YELLOW,0 ),        // 0x79 'y'
  Card( NONE,  0 ),        // 0x7A 'z'
};

const size_t GameConstants::cardcodes_s = 0x30;
const size_t GameConstants::cardcodes_e = 0x30 + DIM(GameConstants::cardcodes);

const CardInfo GameConstants::deckinfo[] = 
{
  CardInfo(Card(RED,    KICKER), 3),    // 0 
  CardInfo(Card(RED,    2     ), 2),    // 1 
  CardInfo(Card(RED,    3     ), 2),    // 2 
  CardInfo(Card(RED,    4     ), 2),    // 3 
  CardInfo(Card(RED,    5     ), 1),    // 4 
  CardInfo(Card(RED,    6     ), 1),    // 5 
  CardInfo(Card(RED,    7     ), 1),    // 6 
  CardInfo(Card(RED,    8     ), 1),    // 7 
  CardInfo(Card(RED,    9     ), 1),    // 8 
  CardInfo(Card(RED,    10    ), 1),    // 9 
  CardInfo(Card(YELLOW, KICKER), 3),    // 10 
  CardInfo(Card(YELLOW, 2     ), 2),    // 11 
  CardInfo(Card(YELLOW, 3     ), 2),    // 12 
  CardInfo(Card(YELLOW, 4     ), 2),    // 13 
  CardInfo(Card(YELLOW, 5     ), 1),    // 14 
  CardInfo(Card(YELLOW, 6     ), 1),    // 15 
  CardInfo(Card(YELLOW, 7     ), 1),    // 16 
  CardInfo(Card(YELLOW, 8     ), 1),    // 17 
  CardInfo(Card(YELLOW, 9     ), 1),    // 18 
  CardInfo(Card(YELLOW, 10    ), 1),    // 19 
  CardInfo(Card(GREEN,  KICKER), 3),    // 20 
  CardInfo(Card(GREEN,  2     ), 2),    // 21 
  CardInfo(Card(GREEN,  3     ), 2),    // 22 
  CardInfo(Card(GREEN,  4     ), 2),    // 23 
  CardInfo(Card(GREEN,  5     ), 1),    // 24 
  CardInfo(Card(GREEN,  6     ), 1),    // 25 
  CardInfo(Card(GREEN,  7     ), 1),    // 26 
  CardInfo(Card(GREEN,  8     ), 1),    // 27 
  CardInfo(Card(GREEN,  9     ), 1),    // 28 
  CardInfo(Card(GREEN,  10    ), 1),    // 29 
  CardInfo(Card(BLUE,   KICKER), 3),    // 30 
  CardInfo(Card(BLUE,   2     ), 2),    // 31 
  CardInfo(Card(BLUE,   3     ), 2),    // 32 
  CardInfo(Card(BLUE,   4     ), 2),    // 33 
  CardInfo(Card(BLUE,   5     ), 1),    // 34 
  CardInfo(Card(BLUE,   6     ), 1),    // 35 
  CardInfo(Card(BLUE,   7     ), 1),    // 36 
  CardInfo(Card(BLUE,   8     ), 1),    // 37 
  CardInfo(Card(BLUE,   9     ), 1),    // 38 
  CardInfo(Card(BLUE,   10    ), 1),    // 39 
  CardInfo(Card(WHITE,  KICKER), 3),    // 40 
  CardInfo(Card(WHITE,  2     ), 2),    // 41 
  CardInfo(Card(WHITE,  3     ), 2),    // 42 
  CardInfo(Card(WHITE,  4     ), 2),    // 43 
  CardInfo(Card(WHITE,  5     ), 1),    // 44 
  CardInfo(Card(WHITE,  6     ), 1),    // 45 
  CardInfo(Card(WHITE,  7     ), 1),    // 46 
  CardInfo(Card(WHITE,  8     ), 1),    // 47 
  CardInfo(Card(WHITE,  9     ), 1),    // 48 
  CardInfo(Card(WHITE,  10    ), 1),    // 49 
};

const size_t GameConstants::deckinfo_size = DIM(GameConstants::deckinfo);
