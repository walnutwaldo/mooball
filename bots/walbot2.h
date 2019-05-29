#include "bot.h"

#define MO MovingObject

#include <vector>

using namespace std;

struct walbot2 : public Bot
{
	int r, c, ppp, ps;	
	int strip_width, dis_bw_paddles;
	bool amleft;


	void init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left);

	double paddleHits(int, int);
	bool should_smash(MO, vector<MO>, int);

	vector<pair<double, pair<int, double>>> getHitLocs(MO, vector<MO>, int);
	int getTargetRow(MO, vector<pair<double, pair<int, double>>>);

	pair<int,int> move(MO me, MO opponent, vector<MO> balls, int timer_offset);
};


