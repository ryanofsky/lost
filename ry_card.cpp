#include "ry_card.h"

void Card::describe()
{
  cerr << GameConstants::GetColor(color);
  if (points == GameConstants::KICKER)
    cerr << " Investment";
  else
    cerr << " " << (int)points;
}

bool Card::Parse(char c1, char c2, Card & c)
{
  const char s = GameConstants::cardcodes_s;
  const char e = GameConstants::cardcodes_e;
  
  if(isbetween(s,e,c1) && isbetween(s,e,c2))
  {
    const Card cd1 = GameConstants::cardcodes[c1 - s];
    const Card cd2 = GameConstants::cardcodes[c2 - s];
    
    if (cd1.color != GameConstants::NONE && cd2.points != 0 )
    {
      c.color = cd1.color;
      c.points = cd2.points;
      return true;
    }
    else if (cd2.color != GameConstants::NONE && cd1.points != 0 )
    {
      c.color = cd2.color;
      c.points = cd1.points;
      return true;
    }
  }
  return false;
}

int Card::getIndex()
{
  int p;

  if (color == GameConstants::NONE)
    return -1;

  if(isbetween((char)GameConstants::CARD_MIN,(char)GameConstants::CARD_MAX,points))
    p = points - GameConstants::CARD_MIN + 1;
  else if (points != GameConstants::KICKER)
    p = 0;
  else  
    return -1;
    
  return p+color*(GameConstants::CARD_MAX-GameConstants::CARD_MIN+2);
}