/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_game.cpp                           *
* Game representation and strategies    *
****************************************/

#include "ry_game.h"
#include "ry_gameconstants.h"
#include "ry_card.h"
#include "ry_turn.h"
#include "ry_help.h"

#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include <algorithm>

using std::cerr;
using std::endl;
using std::setw;
using std::push_heap;
using std::pop_heap;
using std::make_heap;

Game::Game() : stacks(), whoseturn(GC.HOME), parent(NULL), moves_count(0)
{
}

void Game::parse(const string data)
{
  // this method is long, long, long and complicated.
  // it is efficient, though, and fairly robust
  // I didn't put as many comments in here as in the rest of the program
  // where there is AI code. this is just a big loop.
  
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
    S_DRAWPILE  = CardStacks::DRAWPILE,
    S_DISCARD   = CardStacks::DISCARD,
    S_HANDS     = CardStacks::HANDS,
    S_CAMPAIGNS = CardStacks::CAMPAIGNS
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

        if ( map_get(GC.tokens, string_slice(data, field_start, hyphens
             ? hyphen[0] : i),token) )
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
                map_get(GC.players, string_slice(data, hyphen[0]+1, hyphen[1]),
                        whichteam)
                &&
                map_get(GC.colors, string_slice(data, hyphen[1]+1, i),
                        whichcolor)
              )
              {
                whichteam = whichteam == GC.HOME || whichteam == GC.HOMEP 
                            ? GC.HOMETEAM : GC.AWAYTEAM;
                currentstack = S_CAMPAIGNS + whichteam;
                currentmode = ADDCARD;
              }
            }  
          break;
          case DISCARD:
            if (hyphens == 1)
            {
              if (map_get(GC.colors, string_slice(data, hyphen[0]+1, i), 
                  whichcolor))
              {
                currentstack = S_DISCARD;
                currentmode = ADDCARD;
              }
            }
          break;
          case HANDSIZE:
            if (hyphens == 1)
            {
              if (map_get(GC.players, string_slice(data, hyphen[0]+1, i), 
                  whichplayer))
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
              if (map_get(GC.players, string_slice(data, hyphen[0]+1, i), 
                  whichplayer))
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
            if
            (
              i - field_start == 2
              && Card::Parse(str[field_start], str[field_start+1],c)
            )
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
            havecard = i - field_start == 2 
                       && Card::Parse(str[field_start], str[field_start+1],c);
            
            fend = hyphens > 0 ? hyphen[0] : i;
            
            if (!map_get(GC.moves, string_slice(data, field_start, fend), 
                whichmove))
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
                  errormsg = "Campaign move must be followed by a card. "
                             "Error near";
                  lastmove = whichmove;
                }
                else
                  errormsg = "Cruft";
              break;
              case DROP:
                // haven't moved so we are adding a card to a discard pile
                if (nomove) 
                {
                  if (havecard)
                  {
                    currentturn->move = DROP;
                    currentturn->card1 = c;
                    lastmove = 0;
                  }
                  else if (whichmove != 0)
                    errormsg = "Discard move must be followed by a card. "
                               "Error near";
                  else
                    errormsg = "Cruft";
                }
                else // !nomove, 
                {
                  // moved already, so we are picking up a card
                  // from a discard pile 
                  if (havecard)
                  {
                    if (!currentturn->card2)
                    {
                      currentturn->card2 = c;
                      currentturn->move &= ~RANDOMDRAW;
                    }
                    else
                      errormsg = "Can't pick up more than one card during a "
                                 "turn. Error near";
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
                    errormsg = "Can't pick up more than one card during a turn." 
                               " Error near";
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
                    errormsg = "Not allowed to pass more than two cards. "
                               "Error near";
                }
              break;
            } // switch(lastmove)

            if (whichmove == RANDOMDRAW)
            {
              if (currentturn->move == PASS)
                errormsg = "Not allowed to draw from the drawpile when you "
                           "are passing. Error near";
              else if (!currentturn->card2.color)
                errormsg = "Not allowed to draw from the drawpile if you chose "
                           "to draw from the discard pile instead. Error near";
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
  
  // normalize unknown stack distributions
  stacks.normalize(isknown);
}

