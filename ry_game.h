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
    ProbStack camp;
    
    // value proportional to how much potential a campaign has
    double CampaignScores[GameConstants::NUM_COLORS];
    
    // value proportional to how much potential a discard pile has
    double ScavengeScores[GameConstants::NUM_COLORS];

    hands[GameConstants::HOME].fromNormal(myhand);
    goodcampaigns = 0;
    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    {
      Normal2Prob(campaign[0][i],camp);      
      double o = camp.score();
      int first = Card(i,GameConstants::KICKER).getIndex();
      int last = Card(i,GameConstants::CARD_MAX).getIndex();
      
      // estimated number of turns it would take
      // to build up this campaign
      double num = 0;

      for(int j = first; j < last; ++j)
      {
        double d1 = hands[HOME].getprob(j);
        double d2 = hands[HOMEP].getprob(j) / 1.5;
        double d3 = camp.getprob(j);
        num += d1 + d2;
        camp.setprob(d1 + d2 + d3);
      }
      
      // The way campaigns are evaluated is to take the estimated
      // score minus the current score and then to 
      // divide by the estimated number of moves it would
      // take to reach final score.
      CampaignScores[i] = (camp.score()-o) / num;  
      if (CampaignScores[i] > 0) ++goodcampaigns;
      
      NormalStack camp1;
      NormalStack::Iterator it;
      camp1 = campaign[0][i];
      int lastscore = camp1.score();
      ScavengeScores[i] = 0;
      int j = 1;
      for (it = discards[i].cards.begin(); it != discards[i].cards.end(); ++it)
      { 
        camp1.addCard(it);
        int score = camp1.score();
        ScavengeScores[i] += ((double)(score - lastscore)/j);
        lastscore = score;
        ++j;  
      }
    };
    
    struct med // intermediate data structure used for sorting
    {
      i int;
      d double;
      med(int i_, double d_) : i(i_), d(d_) { }
    };
    
    med campaignarray[GameConstants::NUM_COLORS];
    med discardarray[GameConstants::NUM_COLORS];

    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    {
      campaignarray[i] = med(i, CampaignScores[i]);
      discardarray[i] = med(i, ScavengeScores[i]); 
    }  
      
    for(int i = 0; i < GameConstants::NUM_COLORS; ++i)
    for(int j = i; i < GameConstants::NUM_COLORS - 1; ++j)
    {
      if (campaignarray[i].d < campaignarray[j].d)
        swap(campaignarray[i], campaignarray[j]);
      if (discardarray[i].d < discardarray[j].d)
        swap(discardarray[i], discardarray[j]);        
    }
    
    int cn(0), dn(0);
    firstturnc = 0;
    while(cn < GameConstants::NUM_COLORS && dn < GameConstants::NUM_COLORS)
    {
      NormalStack::Iterator it;
      for (it = myhand.cards.begin(); it != myhand.cards.end(); ++it)
      { 
        camp1.addCard(it);
        int score = camp1.score();
        ScavengeScores[i] += ((double)(score - lastscore)/j);
        lastscore = score;
        ++j;  
      }
    }
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
  
  Turn firstturns[4];
  int firstturnc;
  
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
