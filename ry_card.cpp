#include "ry_card.h"
#include "ry_gameconstants.h"

#include <vector>
#include "ry_help.h"

using std::vector;
using std::cerr;
using std::endl;

extern const GameConstants GC;

Card::Card() :
  color(GameConstants::NONE), points(GC.NONE)
{ };

Card::Card(char color_, char points_) : 
  color(color_), points(points_)
{ };

void Card::describe() const
{
  cerr << GC.colorInfo[color].name;
  if (points == GameConstants::KICKER)
    cerr << " Investment";
  else
    cerr << " " << (int)points;
}

bool Card::Parse(char c1, char c2, Card & c)
{
  const char s = GC.CARDCHAR_S;
  const char e = GC.CARDCHAR_E;
  
  if(isbetween(s,e,c1) && isbetween(s,e,c2))
  {
    const Card cd1 = GC.cardChar[c1 - s];
    const Card cd2 = GC.cardChar[c2 - s];
    
    if (cd1.color != GC.NONE && cd2.points != 0 )
    {
      c.color = cd1.color;
      c.points = cd2.points;
      return true;
    }
    else if (cd2.color != GC.NONE && cd1.points != 0 )
    {
      c.color = cd2.color;
      c.points = cd1.points;
      return true;
    }
  }
  return false;
}

int Card::getIndex() const
{
  int p;

  if (color == GC.NONE)
    return -1;

  if(isbetween((char)GC.CARD_MIN,(char)GC.CARD_MAX,points))
    p = points - GC.CARD_MIN + 1;
  else if (points != GC.KICKER)
    p = 0;
  else  
    return -1;
    
  return p+color*(GC.CARD_MAX - GC.CARD_MIN+2);
}

Card::operator bool()
{
  return color != GC.NONE;
}



