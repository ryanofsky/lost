#include "ry_game.h"

#include "ry_gameconstants.h"
#include "ry_card.h"
#include "ry_turn.h"
#include "ry_help.h"

#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <math.h>


using std::cerr;
using std::endl;

extern const GameConstants GC;

void Game::parse(const string data)
{
  /////////////////////////////////////////////////////////////////// CONSTANTS

  enum
  { 
    HAND        = GameConstants::HAND,
    CAMPAIGN    = GameConstants::CAMPAIGN,
    DISCARD     = GameConstants::DISCARD,
    TURN        = GameConstants::TURN,
    DRAWPILE    = GameConstants::DRAWPILE,
    HANDSIZE    = GameConstants::HANDSIZE,
    HOME        = GameConstants::HOME,
    AWAY        = GameConstants::AWAY,
    HOMEP       = GameConstants::HOMEP,
    AWAYP       = GameConstants::AWAYP,
    PLAY        = GameConstants::PLAY,
    DROP        = GameConstants::DROP,
    PASS        = GameConstants::PASS,
    RANDOMDRAW  = GameConstants::RANDOMDRAW,
    NONE        = GameConstants::NONE,
    S_DRAWPILE  = ProbStacks::DRAWPILE,
    S_DISCARD   = ProbStacks::DISCARD,
    S_HANDS     = ProbStacks::HANDS,
    S_CAMPAIGNS = ProbStacks::CAMPAIGNS
  }; 
  
  enum { MAXHYPHENS = 3 };
  
  ////////////////////////////////////////////////////////////////// LOOP STATE

  enum { COMPLAIN, ADDCARD, SETLENGTH, SETTURN } currentmode;
  currentmode = COMPLAIN;

  int currentstack = NONE;
  Turn * currentturn = NULL;

  int field_start(-1), lastmove;
  bool escaped(false);

  int hyphen[MAXHYPHENS];
  int hyphens;

  size_t l = data.length()-1;
  char const * str = data.c_str();

  //////////////////////////////////////////////////////////////////////// LOOP

  stacks.clear(); 
  for(size_t i(0); i <= l; ++i)
  {
    char c = str[i];
    bool is_escape   = c=='\\';
    bool is_last     = i==l;
    bool is_hyphen    = c=='-';
    bool is_space    = !isbetween((char)33,(char)127,c);
    bool is_endhead  = c==':';
    bool is_endfield = c==';' || is_space;
    bool is_sep      = is_endhead || is_endfield || is_last;

    if (field_start<0 && !is_sep) // new field test
    {
      field_start = i;
      hyphens = 0;
    }

    if (escaped) // test for escape characters (only "\n" is recognized)
    {
      if (c == 'n' && (field_start == i-1)) // skip "\\n" if it begins a field
        field_start = -1;
      escaped = false;
    }
    else if (is_escape)
      escaped = true;

    // hyphen test
    if (is_hyphen && hyphens < MAXHYPHENS)
      hyphen[hyphens++] = i;

    if (is_sep && field_start >= 0) // end of field test
    {
      if (is_last && !is_endhead && !is_endfield) ++i;
      const char * errormsg = NULL;

      if (is_endhead) // end of header
      {
        currentmode = COMPLAIN;
        int token,whichteam,whichcolor,whichplayer;

        if ( map_get(GC.tokens, string_slice(data, field_start, hyphens ? hyphen[0] : i),token) )
        switch(token)
        {
          case HAND:
            currentmode = ADDCARD;
            currentstack = S_HANDS + HOME;
          break;
          case CAMPAIGN:
            if (hyphens == 2)
            {
              if
              (
                map_get(GC.players, string_slice(data, hyphen[0]+1, hyphen[1]),whichteam)
                &&
                map_get(GC.colors, string_slice(data, hyphen[1]+1, i),whichcolor)
              )
              {
                whichteam = whichteam == GC.HOME || whichteam == GC.HOMEP ? GC.HOMETEAM : GC.AWAYTEAM;
                currentstack = S_CAMPAIGNS + whichteam;
                currentmode = ADDCARD;
              }
            }  
          break;
          case DISCARD:
            if (hyphens == 1)
            {
              if (map_get(GC.colors, string_slice(data, hyphen[0]+1, i),whichcolor))
              {
                currentstack = S_DISCARD;
                currentmode = ADDCARD;
              }
            }
          break;
          case HANDSIZE:
            if (hyphens == 1)
            {
              if (map_get(GC.players, string_slice(data, hyphen[0]+1, i),whichplayer))
              {
                currentstack = S_HANDS + whichplayer;
                currentmode = SETLENGTH;
              }
            }
          break;
          case DRAWPILE:
            if (hyphens == 0)
            {
              currentstack = S_DRAWPILE;
              currentmode = SETLENGTH;
            }
          break;
          case TURN:
            if (hyphens == 1)
            {
              if (map_get(GC.players, string_slice(data, hyphen[0]+1, i),whichplayer))
              {
                currentturn = &lastturns[whichplayer];
                currentmode = SETTURN;
                lastmove = 0;
              }
            }
          break;
        }
      }
      else // end of normal field (is_sep && !is_colon)
      {
        // declare the temporary variables used in the switch statement
        Card c; 
        bool legit, havecard, nomove;
        size_t fend;
        int whichmove;
        
        switch(currentmode)
        {
          case ADDCARD:
            if (i - field_start == 2 && Card::Parse(str[field_start], str[field_start+1],c))
              stacks.addCard(currentstack, c.getIndex());
            else
              errormsg = "Unable to parse card";
          break;
          case SETLENGTH:
            legit = true;
            for(int j = field_start; j < i; ++j)
            if(!isdigit(str[j]))
            {
              legit = false;
              break;
            }
            if (legit)
              stacks.setNumCards(currentstack, atoi(&str[field_start]));
            else
              errormsg = "Expected integer, not";
          break;
          case SETTURN:
            havecard = i - field_start == 2 && Card::Parse(str[field_start], str[field_start+1],c);
            fend = hyphens > 0 ? hyphen[0] : i;
            
            if (!map_get(GC.moves, string_slice(data, field_start, fend),whichmove))
              whichmove = 0;

            nomove = ((currentturn->move & ~RANDOMDRAW) == 0);

            switch(lastmove)
            {
              case PLAY:
                if (havecard)
                {
                  if (nomove)
                  {
                    currentturn->move |= PLAY;
                    currentturn->card1 = c;
                    lastmove = 0;
                  }
                  else
                    errormsg = "Unexpected";
                }
                else if (whichmove != 0)
                {
                  errormsg = "Campaign move must be followed by a card. Error near";
                  lastmove = whichmove;
                }
                else
                  errormsg = "Cruft";
              break;
              case DROP:
                if (nomove) // haven't moved so we are adding a card to a discard pile
                {
                  if (havecard)
                  {
                    currentturn->move = DROP;
                    currentturn->card1 = c;
                    lastmove = 0;
                  }
                  else if (whichmove != 0)
                    errormsg = "Discard move must be followed by a card. Error near";
                  else
                    errormsg = "Cruft";
                }
                else // moved already, so we are picking up a card from a discard pile (!nomove)
                {
                  if (havecard)
                  {
                    if (!currentturn->card2)
                    {
                      currentturn->card2 = c;
                      currentturn->move &= ~RANDOMDRAW;
                    }
                    else
                      errormsg = "Can't pick up more than one card during a turn. Error near";
                  }
                  else if (whichmove != 0)
                    lastmove = whichmove;
                }
              break;
              case RANDOMDRAW:
                if (havecard)
                {
                  if (!currentturn->card2)
                  {
                    currentturn->move |= RANDOMDRAW;
                    currentturn->card2 = c;
                  }
                  else
                    errormsg = "Can't pick up more than one card during a turn. Error near";
                }
                else if (whichmove != 0)
                  lastmove = whichmove;
                else
                  errormsg = "Cruft";
              break;    
              case PASS:
                if (havecard)
                {
                  if (!currentturn->card1)
                  {
                    currentturn->move = PASS;
                    currentturn->card1 = c;
                  }  
                  else if (!currentturn->card2)
                    currentturn->card2 = c;
                  else
                    errormsg = "Not allowed to pass more than two cards. Error near";
                }
              break;
            } // switch(lastmove)

            if (whichmove == RANDOMDRAW)
            {
              if (currentturn->move == PASS)
                errormsg = "Not allowed to draw from the drawpile when you are passing. Error near";
              else if (!currentturn->card2.color)
                errormsg = "Not allowed to draw from the drawpile if you chose to draw from the discard pile instead. Error near";
              else
                currentturn->move |= RANDOMDRAW;
            }
            if (lastmove == 0) lastmove = whichmove;
          break;
        } // switch (currentmode)
      } // if (!end_head)

      if (currentmode == COMPLAIN) errormsg = "Cruft";
      if (errormsg != NULL)
        cerr << errormsg << " '" << string_slice(data,field_start,i).c_str()
             << "' at position " << field_start << endl << endl;
      field_start = -1;
    } // if (is_sep)
  } // for
  
  stacks.normalize(isknown);
}

