#include <iostream>
#include "ry_game.h"

using std::endl;
using std::cerr;

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " gamestate" << endl << endl;
    return 1;
  }  
  
  const char * c = "hand:3W;4W;9B;2R;TR;KG;5G;KY;7Y;TY;\ncampaign-home-red:KR;4R;5R;9R\ncampaign-home-yellow:\ncampaign-home-green:KG;\ncampaign-home-blue:\ncampaign-home-white:KW;2W;4W\ncampaign-away-red:\ncampaign-away-yellow:\ncampaign-away-green:3G;4G;7G;8G;\ncampaign-away-blue:KB;2B;3B;4B;6B;TB;\ncampaign-away-white:KW;3W;\ndiscard-red:3R;KR;\ndiscard-yellow:9Y;KY;2Y;\ndiscard-green:\ndiscard-blue:\ndiscard-white:2W;\nlastturn-home:campaign-home-green;KG;drawpile;7Y;\nlastturn-away:campaign-away-white;3W;discard-red;KR;\nlastturn-homeP:pass;9B;5G;\nlastturn-awayP:discard-white;2W;drawpile;\ndrawpile:17;";
  
  Game g;
  g.parse(c);
  g.describe();
  
}