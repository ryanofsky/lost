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

  ProbStacks stacks;

  void countCards()
  {
    ProbSkew s;
    stacks.applySkew(s,isknown);
    
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

  inline getScore(float points, float investments, float num)
  {
    if (fequals(num, (float)0.0) return 0;
    float bonus = num < (float)8.0 ? 0 : 20;
    return (investments + 1) * (points - 20) + bonus;
  }
  
  float points[GC::NUM_TEAMS][GC::NUM_COLORS];
  float investments[GC::NUM_TEAMS][GC::NUM_COLORS];
  float counts[GC::NUM_TEAMS][GC::NUM_COLORS];

  void calcScores()
  {
    enum
    {
      S_DRAWPILE = ProbStacks::DRAWPILE,
      S_DISCARD  = ProbStacks::DISCARD,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS
      S_HANDS    = ProbStacks::HANDS
    };

    for(int team = 0; team < GC.NUM_TEAMS; ++team)
    {
      for(int color = 0; color < GC.NUM_COLORS; ++color)
      {
        float & p = points[team][color];
        float & i = investments[team][color];
        float & c = counts[team][color];

        int kickindex = Card(color,GC.KICKER).getIndex();
        int lastindex = Card(color,GC.CARD_MAX).getIndex();        
        
        i = c = stacks.getProb(S_CAMPAIGNS + team, kickindex);
        p = 0;
       
        for(int cardno = lastindex; cardno > kickindex; ++cardno)
        {
          float prob = getProb(S_CAMPAIGNS + team, cardno);
          float cardpoints = GC.cardInfo[cardno].card.points;
          p += prob * cardpoints;
          c += prob;          
        }
      }
    }
  }
  
  priority_queue<Turn> moves;
  int depth;
  int me;
  Card pickup;
  
  int findSuccessors(int me, int depth)
  {
    this->depth = depth;
    this->me = me;
    
    enum
    {
      S_DRAWPILE = ProbStacks::DRAWPILE,
      S_DISCARD  = ProbStacks::DISCARD,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS
      S_HANDS    = ProbStacks::HANDS
    };

    float ppoints[GC::NUM_TEAMS][GC::NUM_COLORS];
    float pinvestments[GC::NUM_TEAMS][GC::NUM_COLORS];
    float pcounts[GC::NUM_TEAMS][GC::NUM_COLORS];

    int myteam = GC.playerInfo[me].team;
    int oteam = myteam == HOMETEAM ? AWAYTEAM : HOMETEAM;
    float turnsleft = stacks.stackisum[S_DRAWPILE] / (float)GC.NUM_PLAYERS;

    // cycle through colors
    for(int color = 0; color < GC.NUM_COLORS; ++color)
    {
      
      // find potential scores
      
      for(int team = 0; team < GC.NUM_TEAMS; ++team)
      {
        ppoints[team][color]      = points[team][color];
        pinvestments[team][color] = investments[team][color];
        pcounts[team][color]      = counts[team][color];
      }
      
      int lastcard = GC.NONE;
      for(int player = 0; player < GC.NUM_PLAYERS; ++player)
      {
        int team = GC.playerInfo[player].team;
        
        float & p = ppoints[team][color];
        float & i = pinvestments[team][color];
        float & c = pcounts[team][color];
        
        int firstindex = Card(color,stacks.campaignmin[team][color]).getIndex();
        int kickindex  = Card(color,GC.KICKER).getIndex();
        int lastindex  = Card(color,GC.CARD_MAX).getIndex();        
        
        i = c = stacks.getProb(S_CAMPAIGNS + team, kickindex);
        p = 0;
       
        if (firstindex != kickindex) ++firstindex;
        
        float scount = c;
        for(int cardno = lastindex; cardno >= firstindex; ++cardno)
        {
          if (c - scount >= turnsleft) break;
          float prob = getProb(S_HANDS + player, cardno);
          if (prob > (float)1 && cardno != kickindex)
            prob = 1;
          float cardpoints = GC.cardInfo[cardno].card.points;
          p += prob * cardpoints;
          c += prob;
          if (player == me) lastcard = cardno;
        }
      }

      int firstindex = Card(color,stacks.campaignmin[myteam][color]).getIndex();
      int ofirstindex = Card(color,stacks.campaignmin[oteam][color]).getIndex();
      int kickindex  = Card(color,GC.KICKER).getIndex();
      int lastindex  = Card(color,GC.CARD_MAX).getIndex(); 

      // Find a card to discard. (Currently chooses the lowest possible one)
      float found = 0;
      int dropcard = GC.NONE;
      for(int cardno = kickindex; cardno <= lastindex; ++cardno)
      {
        found += stacks.getProb[S_HANDS + me];
        if (found >= 1)
        {
          dropcard = cardno;
          break;
        }  
      };
      
      // Find a card to play, or two cards to pass
      int playcard[3];
      int playcount = 0;
      if (lastcard != GC.NONE)
      {
        found = 0;
        for(int cardno = lastcard; cardno < lastindex; ++cardno)
        {
          found += getProb(S_HANDS + me, playcard);
          if (found >= 1)
          {
            if (cardno == kickindex)
            {
              playcard[0] = playcard[1] = playcard[2] = cardno;
              playcount = (int)found;
              found = 0;
            }
            else
            { 
              playcard[playcount++] = cardno;
              if (playcount >= 3) break; else found = 0;
            }  
          }  
        }
      }  
      
      // my potential campaign value
      float pscore = getScore(ppoints[myteam][color],pinvestments[myteam][color],
                              pcounts[myteam][color]);
      
      // my campaign value
      float cscore = getScore(points[myteam][color],investments[myteam][color],
                              counts[myteam][color]);            

      // enemies potential campaign value
      float epscore = getScore(ppoints[oteam][color],pinvestments[oteam][color],
                              pcounts[oteam][color]);

      // enemy's campaign value
      float ecscore = getScore(points[oteam][color],investments[oteam][color],
                              counts[oteam][color]);        
      
      // evaluate the play
      playeval[color] = pscore - cscore;
      float gapsize = playcard - firstindex;
      if (playeval[color] > 0)
        playeval[color] *= 2 / (2 + gapsize);
      
      // add the play card(s) to the priority queue
      if (playcount >= 1)
      {
        Turn turn;
        
        if (playcount >= 3 && stacks.stackisum[S_HANDS + me] > 7)
        {
          turn.move = GC.PASS;
          turn.card1 = playcard[0];
          turn.card2 = playcard[2];
        }
        
        turn.move = GC.PLAY;
        turn.card1 = Card(color,GC.cardInfo[playcard].points);
        turn.eval = playeval[color];
          
        moves.push(turn);
      }
      
      // evaluate the drop card and add it to the priority queue
      if (dropcard != GC.NONE)
      {
        int droppoints = GC.cardInfo[dropcard].points
        
        Turn turn;
        turn.move = GC.DROP;
        turn.card1 = Card(color,droppoints);
        
        if (kickindex < dropcard && dropcard <= ofirstindex) 
          turn.eval = 0;
        else  
          turn.eval = ((cscore - pscore) + (ecscore - epscore)) / 2;
        moves.push(turn);
      }
    }
    
    // find the best pile to pick up from at the end of the turn
    this->pickup = NONE;
    float value = 0; 
    for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
    {
      Card c = GC.cardInfo[cardno].card;
      int minvalue = campaignmin[g.me][c.color];
      if (kicker < c.points && c.points <= minvalue)
        value += 0;
      else if (c.points == kicker)
        value += GC.CARD_MAX * playeval[c.color] * stacks.getProb(S_DRAWPILE,cardno);
      else if (c.points > minvalue)
        value += c.points * playeval[c.color] * stacks.getProb(S_DRAWPILE,cardno);
    }
    value /= stacks.stackisum[S_DRAWPILE];
    
    for(int color=0; color<GC.NUM_COLORS; ++color)
    {
      campaignmin[g.me][color];
      double nvalue = 0;
      
      if (stacks.discardtop[color] != GC.NONE)
      {
        int points = GC.cardInfo[stacks.discardtop[color]].card.points;
        if (points == KICKER)
          nvalue = GC.CARD_MAX;
        else
          nvalue = points;
        nvalue *= playeval[color];
        if (nvalue > value)
        {
          value = nvalue;
          this->pickup = color; 
        }
      }
    }  
  }

  Game(Game const & g, Turn & t)
  {
    enum
    { 
      S_HANDS = ProbStacks::HANDS,
      S_DISCARDS = ProbStacks::DISCARD
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS
    };
    
    if (t.move == GC.PASS)
    {
      int partner =  g.me == GC.HOME  ? GC.HOMEP :
                    (g.me == GC.HOMEP ? GC.HOME  :
                    (g.me == GC.AWAY  ? GC.AWAYP : GC.AWAY));
      stacks.moveCard(t.card1.getIndex(), S_HANDS + g.me, S_HANDS + g.partner);
    }
    else
    {
      if (t.move == GC.PLAY)
      {
        int team = GC.playerInfo[g.me].team;
        stacks.moveCard(t.card1.getIndex(), S_HANDS + g.me, S_CAMPAIGNS + team);
      }
      else if (t.move == GC.DROP)
      {    
        stacks.moveCard(t.card1.getIndex(), S_HANDS + g.me, S_DISCARDS);
      }
      
      if (g.pickup == GC.NONE)
      {
        t.move |= GC.RANDOMDRAW;
      }
      else
      {
        t.card2 = (g.pickup, 
      }
      
    }
  }

  int eval()
  {
    float sum = 0;
    for(int team = 0; team < GC.NUM_TEAMS; ++team)
    for(int color = 0; team < GC.COLORS; ++color)
    {
      float mult = team == GC.HOMETEAM ? 1 : -1;
      sum += getScore(points[team][color], investments[team][color], counts[team][color]);
    }
    return sum;
  }  
 
  
  float points[GC::NUM_TEAMS][GC::NUM_COLORS];
  float investments[GC::NUM_TEAMS][GC::NUM_COLORS];
  float counts[GC::NUM_TEAMS][GC::NUM_COLORS];
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

  static const Probstacks::IsKnown isknown;
  
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
