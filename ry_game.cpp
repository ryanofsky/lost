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

  const int MAXhyphenS(3);

  /////////////////////////// FIND CONVENIENT NAMES FOR VARIABLES AND CONSTANTS

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
    NONE       = GameConstants::NONE
  }; 
  
  ////////////////////////////////////////////////////////////////// LOOP STATE

  Stack * currentstack(NULL);
  Turn * currentturn(NULL);

  enum { COMPLAIN, ADDCARD, SETLENGTH, SETTURN } currentmode;
  currentmode = COMPLAIN;

  int field_start(-1), lastmove;
  bool escaped(false);

  int hyphen[MAXhyphenS];
  int hyphens;

  size_t l = data.length()-1;
  char const * str = data.c_str();

  //////////////////////////////////////////////////////////////////////// LOOP

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
      lastmove = 0;
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
    if (is_hyphen && hyphens < MAXhyphenS)
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
            currentstack = &this->myhand;
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
                whichteam = whichteam == HOME || whichteam == HOMEP ? 0 : 1;
                currentstack = &campaigns[whichteam][whichcolor];
                currentmode = ADDCARD;
              }
            }  
          break;
          case DISCARD:
            if (hyphens == 1)
            {
              if (map_get(GC.colors, string_slice(data, hyphen[0]+1, i),whichcolor))
              {
                currentstack = &discards[whichcolor];
                currentmode = ADDCARD;
              }
            }
          break;
          case HANDSIZE:
            if (hyphens == 1)
            {
              if (map_get(GC.players, string_slice(data, hyphen[0]+1, i),whichplayer))
              {
                currentstack = &hands[whichplayer];
                currentmode = SETLENGTH;
              }
            }
          break;
          case DRAWPILE:
            if (hyphens == 0)
            {
              currentstack = &drawpile;
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
              }
            }
          break;
        }
        if (currentmode == COMPLAIN) errormsg = "Unexpected";
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
              currentstack->addCard(c);
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
              currentstack->setNumCards(atoi(&str[field_start]));
            else
              errormsg = "Expected integer, not";
          break;
          case SETTURN:
            havecard = i - field_start == 2 && Card::Parse(str[field_start], str[field_start+1],c);
            fend = hyphens > 0 ? hyphen[0] : i;
            
            whichmove;
            if (!map_get(GC.moves, string_slice(data, field_start, fend),whichmove))
              whichmove = NONE;

            nomove = currentturn->move & ~RANDOMDRAW == 0;

            switch(lastmove)
            {
              PLAY:
                if (havecard)
                {
                  if (nomove)
                  {
                    currentturn->move |= PLAY;
                    currentturn->card1 = c;
                    lastmove = NONE;
                  }
                  else
                    errormsg = "Unexpected";
                }
                else if (whichmove != NONE)
                {
                  errormsg = "Campaign move must be followed by a card. Error near";
                  lastmove = whichmove;
                }
                else
                  errormsg = "Cruft";
              break;
              DROP:
                if (nomove) // haven't moved so we are adding a card to a discard pile
                {
                  if (havecard)
                  {
                    currentturn->move = DROP;
                    currentturn->card1 = c;
                    lastmove = NONE;
                  }
                  else if (whichmove != NONE)
                    errormsg = "Discard move must be followed by a card. Error near";
                  else
                    errormsg = "Cruft";
                }
                else // moved already, so we are picking up a card from a discard pile (!nomove)
                {
                  if (havecard)
                  {
                    if (currentturn->card2.color == NONE)
                    {
                      currentturn->card2 = c;
                      currentturn->move &= ~RANDOMDRAW;
                    }
                    else
                      errormsg = "Can't pick up more than one card during a turn. Error near";
                  }
                  else if (whichmove != NONE)
                    lastmove = whichmove;
                }
              break;
              RANDOMDRAW:
                if (havecard)
                {
                  if (currentturn->card2.color == NONE)
                  {
                    currentturn->move |= RANDOMDRAW;
                    currentturn->card2 = c;
                  }
                  else
                    errormsg = "Can't pick up more than one card during a turn. Error near";
                }
                else if (whichmove != NONE)
                  lastmove = whichmove;
                else
                  errormsg = "Cruft";
              PASS:
                if (havecard)
                {
                  if (currentturn->card1.color == NONE)
                    currentturn->card1 = c;
                  else if (currentturn->card2.color == NONE)
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
              else if (currentturn->card2.color != NONE)
                errormsg = "Not allowed to draw from the drawpile if you chose to draw from the discard pile instead. Error near";
              else
                currentturn->move |= RANDOMDRAW;
            }
            if (lastmove == NONE) lastmove = whichmove;
          break;
        } // switch (currentmode)
      } // if (!end_head)

      if (errormsg != NULL)
        cerr << errormsg << " '" << string_slice(data,field_start,i).c_str()
             << "' at position " << field_start << endl << endl;
      
      field_start = -1;

    } // if (is_sep)
  } // for  
}