void Game::countCards()
{
  // skew matrix filled with 0s. As we go through the previous moves,
  // we start to fill this matrix with 1s, 2s, 3s, -1s, -2s, etc...
  // positive numbers increase the likelihood that a card is in a stack
  // negative numbers decrease the likelihood
  
  StackSkew stackSkew;
  
  // give "stackSkew.skew" the shorter name "skew"
  CardStacks::DistMatrix & skew = stackSkew.skew;
 
  // give constants shorter names
  enum
  {
    S_DRAWPILE  = CardStacks::DRAWPILE,
    S_DISCARD   = CardStacks::DISCARD,
    S_CAMPAIGNS = CardStacks::CAMPAIGNS,
    S_HANDS     = CardStacks::HANDS
  };
 
  // estimate by looking at players last moves
  int players[] =
  { 
    GC.HOMEP,
    GC.AWAY,
    GC.AWAYP
  };
  
  // loop through players
  for(int i = 0; i < DIM(players); ++i)
  {
    int p = players[i];
    int team = GC.playerInfo[p].team;
    Turn turn = lastturns[p];
    int move = turn.move & ~GC.RANDOMDRAW;
    
    if (move == GC.PLAY)
    {
      // very unlikely they have any lower cards of the campaign they just
      // played, therefore lower likelihood of lower cards
      int s = Card(turn.card1.color, GC.KICKER).getIndex();
      int e = turn.card1.getIndex();
      for(int cardno = s; cardno < e; ++cardno)
        skew[S_HANDS + p][cardno] -= 5.0;
      
      // likewise, raise slightly the likelihood that they have higher cards
      int l = Card(turn.card1.color, GC.CARD_MAX).getIndex();
      for(int cardno = e+1; cardno <= l; ++cardno)
        skew[S_HANDS + p][cardno] += 0.8;
    }
    else if (move == GC.DROP)
    {
  
      // If they dropped a card, they probably don't have many others that
      // are the same color (ignore the possibility that they are bluffing)
      
      int s = Card(turn.card1.color, GC.KICKER).getIndex();
      int c = turn.card1.getIndex();
      int e;
      
      // If the card they dropped wasn't playable, it doesn't mean they
      // don't have higher cards that are playable.

      int m = stacks.campaignmin[GC.playerInfo[p].team][turn.card1.color];

      if (m == GC.NONE)
        e = Card(turn.card1.color, GC.CARD_MAX).getIndex();
      else
        e = Card(turn.card1.color, m).getIndex() - 1;

      for(int cardno = s; cardno <= e; ++cardno)
        skew[S_HANDS + p][cardno] -= (cardno == c) ? 0.0 : 1.0;
    }
    else if (move == GC.PASS && turn.card1 && turn.card2)
    {
      int moved1 = turn.card1.getIndex();
      int moved2 = turn.card2.getIndex();

      if (turn.card1.color == turn.card2.color)
      {
        // passed two cards of the same color.
        // raise the likelihood that this person has higher cards
        // of the same campaign
        
        int s = Card(turn.card1.color, GC.KICKER).getIndex();
        int e = Card(turn.card1.color, GC.CARD_MAX).getIndex();

        bool decrease = true;   
        for(int c = s; c <= e; ++c)
        {                       
          if (c == moved1 || c == moved2)
            decrease = false;   
          else if (decrease)        
            skew[S_HANDS + p][c] -= 0.5;
          else
            skew[S_HANDS + p][c] += 1.0;
        }
      }
      else
      {
        // passed two random cards, so slightly lower the likelihood that
        // partner has other cards of these colors
        
        int s1 = Card(turn.card1.color, GC.KICKER).getIndex();
        int e1 = Card(turn.card1.color, GC.CARD_MAX).getIndex();
        int s2 = Card(turn.card2.color, GC.KICKER).getIndex();
        for(int c1 = s1; c1 <= e1; ++c1)
        {
          int c2 = c1+s2-s1;
          if (c1 != moved1)
            skew[S_HANDS + p][c1] -= 0.5;
          if (c2 != moved2)  
            skew[S_HANDS + p][c2] -= 0.5;
        }
      }
    }
  }
      
  stacks.applySkew(stackSkew,isknown);
}  

