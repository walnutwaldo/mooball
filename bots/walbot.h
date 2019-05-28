#include "bot.h"

#define MO MovingObject

#include <vector>

using namespace std;

struct walbot : public Bot
{
	int r, c, ppp, ps;	
	bool amleft;
	void init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left);

	double paddleHits(int, int);
	bool should_smash(MO, vector<MO>);

	pair<int,int> move(MO me, MO opponent, vector<MO> balls, int timer_offset);
};


