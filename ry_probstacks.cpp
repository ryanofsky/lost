#include "ry_probstacks.h"
#include "ry_help.h"

#include <assert.h>
#include<math.h>

extern const GameConstants GC;

using std::min;


void ProbStacks::clear()
{
  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {
    for(int stack = 0; stack < NUM_STACKS; ++stack)
      prob[stack][cardno] = 0.0;
  }  

  for(int color = 0; color < GC.NUM_COLORS; ++color)
  {
    discardSize[color] = 0;
    for(int team = 0; team < GC.NUM_TEAMS; ++team)
      campaignmin[team][color] = GC.KICKER;
  }  
}

void ProbStacks::addCard(int stack, int card)
{  
  prob[stack][card] += 1.0;
  ++stackisum[stack];
  if (stack == GC.CAMPAIGN + GC.HOMETEAM || stack == GC.CAMPAIGN + GC.AWAYTEAM)
  {
    Card c = GC.cardInfo[card].card;
    int team = stack - GC.CAMPAIGN;
    if (campaignmin[team][c.color] < c.points)
      campaignmin[team][c.color] = c.points;
  }
  else if (stack == DISCARD)
  {
    assert (GameConstants::NUM_CARDS % GameConstants::NUM_COLORS == 0);
    int ncards = GameConstants::NUM_CARDS / GameConstants::NUM_COLORS;
    int color = GC.cardInfo[card].card.color;
    assert (discardSize[color] >= 0 && discardSize[color] < ncards);
    int pos = ncards * color + discardSize[color] - 1;
    discardOrder[pos] = card;
    ++discardSize[color];    
  }
}

  int ProbStacks::discardTop(int color)
  {
    assert (GameConstants::NUM_CARDS % GameConstants::NUM_COLORS == 0);
    if (discardSize[color] == 0)
    {
      return GameConstants::NONE;
    }
    else
    {
      return (color * (GameConstants::NUM_CARDS / GameConstants::NUM_COLORS) + discardSize[color]);    
    }
  }
  
  float ProbStacks::getProb(int stack, int card)
  {
    return prob[stack][card] * stackisum[stack] / stacksum[stack];
  }



void ProbStacks::setNumCards(int stack, int num)
{
  stackisum[stack] = num;
};

void ProbStacks::normalize(const IsKnown isknown)
{
  // total number of cards in the unknown stacks
  float totalcards = 0;
  for(int stack = 0; stack < NUM_STACKS; ++stack)
  if (!isknown[stack])
  {
    totalcards += stackisum[stack];
    stacksum[stack] = 0;
  }
  
  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {    
    // actual sum of the probabilities for this card
    float sum = 0;
    
    // ideal sum of the probabilities for this card
    float isum = GC.cardInfo[cardno].freq;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (isknown[stack])
      sum += prob[stack][cardno];
    
    // missing sum
    float msum = isum - sum;
    
    // divide up the missing sum amoung the different stacks
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (!isknown[stack])
    {
      prob[stack][cardno] = msum * stackisum[stack] / totalcards;
      stacksum[stack] += prob[stack][cardno];
    }  
  }
}

void ProbStacks::applySkew(ProbSkew & ps, IsKnown isknown)
{
  // arbitrary growth factor, the higher this number, the
  // more skew will be applied for a given skew factor
  const int ARBITRARY = 2;

  for(int stack = 0; stack < NUM_STACKS; ++stack)
    stacksum[stack] = 0;

  for(int cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {
    // actual sum of the probabilities for this card
    float sum = 0;
    
    // ideal sum of the probabilities for this card
    float isum = GC.cardInfo[cardno].freq;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    {
      float skew = ps.skew[stack][cardno];
      float orig = prob[stack][cardno];
      float & nval = prob[stack][cardno];
      if (isknown[stack])
      {
        isum -= orig;
      }
      else
      {  
        if (skew < 0)
          nval *= pow(ARBITRARY, skew);      
        else if (skew > 0)
          nval += (orig-isum) * pow(ARBITRARY, -skew);
        sum += nval;  
      }  
    }
    
    if (fequals(sum,(float)0.0)) continue;
    
    // normalization multiplier
    float norm = isum / sum;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    {
      float & nval = prob[stack][cardno];
      if (!isknown[stack]) nval *= norm;
      stacksum[stack] += nval;
    }  
  }  
}

  void ProbStacks::passCard(int cardno, int source, int dest, bool fractional)
  {
    assert(source != dest);
    --stackisum[source];
    ++stackisum[dest];
    float moveamt = 1.0;
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
            prob[source][c]
          );
      
        prob[source][cardno] -= maxmove;
        prob[dest][cardno] += maxmove;
        moveamt += maxmove;
        --c;
        
      } 
      while(fractional && moveamt < 1.0 && c >= m); 
      
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
  
  
ProbSkew::ProbSkew()
{
  for(int i = 0; i < ProbStacks::NUM_STACKS; ++i)
  for(int j = 0; j < GC.NUM_UCARDS; ++j)
    skew[i][j] = 0;
}