void Game::describe()
{
  // just print out the values of many variables here
  
  char const * stacklabels[] = 
  {
    "DRAW", // DRAWPILE
    "DISC", // DISCARD
    "HCAM", // CAMPAIGNS + HOMETEAM
    "ACAM", // CAMPAIGNS + AWAYTEAM
    "ME",   // HANDS + HOME
    "OPP",  // HANDS + AWAY
    "PART", // HANDS + HOMEP
    "OPRT"  // HANDS + AWAYP
  };

  cerr << "Fuzzy Distributions of cards in each stack (x100)\n\n";
  
  cerr << "Card  ";
  
  for(int col = 0; col < DIM(stacklabels); ++col)
    cerr << setw(6) << stacklabels[col];
  cerr << endl;
  
  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {
    Card card = GC.cardInfo[cardno].card;
    cerr << GC.pointSymbol(card.points) << GC.colorInfo[card.color].symbol
         << "   ";
    for(int col = 0; col < DIM(stacklabels); ++col)
      cerr << setw(6) << (int)(stacks.dist[col][cardno] * 100.0);
    cerr << endl;
  }
  cerr << "Isum";
  for(int col = 0; col < DIM(stacklabels); ++col)
    cerr << setw(6) << (int)(stacks.stackisum[col]*100);
  cerr << endl;

  cerr << "Sum ";
  for(int col = 0; col < DIM(stacklabels); ++col)
    cerr << setw(6) << (int)(stacks.stacksum[col]*100);
  cerr << endl << endl;

  cerr << "Minimum playable cards: " << endl << "  HOME  AWAY" << endl;
  
  
  for(int color = 0; color < GC.NUM_COLORS; ++color)  
  {
    int cardno;
    for(int i=0; i < 2; ++i)
    {
      cerr << "   ";
      if(stacks.isPlayable(i, color, cardno))
      {
        Card card = GC.cardInfo[cardno].card;
        cerr << GC.pointSymbol(card.points) << GC.colorInfo[card.color].symbol;
      }  
      else
        cerr << "--";
    }
    cerr << endl;
  }
  cerr << endl;
  
  cerr << "Last Turns:"<< endl << endl;  
  
  for(int i = 0; i < GC.NUM_PLAYERS; ++i)
  {
    cerr << GC.playerInfo[i].possesive << " last move: ";
    lastturns[i].describe();
    cerr << endl << endl;
  }
}

void Game::moves_push(Turn t)
{
  assert(moves_count < DIM(moves));
  moves[moves_count] = t;
  ++moves_count;
  push_heap(moves,moves+moves_count);
}

Turn Game::moves_pop()
{
  pop_heap(moves,moves+moves_count);
  --moves_count;
  return moves[moves_count];
}

int Game::moves_size()
{
  return moves_count;
}