void Game::describe()
{
/*
  cerr << "My Cards:  ";
  myhand.describe();
  cerr << endl << endl;
  
  for(int i = 0; i < 2; ++i)
  {
    if (i == 0)
      cerr << "My Campaigns:" << endl << endl;
    else
      cerr << "Opposing Campaigns:" << endl << endl;
  
    for(int j = 0; j < GameConstants::NUM_COLORS; ++j)
    {
      cerr << "  " << GameConstants::GetColor(j) << " ";
      cerr << "Campaign: ";
      campaigns[i][j].describe();
      cerr << endl;
    }
    cerr << endl;
    
    cerr << "Drawpile: ";
    drawpile.describe();
    cerr << endl << endl;
  }
  
  for(int i = 0; i < GameConstants::NUM_PLAYERS; ++i)
  {
    cerr << GameConstants::GetTeam(i) << " last move: ";
    lastturns[i].describe();
    cerr << endl << endl;
  }
  
  for(int i = 0; i < GameConstants::NUM_PLAYERS; ++i)
  {
    cerr << GameConstants::GetTeam(i) << " hand: ";
    hands[i].describe();
    cerr << endl << endl;
  }
*/
}

bool const Game::isknown[ProbStacks::NUM_STACKS] = 
{
  false,   // DRAWPILE
  true,    // DISCARD
  true,    // CAMPAIGNS + HOMETEAM
  true,    // CAMPAIGNS + AWAYTEAM
  true,    // HANDS + HOME
  false,   // HANDS + AWAY
  false,   // HANDS + HOMEP
  false    // HANDS + AWAYP
};

  void Game::countCards()
  {
    ProbSkew s;
    ProbStacks::ProbMatrix & skew = s.skew;
   
    enum
    {
      S_DRAWPILE  = ProbStacks::DRAWPILE,
      S_DISCARD   = ProbStacks::DISCARD,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS,
      S_HANDS     = ProbStacks::HANDS
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
        int e = turn.card1.getIndex();
        for(int cardno = s; cardno < e; ++cardno)
          skew[S_HANDS + p][cardno] -= 5.0;
        
        // likewise, raise slightly the probability that they have higher cards
        int l = Card(turn.card1.color, GC.CARD_MAX).getIndex();
        for(int cardno = e; cardno <= l; ++cardno)
          skew[S_HANDS + p][cardno] += 0.8;
      }
      else if (move == GC.DROP)
      {
        // If they dropped a card, they don't have many others of the same campaign
        
        int s = Card(turn.card1.color, GC.KICKER).getIndex();
        int e = turn.card1.getIndex();
        int l = Card(turn.card1.color, GC.KICKER).getIndex();
        for(int cardno = s; cardno <= l; ++cardno)
          skew[S_HANDS + p][cardno] -= (cardno == e) ? 2.0 : 1.0;
      }
      else if (move == GC.PASS && turn.card1 && turn.card2)
      {
        if (turn.card1.color == turn.card2.color)
        {
          // passed two cards of the same color.
          // raise the probability that this person has higher cards of the same campaign
          // lower the probability that the person has the cards that he just passed
          
          int s = Card(turn.card1.color, GC.KICKER).getIndex();
          int e = Card(turn.card1.color, GC.CARD_MAX).getIndex();
          int moved1 = turn.card1.getIndex();
          int moved2 = turn.card2.getIndex();

          bool decrease = true;   
          for(int c = s; c <= e; ++c)
          {                       
            if (c == moved1 || c == moved2)
            {                     
              skew[S_HANDS + p][c] -= 2;
              decrease = false;   
            }                     
            else if (decrease)        
              skew[S_HANDS + p][c] -= 0.5;
            else
              skew[S_HANDS + p][c] += 1.0;
          }
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

  float inline Game::getScore(float points, float investments, float num)
  {
    if (fequals(num, (float)0.0)) return 0;
    float bonus = num < (float)8.0 ? 0 : 20;
    return (investments + 1) * (points - 20) + bonus;
  }
  
  void Game::calcScores()
  {
    enum
    {
      S_DRAWPILE = ProbStacks::DRAWPILE,
      S_DISCARD  = ProbStacks::DISCARD,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS,
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
          float prob = stacks.getProb(S_CAMPAIGNS + team, cardno);
          float cardpoints = GC.cardInfo[cardno].card.points;
          p += prob * cardpoints;
          c += prob;          
        }
      }
    }
  }
  
  void Game::findSuccessors(int me, int depth)
  {
    this->depth = depth;
    this->me = me;
    
    enum
    {
      S_DRAWPILE = ProbStacks::DRAWPILE,
      S_DISCARD  = ProbStacks::DISCARD,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS,
      S_HANDS    = ProbStacks::HANDS
    };

    float ppoints[GC.NUM_TEAMS][GC.NUM_COLORS];
    float pinvestments[GC.NUM_TEAMS][GC.NUM_COLORS];
    float pcounts[GC.NUM_TEAMS][GC.NUM_COLORS];

    int myteam = GC.playerInfo[me].team;
    int oteam = myteam == GC.HOMETEAM ? GC.AWAYTEAM : GC.HOMETEAM;
    float turnsleft = stacks.stackisum[S_DRAWPILE] / (float)GC.NUM_PLAYERS;

    float playeval[GC.NUM_COLORS];

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
          float prob = stacks.getProb(S_HANDS + player, cardno);
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
        found += stacks.getProb(S_HANDS + me, cardno);
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
          found += stacks.getProb(S_HANDS + me, cardno);
          if (found >= 1)
          {
            if (cardno == kickindex)
            {
              playcard[0] = playcard[1] = playcard[2] = cardno;
              playcount = (int)floor(found);
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
      
      // evaluate the play (or pass)
      playeval[color] = pscore - cscore;
      float gapsize = playcard[0] - firstindex;
      if (playeval[color] > 0)
        playeval[color] *= 2 / (2 + gapsize);
      
      // add the play card(s) to the priority queue
      if (playcount >= 1)
      {
        Turn turn;
        
        if (playcount >= 3 && stacks.stackisum[S_HANDS + me] > 7)
        {
          turn.move = GC.PASS;
          turn.card1 = GC.cardInfo[playcard[0]].card;
          turn.card2 = GC.cardInfo[playcard[2]].card;
        }
        
        turn.move = GC.PLAY;
        turn.card1 = Card(color,GC.cardInfo[playcard[0]].card.points);
        turn.eval = playeval[color];
          
        moves.push(turn);
      }
      
      // evaluate the drop card and add it to the priority queue
      if (dropcard != GC.NONE)
      {
        int droppoints = GC.cardInfo[dropcard].card.points;
        
        Turn turn;
        turn.move = GC.DROP;
        turn.card1 = Card(color,droppoints);
        
        if (kickindex < dropcard && dropcard <= ofirstindex) 
          turn.eval = 0;
        else  
          turn.eval = ((cscore - pscore) + (ecscore - epscore)) / 2;
        moves.push(turn);
      }
    }; // for color
    
    // find the best pile to pick up from at the end of the turn
    this->pickup = Card();
    
    // find potential value of a card from the drawpile
    float value = 0; 
    for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
    {
      Card c = GC.cardInfo[cardno].card;
      int minvalue = stacks.campaignmin[myteam][c.color];
      if (GC.KICKER < c.points && c.points <= minvalue)
        value += 0;
      else if (c.points == GC.KICKER)
        value += GC.CARD_MAX * playeval[c.color] * stacks.getProb(S_DRAWPILE,cardno);
      else if (c.points > minvalue)
        value += c.points * playeval[c.color] * stacks.getProb(S_DRAWPILE,cardno);
    }
    value /= stacks.stackisum[S_DRAWPILE];
    
    // compare drawpile value to each of the discard piles
    for(int color=0; color<GC.NUM_COLORS; ++color)
    {
      stacks.campaignmin[myteam][color];
      double nvalue = 0;
      int discardtop = stacks.discardTop(color);
      if (discardtop != GC.NONE)
      {
        int points = GC.cardInfo[discardtop].card.points;
        if (points == GC.KICKER)
          nvalue = GC.CARD_MAX;
        else
          nvalue = points;
        nvalue *= playeval[color];
        if (nvalue > value)
        {
          value = nvalue;
          this->pickup = GC.cardInfo[discardtop].card; 
        }
      }
    }  
  };
  
  Game::Game(Game const & g, Turn & t)
  {
    this->parent = &g;
    
    enum
    { 
      S_HANDS = ProbStacks::HANDS,
      S_DISCARDS = ProbStacks::DISCARD,
      S_DRAWPILE = ProbStacks::DRAWPILE,
      S_CAMPAIGNS = ProbStacks::CAMPAIGNS
    };
    
    if (t.move == GC.PASS)
    {
      int partner =  g.me == GC.HOME  ? GC.HOMEP :
                    (g.me == GC.HOMEP ? GC.HOME  :
                    (g.me == GC.AWAY  ? GC.AWAYP : GC.AWAY));
      stacks.passCard(t.card1.getIndex(), S_HANDS + g.me, S_HANDS + partner);
    }
    else
    {
      if (t.move == GC.PLAY)
      {
        int team = GC.playerInfo[g.me].team;
        stacks.passCard(t.card1.getIndex(), S_HANDS + g.me, S_CAMPAIGNS + team);
      }
      else if (t.move == GC.DROP)
      {    
        stacks.passCard(t.card1.getIndex(), S_HANDS + g.me, S_DISCARDS);
      }
      
      if (t.card2)
      {
        t.move |= GC.RANDOMDRAW;
        stacks.passCard(GC.NONE, S_DRAWPILE, S_HANDS + g.me);
      }
      else // t.card2
      {
        int top = stacks.discardTop(t.card2.color);
        assert(top != GC.NONE);
        t.card2.points = GC.cardInfo[top].card.points;
        stacks.passCard(t.card2.getIndex(), S_DISCARDS, S_HANDS + g.me);
      }
    }
  }
  
  int Game::eval()
  {
    float sum = 0;
    for(int team = 0; team < GC.NUM_TEAMS; ++team)
    for(int color = 0; team < GC.NUM_COLORS; ++color)
    {
      float mult = team == GC.HOMETEAM ? 1 : -1;
      sum += getScore(points[team][color], investments[team][color], counts[team][color]);
    }
    return sum;
  }  
  