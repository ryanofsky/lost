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