void Game::findSuccessors()
{
  // give shorter names to CardStacks constants
  enum
  {
    S_DRAWPILE = CardStacks::DRAWPILE,
    S_DISCARD  = CardStacks::DISCARD,
    S_CAMPAIGNS = CardStacks::CAMPAIGNS,
    S_HANDS    = CardStacks::HANDS
  };

  // p prefix stands for "potential", these arrays are used to calculate
  // what each campaigns *could* be worth if players played every card
  // in their hands

  float ppoints[GC.NUM_TEAMS][GC.NUM_COLORS];
  float pinvestments[GC.NUM_TEAMS][GC.NUM_COLORS];
  float pcounts[GC.NUM_TEAMS][GC.NUM_COLORS];

  // stores the team number of the current player
  int whoseteam = GC.playerInfo[whoseturn].team;
  
  // stores the opposing team number
  int oteam = whoseteam == GC.HOMETEAM ? GC.AWAYTEAM : GC.HOMETEAM;
  
  // a conservative guess at how many turns each player has left
  float turnsleft = stacks.stackisum[S_DRAWPILE] / (float)GC.NUM_PLAYERS;

  // *potential* value of each campaign to the current player
  // used to determine which pile to draw from at the end of
  // the turn
  float playeval[GC.NUM_COLORS];

  // get current values for this->points, investments, counts
  this->examineCampaigns();

  // cycle through colors
  for(int color = 0; color < GC.NUM_COLORS; ++color)
  {
    // kicker index in current color
    int kick  = Card(color,GC.KICKER).getIndex();

    // last card index in current color
    int last  = Card(color,GC.CARD_MAX).getIndex(); 

    // set potential scores = current scores
    for(int team = 0; team < GC.NUM_TEAMS; ++team)
    {
      ppoints[team][color]      = points[team][color];
      pinvestments[team][color] = investments[team][color];
      pcounts[team][color]      = counts[team][color];
    }
    
    // lowest playable card in current players hand in current color
    // takes into account the number of turns left
    int lowcard = GC.NONE;

    // lowest playable card for current team
    int lowestcard = GC.NONE;

    // lowest playable card for the opposing team
    int olowestcard = GC.NONE;
    
    // add to potential scores looking at each players hand
    for(int player = 0; player < GC.NUM_PLAYERS; ++player)
    {
      int team = GC.playerInfo[player].team;

      int firstindex;
      if (stacks.isPlayable(team,color,firstindex))
      {
        // shorter names
        float & p = ppoints[team][color];
        float & i = pinvestments[team][color];
        float & c = pcounts[team][color];
      
        float f = stacks.getnum(S_CAMPAIGNS + team, kick);
        i += f;
        c += f; 
  
        float sc = c; // starting c value

        for(int cardno = last; cardno >= firstindex; --cardno)
        {
          if (c - sc >= turnsleft) break;
          float dist = stacks.getnum(S_HANDS + player, cardno);
          if (dist > 1.0 && cardno != kick)
            dist = 1.0;
          float cardpoints = GC.cardInfo[cardno].card.points;
          p += dist * cardpoints;
          c += dist;
          if (player == whoseturn)
            lowcard = cardno;
        }
        
        if (team == whoseteam)
          lowestcard = firstindex;
        else
          olowestcard = firstindex;        
      }
    }

    // potential campaign value for current color and current player
    float pscore = getScore(ppoints[whoseteam][color], 
                            pinvestments[whoseteam][color],
                            pcounts[whoseteam][color]);
    
    // current campaign value for current color and current player
    float cscore = getScore(points[whoseteam][color], 
                            investments[whoseteam][color],
                            counts[whoseteam][color]);            

    // opponent's potential campaign value
    float epscore = getScore(ppoints[oteam][color],
                             pinvestments[oteam][color],
                             pcounts[oteam][color]);

    // opponents's current campaign value
    float ecscore = getScore(points[oteam][color],
                             investments[oteam][color],
                             counts[oteam][color]);        

    // Find a card to discard. (chooses the lowest possible one)
    float found = 0;
    int dropcard = GC.NONE;
    for(int cardno = kick; cardno <= last; ++cardno)
    {
      found += stacks.getnum(S_HANDS + whoseturn, cardno);
      if (found >= 1.0)
      {
        dropcard = cardno;
        break;
      }  
    };
    
    // evaluate the drop card and add it to the priority queue
    if (dropcard != GC.NONE)
    {
      int droppoints = GC.cardInfo[dropcard].card.points;
      Turn turn;
      turn.move = GC.DROP;
      turn.card1 = Card(color,droppoints);
      
      // if the drop card is not playable by opponent
      if (olowestcard != GC.NONE && dropcard < olowestcard) 
        turn.eval = 0;
      else  
        turn.eval = ((cscore - pscore) + (ecscore - epscore)) / 2;
      moves_push(turn);
    }    
    
    playeval[color] = -100000000;
   
    // if a play is possible
    if (lowcard != GC.NONE)
    {
      // Find a card to play, or two cards to pass
      int playcard[3];
      int playcount = 0;

      found = 0.0;
      for(int cardno = lowcard; cardno <= last; ++cardno)
      {
        found += stacks.getnum(S_HANDS + whoseturn, cardno);
        if (found >= 1.0)
        {
          if (cardno == kick)
          {
            playcard[0] = playcard[1] = playcard[2] = cardno;
            playcount = (int)floor(found);
            found = 0.0;
          }
          else
          { 
            playcard[playcount] = cardno;
            if (++playcount >= 3) break; else found = 0;
          }  
        }  
      }
   
      // evaluate the play (or pass). big potential increase in score is good,
      // big gap between cards is bad
      playeval[color] = pscore - cscore;
      float gapsize = playcard[0] - lowestcard;
      if (playeval[color] > 0)
        playeval[color] *= 2 / (2 + gapsize);
    
      // add the play card(s) to the priority queue
      if (playcount >= 1)
      {
        Turn turn;
      
        if (playcount >= 3 && stacks.stackisum[S_HANDS + whoseturn] > 7)
        {
          turn.move = GC.PASS;
          turn.card1 = GC.cardInfo[playcard[0]].card;
          turn.card2 = GC.cardInfo[playcard[2]].card;
        }

        turn.move = GC.PLAY;
        turn.card1 = Card(color,GC.cardInfo[playcard[0]].card.points);
        turn.eval = playeval[color];
        
        moves_push(turn);
      }
    }
  }; // for color

  // find the best pile to pick up from at the end of the turn
  this->pickup = Card();
  
  // find potential value of a card from the drawpile
  float value = 0; 
  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {
    Card c = GC.cardInfo[cardno].card;
    int minvalue = stacks.campaignmin[whoseteam][c.color];
    if (GC.KICKER < c.points && c.points <= minvalue)
      value += 0;
    else if (c.points == GC.KICKER)
      value += GC.CARD_MAX * playeval[c.color] 
               * stacks.getnum(S_DRAWPILE,cardno);
    else if (c.points > minvalue)
      value += c.points * playeval[c.color] * stacks.getnum(S_DRAWPILE,cardno);
  }
  value /= stacks.stackisum[S_DRAWPILE];
 
  // compare drawpile value to each of the discard piles
  for(int color=0; color<GC.NUM_COLORS; ++color)
  {

    stacks.campaignmin[whoseteam][color];
    double nvalue = 0;
    int discardtop = stacks.discardTop(color);
    if (discardtop != GC.NONE)
    {
      int points = GC.cardInfo[discardtop].card.points;

      if (points == GC.KICKER)
        nvalue = GC.CARD_MAX;
      else
        nvalue = points;
      nvalue += playeval[color];
      if (nvalue > value)
      {
        value = nvalue;
        this->pickup = GC.cardInfo[discardtop].card; 
      }
    }
  }
  if (!parent)
    moves_describe(); 
};

