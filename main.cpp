#include <bits/stdc++.h>
#include "moosball.h"

// See bot.h for info on the bot interface
// Your bot will be a subclass derived from the simple "bot" class defined in bot.h

// Include your bot code here
#include "bots/back_and_forth_bot.h"
#include "bots/dummy_bot.h"
#include "bots/smash_bot.h"
#include "bots/goalie_bot.h"
#include "bots/random_bot.h"
#include "bots/walbot.h"
#include "bots/walbot2.h"
#include "bots/intercept_bot.h"

int main(int argc, char *argv[])
{
	/* Set up bots here -- either add 2 bots for autonomous play or 1 bot to play against a human */
	moosball_addbot(new walbot);
	moosball_addbot(new intercept_bot);

	moosball_play(argc, argv);  
	return 0;
}




