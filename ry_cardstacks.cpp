/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_cardstacks.cpp                     *
* Card Stack and Stack Skew Classes     *
****************************************/

#include "ry_cardstacks.h"
#include "ry_help.h"

#include <assert.h>
#include<math.h>
#include <algorithm>

using std::min;

void CardStacks::clear()
{
  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {
    for(int stack = 0; stack < NUM_STACKS; ++stack)
      dist[stack][cardno] = 0.0;
  }  

  for(int color = 0; color < GC.NUM_COLORS; ++color)
  {
    discardSize[color] = 0;
    for(int team = 0; team < GC.NUM_TEAMS; ++team)
      campaignmin[team][color] = GC.NONE;
  }  
}

void CardStacks::addCard(int stack, int card)
{  
  dist[stack][card] += 1.0;
  ++stackisum[stack];
  if (stack == CAMPAIGNS + GC.HOMETEAM || stack == CAMPAIGNS + GC.AWAYTEAM)
  {
    Card c = GC.cardInfo[card].card;
    int team = stack - CAMPAIGNS;
    if (campaignmin[team][c.color] < c.points)
      campaignmin[team][c.color] = c.points;
  }
  else if (stack == DISCARD)
  {
    assert (GameConstants::NUM_CARDS % GameConstants::NUM_COLORS == 0);
    int ncards = GameConstants::NUM_CARDS / GameConstants::NUM_COLORS;
    int color = GC.cardInfo[card].card.color;
    assert (discardSize[color] >= 0 && discardSize[color] < ncards);
    int pos = ncards * color + discardSize[color];
    discardOrder[pos] = card;
    ++discardSize[color];    
  }
}

void CardStacks::setNumCards(int stack, int num)
{
  stackisum[stack] = num;
};

void CardStacks::normalize(const IsKnown isknown)
{
  // total number of cards in the unknown stacks
  float totalcards = 0;

  for(int stack = 0; stack < NUM_STACKS; ++stack)
  {
    stacksum[stack] = 0;
    if (isknown[stack])
      stackisum[stack] = 0;
    else
      totalcards += stackisum[stack];
  }
  
  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {    
    // actual sum of the distributions for this card
    float sum = 0;
    
    // ideal sum of the distributions for this card
    float isum = GC.cardInfo[cardno].freq;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (isknown[stack])
      sum += dist[stack][cardno];
    
    // missing sum
    float msum = isum - sum;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    {
      // divide up the missing sum between the unknown stacks
      if (!isknown[stack])
        dist[stack][cardno] = msum * stackisum[stack] / totalcards;
      stacksum[stack] += dist[stack][cardno];
    }  
  }
  
  for(int stack = 0; stack < NUM_STACKS; ++stack)
  if (isknown[stack])
    stackisum[stack] = stacksum[stack];

}

void CardStacks::applySkew(StackSkew & ss, IsKnown isknown)
{
  // arbitrary growth factor, the higher this number, the
  // more skew will be applied for a given skew factor
  const int ARBITRARY = 2;

  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {
    // actual sum of the distribution of this card (for unknown stacks)
    float sum = 0;
    
    // ideal sum of the distribution of this card (for unknown stacks)
    float isum = GC.cardInfo[cardno].freq;
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (isknown[stack])
      isum -= dist[stack][cardno];
   
    // original distribution before skew is applied
    float old[NUM_STACKS];
   
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (!isknown[stack])
    {
      float skew = ss.skew[stack][cardno];
      float & val = dist[stack][cardno];
      
      old[stack] = val;
      
      if (skew < 0)
        // val approaches 0 for large skew values
        val = val * pow(ARBITRARY, skew);      
      else if (skew > 0)
        // val approaches isum for large skew values
        val = isum + (val-isum) * pow(ARBITRARY, -skew);
      
      sum += val;
    }
    
    // normalization multiplier
    float norm = fequals(sum,(float)0.0) ? 1.123 :isum / sum;
    assert(norm >= 0.0);
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (!isknown[stack])
    {
      float & val = dist[stack][cardno];
      val *= norm;
      stacksum[stack] += val - old[stack];
    }  
  }  
}

void CardStacks::passCard(int cardno, int source, int dest, bool fractional)
{
  assert(source != dest);
  stackisum[source] -= 1.0;;
  ++stackisum[dest] += 1.0;
  float moveamt;
  if (cardno != GC.NONE)
  {
    moveamt = 0;
    int color = GC.cardInfo[cardno].card.color;
    int c = cardno;
    int m = Card(color, GC.KICKER).getIndex();
    do
    {
      float maxmove = 
        min
        (
          (float) 1.0 - moveamt,
          dist[source][c]
        );
    
      dist[source][cardno] -= maxmove;
      dist[dest][cardno] += maxmove;
      moveamt += maxmove;
      --c;
      
    } 
    while(fractional && moveamt < 1.0 && c >= m); 

    if (dest == CAMPAIGNS + GC.HOMETEAM || dest == CAMPAIGNS + GC.AWAYTEAM)
    {
      Card c = GC.cardInfo[cardno].card;
      int team = dest - CAMPAIGNS;
      if (campaignmin[team][c.color] < c.points)
        campaignmin[team][c.color] = c.points;
    }      
    
    if (source == DISCARD)
    {
      int ncards = GC.NUM_CARDS / GC.NUM_COLORS;

      assert (discardSize[color] > 0);
      --discardSize[color];
      int pos = ncards * color + discardSize[color];
      discardOrder[pos] = cardno;
    }
    else if (dest == DISCARD)
    {
      int ncards = GC.NUM_CARDS / GC.NUM_COLORS;
      assert (discardSize[color] >= 0 && discardSize[color] < ncards);
      int pos = ncards * color + discardSize[color];
      discardOrder[pos] = cardno;
      ++discardSize[color];
    }
  }
  stacksum[source] -= moveamt;
  stacksum[dest] += moveamt;  
}

float CardStacks::getnum(int stack, int card)
{
  return dist[stack][card] > 0 ? dist[stack][card] * stackisum[stack] / stacksum[stack] : 0.0;
}

int CardStacks::discardTop(int color)
{
  assert (GameConstants::NUM_CARDS % GameConstants::NUM_COLORS == 0);
  if (discardSize[color] == 0)
    return GameConstants::NONE;

  int pos = GC.NUM_CARDS / GC.NUM_COLORS * color + discardSize[color] - 1;  
  return discardOrder[pos];
}

bool CardStacks::isPlayable(int team, int color, int & cardno)
{
  int fp = campaignmin[team][color];
  if (fp == GC.NONE || fp == GC.KICKER)
  {
    cardno = Card(color,GC.KICKER).getIndex();
    return true;
  }  
  else 
  {
    cardno = Card(color,fp).getIndex() + 1;
    return fp < GC.CARD_MAX;
  }
}
  
StackSkew::StackSkew()
{
  for(int i = 0; i < CardStacks::NUM_STACKS; ++i)
  for(int j = 0; j < GC.NUM_UCARDS; ++j)
    skew[i][j] = 0;
}
