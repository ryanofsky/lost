// real work
// TODO: finish up Game psuedo-copy constructor

// administravia
// TODO: extract methods, rebuild makefile, compile, clue alphabeta in on the new Game interface

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
    ProbStacks::ProbMatrix & skew = s.skew;
   
    enum
    {
      S_DRAWPILE = ProbStacks::DRAWPILE,
      S_DISCARD  = ProbStacks::DISCARD,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS
      S_HANDS    = ProbStacks::HANDS
    };
   
    // estimate by looking at players last moves
    
    int players[] =
    { 
      GC.HOMEP,
      GC.AWAY,
      GC.AWAYP
    };
    
    for(int i = 0; i < DIM(players); ++i)
    {
      int p = players[i];
      int team = GC.playerInfo[p].team;
      Turn turn = lastturns[p];
      int move = turn.move & ~GC.RANDOMDRAW;
      
      if (move == GC.PLAY)
      {
        // very unlikely they have any lower cards of the campaign they just
        // played, therefore lower probabilities of lower cards
        int s = Card(turn.card1.color, GC.KICKER).getIndex();
        int e = move.card1.getIndex();
        for(int cardno = s; cardno < e; ++cardno)
          skew[S_HANDS + p][cardno] -= 5.0;
        
        // likewise, raise slightly the probability that they have higher cards
        int l = Card(turn.card1.color, GC.CARD_MAX).getIndex());
        for(int cardno = e; cardno <= l; ++cardno)
          skew[S_HANDS + p][cardno] += 0.8;
      }
      else if (move == GC.DROP)
      {
        // If they dropped a card, they don't have many others of the same campaign
        
        int s = Card(turn.card1.color, GC.KICKER).getIndex();
        int e = move.card1.getIndex();
        int l = Card(turn.card1.color, GC.KICKER).getIndex();
        for(int cardno = s; cardno <= l; ++cardno)
          skew[S_HANDS + p][cardno] -= (cardno == e) ? 2.0 : 1.0;
      }
      else if (move == PASS && turn.card1 && turn.card2)
      {
        if (turn.card1.color == turn.card2.color)
        {
          // passed two cards of the same color.
          // raise the probability that this person has higher cards of the same campaign
          // lower the probability that the person has the cards theat he just passed
          
          int s1 = Card(turn.card1.color, GC.KICKER).getIndex();
          int e1 = Card(turn.card1.color, GC.CARD_MAX).getIndex();
          int moved = turn.card1.getIndex();
          int s2 = Card(turn.card2.color, GC.KICKER).getIndex();
          bool decrease = true;   
          for(int c1 = s1; c1 <= e1; ++c1)
          {                       
            if (c1 == moved || c2 == moved)
            {                     
              skew[S_HANDS + p][c1] -= 2;
              decrease = false;   
            }                     
            else                  
              skew[S_HANDS +      
             if (decrease)        
              skew[S_HANDS + p][c1] -= 0.5;
            else
              skew[S_
                
          }
        }


          // TODO: fill in this blank
        }
        else
        {
          // passed two random cards, so slightly lower the probability that
          // partner has other cards of these colors
          
          int s1 = Card(turn.card1.color, GC.KICKER).getIndex();
          int e1 = Card(turn.card1.color, GC.CARD_MAX).getIndex();
          int s2 = Card(turn.card2.color, GC.KICKER).getIndex();
          for(int c1 = s1; c1 <= e1; ++c1)
          {
            skew[S_HANDS + p][c1] -= 0.5;
            skew[S_HANDS + p][c1+s2-s1] -= 0.5;
          }
        }
      }
    }
        
    stacks.applySkew(s,isknown);
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
      
      if (g.card2.color == GC.NONE)
      {
        t.move |= GC.RANDOMDRAW;
        stacks.moveCard(GC.NONE, S_DRAWPILE, S_HAND + GC.me));
      }
      else // g.card2
      {
        t.card2.point = g.stacks.prob.discardq[t.card2.color].top()->point;
        stacks.moveCard(t.card2.getIndex(), S_DISCARD + card2.color, S_HANDS + GC.me, 
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
};

#endif
