#include "ry_game.h"

#include "ry_gameconstants.h"
#include "ry_card.h"
#include "ry_turn.h"

#include <ctype.h>
#include <stdlib.h>
#include <iostream>

using std::cerr;
using std::endl;

extern GameConstants GC;

void Game::parse(const string data)
{
  /////////////////////////////////////////////////////////////////// CONSTANTS

  enum
  { 
    HAND       = GameConstants::HAND,
    CAMPAIGN   = GameConstants::CAMPAIGN,
    DISCARD    = GameConstants::DISCARD,
    TURN       = GameConstants::TURN,
    DRAWPILE   = GameConstants::DRAWPILE,
    HANDSIZE   = GameConstants::HANDSIZE,
    HOME       = GameConstants::HOME,
    AWAY       = GameConstants::AWAY,
    HOMEP      = GameConstants::HOMEP,
    AWAYP      = GameConstants::AWAYP,
    PLAY       = GameConstants::PLAY,
    DROP       = GameConstants::DROP,
    PASS       = GameConstants::PASS,
    RANDOMDRAW = GameConstants::RANDOMDRAW,
    NONE       = GameConstants::NONE,
    S_DRAWPILE = ProbStacks::DRAWPILE,
    S_DISCARD  = ProbStacks::DISCARD,
    S_HANDS    = ProbStacks::HANDS
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
                whichteam = whichteam == HOME || whichteam == HOMEP ? HOMETEAM : AWAYTEAM;
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