Game::Game(Game const & g, Turn & t)
{
  // copy relevant variables from game g, and make changes
  // based on what is done in turn t
  // also, pickup information into turn t
  
  stacks = g.stacks;
  whoseturn = (g.whoseturn + 1) % GC.NUM_PLAYERS;
  parent = &g;
  moves_count = 0;
  
  // give constants shorter name
  enum
  { 
    S_HANDS = CardStacks::HANDS,
    S_DISCARDS = CardStacks::DISCARD,
    S_DRAWPILE = CardStacks::DRAWPILE,
    S_CAMPAIGNS = CardStacks::CAMPAIGNS
  };

  // if the last move is a pass...
  if (t.move == GC.PASS)
  {
    // find the partner
    int partner =  g.whoseturn == GC.HOME  ? GC.HOMEP :
                  (g.whoseturn == GC.HOMEP ? GC.HOME  :
                  (g.whoseturn == GC.AWAY  ? GC.AWAYP : GC.AWAY));
    
    // pass the cards
    stacks.passCard(t.card1.getIndex(), S_HANDS + g.whoseturn,
                    S_HANDS + partner, true);

    stacks.passCard(t.card2.getIndex(), S_HANDS + g.whoseturn,
                    S_HANDS + partner, true);
  }
  else
  {
    if (t.move == GC.PLAY) // last move is a play
    {
      int team = GC.playerInfo[g.whoseturn].team;
      
      // put card on campaign stack
      stacks.passCard(t.card1.getIndex(), S_HANDS + g.whoseturn,
                      S_CAMPAIGNS + team, true);
    }
    else if (t.move == GC.DROP)
    {    
      // put card on discard pile
      stacks.passCard(t.card1.getIndex(), S_HANDS + g.whoseturn,
                      S_DISCARDS, true);
    }
    
    // decide whether to pick up from the draw pile or discard pile
    // can't pick up a card that was just discarded
    if (g.pickup && !(t.move == GC.DROP && g.pickup.color == t.card1.color))
    {
      int top = stacks.discardTop(g.pickup.color);
      assert(top == g.pickup.getIndex());
      t.card2 = g.pickup;
      stacks.passCard(top, S_DISCARDS, S_HANDS + g.whoseturn);
    }
    else
    {
      // draw a random card from the drawpile
      t.move |= GC.RANDOMDRAW;
      stacks.passCard(GC.NONE, S_DRAWPILE, S_HANDS + g.whoseturn);
    }  
  }
}

