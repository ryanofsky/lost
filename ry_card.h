#ifndef ry_card_h
#define ry_card_h

// Card class
class Card
{
public:  
  
  // set to GameConstants:: RED, YELLOW, GREEN, BLUE, WHITE, or NONE
  char color;
  
  // integer number of points, or GameConstants::KICKER
  char points;
  
  // default constructor
  Card();
  
  // constructor with arguments
  Card(char color, char points);

  // Set card c based on a two letter card code
  static bool Parse(char c1, char c2, Card & c);

  // Return the card's index in the GameConstants::cardInfo array
  int getIndex() const;

  // Print a description of the card to the standard error output
  void describe() const;
  
  // convenience method. if this->color is NONE returns false, else true
  operator bool() const;
};

#endif
