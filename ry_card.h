#ifndef ry_card_h
#define ry_card_h

class Card
{
public:  
  
  char color;
  char points;
  
  Card();
  Card(char color, char points);

  void describe() const;
  
  static bool Parse(char c1, char c2, Card & c);
  int getIndex() const;
  
  operator bool();
};

#endif