float Game::eval()
{
  float sum = 0;
  for(int team = 0; team < GC.NUM_TEAMS; ++team)
  for(int color = 0; color < GC.NUM_COLORS; ++color)
  {
    float mult = team == GC.HOMETEAM ? 1 : -1;
    sum += getScore(points[team][color], investments[team][color],
                    counts[team][color]) * mult;
  }
  return sum;
}  
  

bool const Game::isknown[CardStacks::NUM_STACKS] = 
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

float inline Game::getScore(float points, float investments, float num)
{
  if (fequals(num, (float)0.0)) return 0;
  float bonus = num < (float)8.0 ? 0 : 20;
  return (investments + 1) * (points - 20) + bonus;
}

void Game::examineCampaigns()
{
  enum
  {
    S_DRAWPILE = CardStacks::DRAWPILE,
    S_DISCARD  = CardStacks::DISCARD,
    S_CAMPAIGNS = CardStacks::CAMPAIGNS,
    S_HANDS    = CardStacks::HANDS
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
      
      i = c = stacks.getnum(S_CAMPAIGNS + team, kickindex);
      p = 0;
     
      for(int cardno = lastindex; cardno > kickindex; --cardno)
      {
        float dist = stacks.getnum(S_CAMPAIGNS + team, cardno);
        float cardpoints = GC.cardInfo[cardno].card.points;
        p += dist * cardpoints;
        c += dist;          
      }
    }
  }
}

void Game::moves_describe()
{
  for(int i = moves_count; i > 0; --i)
  {
    Turn t = moves[0];
    cerr << "eval = " << t.eval << " ";

    if (t.move == GC.PLAY)
    {
      cerr << "Play a ";
      t.card1.describe();
    }
    else if (t.move == GC.DROP)
    {
      cerr << "Discard a ";
      t.card1.describe();
    };
  
    if (t.move == GC.PLAY || t.move == GC.DROP)
    {
      cerr << ", pick up a ";
      if (pickup)
        pickup.describe();
      else
        cerr << "random card";
    }
    else if (t.move == GC.PASS)
    {
      cerr << "Pass a ";
      t.card1.describe();
      cerr << " and a ";
      t.card2.describe();
    }
    cerr << endl;    
    pop_heap(moves, moves + i);
  }
  make_heap(moves,moves+moves_count);
}
  