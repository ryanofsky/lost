#ifndef ry_game_h
#define ry_game_h

#include "ry_card.h"
#include "ry_turn.h"
#include <algorithm>

using std::min;

class Game
{
public:
  
  Game * parent;
  int backedUp;

  void countCards()
  {
    
    // estimate by counting
    
    emptyProbStacks(myhand);
    
    for(int i = 1; i < GameConstants::NUM_COLORS + 1; ++i)
      emptyProbStacks(discards[i]);
    
    for(int i = 1; i < GameConstants::NUM_COLORS + 1; ++i)
    {
      emptyProbStacks(campaigns[0][i]);
      emptyProbStacks(campaigns[1][i]);
    }
    
    // estimate by looking at players last moves
    
    int players[] =
    { 
      GameConstants::HOMEP,
      GameConstants::AWAY,
      GameConstants::AWAYP
    };
    
    for(int i = 0; i < 3; ++i)
    {
      int p = players[i];
      Turn t = lastturns[p];
      if (t.move & ~GameConstants::RANDOMDRAW == GameConstants::PLAY)
      {
        // unlikely they have any lower cards of the campaign they just played
        // therefore lower probabilities of lower cards
        changeProb(hands[p], t.card1.points, false, t.card1.color, false,2);
        
        // likewise, raise slightly the probability that they have higher cards
        changeProb(hands[p], t.card1.points, true, t.card1.color, true,1.5);
      }
      else if (t.move & ~GameConstants::RANDOMDRAW == GameConstants::DROP)
      {
        // less likely they have any lower cards of the campaign they
        // just dropped
        changeProb(hands[p], t.card1.points, false, t.card1.color, false,0.5);
        
        // even less likely they have any higher cards of the campaign 
        // they just dropped
        changeProb(hands[p], t.card1.points, true, t.card1.color, false,1.0);
      }
    }
    
    Turn t = lastturns[GameConstants::HOMEP];
    if (t.move == GameConstants::PASS)
    {
      if (t.card1.color == t.card2.color)
      {
        // if I was passed two cards of the same color, my teammate
        // probably has higher cards in the same campaign
        int v = min(t.card1.points, t.card2.points);
        changeProb(hands[GameConstants::HOMEP], v, true, t.card1.color, true,2.0);
      }
    }
  }  

  int findFirstSuccessors()
  {
    return 0;  
  }

  int findSuccessors(int whoseturn)
  {
    return whoseturn;  
  }
  
  void getSuccessor(int index, Game & succ)
  {
    succ.backedUp = index;
  }

  void getSuccessor(int index, Game & succ, Turn & turn, double & pscore)
  {
    Turn t(turn);
    succ.backedUp = index;
    pscore = 0.0;
  }
  
  int eval()
  {
    return 0;
  }
  
  void parse(const string data);
  void describe();
  
public:  
  NormalStack myhand;
  ProbStack hands[GameConstants::NUM_PLAYERS];
  NormalStack discards[GameConstants::NUM_COLORS];
  NormalStack campaigns[2][GameConstants::NUM_COLORS];
  ProbStack drawpile;
  Turn lastturns[GameConstants::NUM_PLAYERS];  

private:  
  void inline emptyProbStacks(NormalStack & n)
  {
    NormalStack::iterator it;
    for (it = n.cards.begin(); it != n.cards.end(); ++it)
    { 
      drawpile.setprob(*it, 0);
      for(int i = 0; i < GameConstants::NUM_PLAYERS; ++i)
        hands[i].setprob(*it, 0);
    }
  }
  
  void inline changeProb(ProbStack & p, int startat, bool greater, int color, bool raise, double amt = 1.0)
  {
    int first, last, d;
    
    first = Card(color, startat).getIndex();
    if (greater)
      last = Card(color, GameConstants::CARD_MAX);
    else  
      last = Card(color, GameConstants::CARD_MIN) - 1;
      
    if (first < 0 || last < 0 || first == last) return;  
    d = greater ? 1 : -1;
    
    for(;;)
    {
      first += d;
      if (first == last) break;
      double q = p.getprob(first);
      if (raise)
        p.setprob(first, 1.0 - (1.0-q) / (amt + 1.0));
      else
        p.setprob(first, q / (amt + 1.0));      
    }
  }
};

#endif
