/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_main.cpp                           *
* Lost Cities main() method             *
****************************************/


#include <iostream>

#include "ry_game.h"
#include "ry_alphabeta.h"
#include "ry_turn.h"
#include <assert.h>

using std::endl;
using std::cerr;

// main method
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " gamestate" << endl << endl;
    return 1;
  }  
  
  //const char * c = "hand:3W;4W;9B;2R;TR;KG;5G;KY;7Y;TY;\ncampaign-home-red:KR;4R;5R;9R\ncampaign-home-yellow:\ncampaign-home-green:KG;\ncampaign-home-blue:\ncampaign-home-white:KW;2W;4W\ncampaign-away-red:\ncampaign-away-yellow:\ncampaign-away-green:3G;4G;7G;8G;\ncampaign-away-blue:KB;2B;3B;4B;6B;TB;\ncampaign-away-white:KW;3W;\ndiscard-red:3R;KR;\ndiscard-yellow:9Y;KY;2Y;\ndiscard-green:\ndiscard-blue:\ndiscard-white:2W;\nlastturn-home:campaign-home-green;KG;drawpile;7Y;\nlastturn-away:campaign-away-white;3W;discard-red;KR;\nlastturn-homeP:pass;9B;5G;\nlastturn-awayP:discard-white;2W;drawpile;\ndrawpile:17;";
  //const char * c = "hand:3W;4W;9B;2R;TR;KG;5G;KY;7Y;TY;\\ncampaign-home-red:KR;4R;5R;9R\\ncampaign-home-yellow:\\ncampaign-home-green:KG;\\ncampaign-home-blue:\\ncampaign-home-white:KW;2W;4W\\ncampaign-away-red:\\ncampaign-away-yellow:\\ncampaign-away-green:3G;4G;7G;8G;\\ncampaign-away-blue:KB;2B;3B;4B;6B;TB;\\ncampaign-away-white:KW;3W;\\ndiscard-red:3R;KR;\\ndiscard-yellow:9Y;KY;2Y;\\ndiscard-green:\\ndiscard-blue:\\ndiscard-white:2W;\\nlastturn-home:campaign-home-green;KG;drawpile;7Y;\\nlastturn-away:campaign-away-white;3W;discard-red;KR;\\nlastturn-homeP:pass;9B;5G;\\nlastturn-awayP:discard-white;2W;drawpile;\\ndrawpile:17;\\nhandsize-home:8;\\nhandsize-away:10;\\nhandsize-homeP:8;\\nhandsize-awayP:6;";
  //const char *c = "hand:TB;6R;TG;4G;5Y;4Y;2W;6G;\\ncampaign-home-red:\\ncampaign-home-yellow:\\ncampaign-home-green:\\ncampaign-home-blue:\\ncampaign-home-white:\\ncampaign-away-red:\\ncampaign-away-yellow:\\ncampaign-away-green:\\ncampaign-away-blue:\\ncampaign-away-white:\\ndiscard-red:2R;4R;7R;\\ndiscard-yellow:3Y;KY;8Y;KY;\\ndiscard-green:3G;2G;\\ndiscard-blue:7B;3B;\\ndiscard-white:6W;KW;\\nlastturn-home:discard-blue;3B;drawpile;6G;\\nlastturn-away:discard-green;2G;drawpile;\\nlastturn-homeP:discard-red;4R;drawpile;\\nlastturn-awayP:discard-re ;7R;drawpile;\\nhandsize-home:8;\\nhandsize-away:8;\\nhandsize-homeP:8;\\nhandsize-awayP:8;\\ndrawpile:30;";
  const char * c = argv[1];
  //const char * c = "hand:KY;KY;5G;6R;4B;3G;5W;KG;\\ncampaign-home-red:3R;\\ncampaign-home-yellow:2Y;4Y;5Y;\\ncampaign-home-green:\\ncampaign-home-blue:KB;8B;\\ncampaign-home-white:2W;6W;8W;\\ncampaign-away-red:KR;KR;\\ncampaign-away-yellow:3Y;\\ncampaign-away-green:\\ncampaign-away-blue:2B;7B;\\ncampaign-away-white:KW;3W;\\ndiscard-red:2R;TR;3R;KR;\\ndiscard-yellow:2Y;\\ndiscard-green:KG;2G;6G;3G;\\ndiscard-blue:2B;6B;3B;KB;\\ndiscard-white:\\nlastturn-home:campaign-home-blue;8B;drawpile;KG;\\nlastturn-away:campaign-away-yellow;3Y;drawpile;\\nlastturn-homeP:discard-red;KR;drawpile;\\nlastturn-awayP:discard-green;3G;drawpile;\\nhandsize-home:8;\\nhandsize-away:8;\\nhandsize-homeP:8;\\nhandsize-awayP:8;\\ndrawpile:14;";
  //const char * c = "hand:3Y;6Y;5G;9B;3G;4G;7W;8R;\\ncampaign-home-red:\\ncampaign-home-yellow:\\ncampaign-home-green:\\ncampaign-home-blue:\\ncampaign-home-white:\\ncampaign-away-red:\\ncampaign-away-yellow:\\ncampaign-away-green:\\ncampaign-away-blue:\\ncampaign-away-white:\\ndiscard-red:\\ndiscard-yellow:\\ndiscard-green:\\ndiscard-blue:\\ndiscard-white:\\nlastturn-home:null;\\nlastturn-away:null;\\nlastturn-homeP:null;\\nlastturn-awayP:null;\\nhandsize-home:8;\\nhandsize-away:8;\\nhandsize-homeP:8;\\nhandsize-awayP:8;\\ndrawpile:43;";
  
  // declare game state, g
  Game g;
  
  // load game state
  g.parse(c);

  // do some guesswork to make the game state more useful
  g.countCards();

  //  g.describe();

  // alpha beta search with depth limit of 12
  ABsearch ab(12);
  
  // do alpha beta search, put best move in g.newturn
  ab.go(g);
  
  // display the best move
  g.newturn.output();

  // that is all
  return 0;
}
