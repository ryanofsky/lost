#include "ry_probstacks.h"

extern const GameConstants GC;

void ProbStacks::clear()
{
  for(int cardno = 0; i < GC.NUM_UCARDS; ++cardno)
  {
    discardnum[cardno] = GC.NONE;
    for(int stack = 0; stack < NUM_STACKS; ++stack)
      prob[stack][cardno] = 0.0;
  }  

  for(int color = 0; color < GC.NUM_COLORS; ++color)
  {
    if (!discardq[color].empty())
      discardq[color] = priority_queue<DiscardNode>();
    for(int team = 0; team < GC.NUM_TEAMS; ++team)
      campaignmin[team][color] = GC.KICKER;
  }  
}

void ProbStacks::addCard(int stack, int card)
{  
  prob[stack][card] += 1.0;
  if (stack == CAMPAIGN + GC.HOMETEAM || stack == CAMPAIGN + GC.AWAYTEAM)
  {
    Card c = GC.cardInfo[card].card;
    int team = stack - CAMPAIGN;
    if (campaignmin[team][c.color] < c.points)
      campaignmin[team][c.color] = c.points;
  }
  else if (stack == DISCARD)
  {
    int color = GC.cardInfo[card].color;
    discardq[color].push(DiscardNode());
    
    discardtop[color] = card;
    int & ds = discardsum[color];
    discardnum[card] = ds++;
    ++ds;
  }
}

void ProbStacks::setNumCards(int stack, int num)
{
  stackisum[stack] = num;
};

void ProbStacks::normalize(const IsKnown isknown)
{
  // total number of cards in the unknown stacks
  double totalcards = 0;
  for(int stack = 0; stack < NUM_STACKS; ++stack)
  if (!isknown[stack])
  {
    totalcards += stackisum[stack];
    stacksum[stack] = 0;
  }
  
  for(cardno = 0; cardno < GC.NUM_UCARDS; ++cardno)
  {    
    // actual sum of the probabilities for this card
    double sum = 0;
    
    // ideal sum of the probabilities for this card
    double isum = GC.cardInfo(c).freq;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    if (isknown[stack])
      sum += prob[stack][cardno];
    
    // missing sum
    double msum = isum - sum;
    
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
    double sum = 0;
    
    // ideal sum of the probabilities for this card
    double isum = GC.cardInfo(c).freq;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    {
      double skew = pt.skew[stack][cardno];
      double orig = prob[stack][cardno];
      double & nval = prob[stack][cardno];
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
    
    if (fequals(sum,0)) continue;
    
    // normalization multiplier
    double norm = isum / sum;
    
    for(int stack = 0; stack < NUM_STACKS; ++stack)
    {
      double & nval = prob[stack][cardno];
      if (!isknown[stack]) nval *= norm;
      stacksum[stack] += nval;
    }  
  }  
}

ProbSkew::ProbSkew()
{
  for(int i = 0; i < ProbStacks::NUM_STACKS; ++i)
  for(int j = 0; j < GC.NUM_UCARDS; ++j)
    skew[i][j] = 0;
}
