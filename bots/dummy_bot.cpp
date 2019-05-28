#include "dummy_bot.h"
#include <vector>
using namespace std;

void dummy_bot::init(int Nrows, int Ncols, int Npaddles_per_player,
			  int paddle_size, bool am_i_left)
{
  bot_name = "DUMMY_BOT";
}

pair<int,int> dummy_bot::move(MovingObject me,
				   MovingObject opponent,
				   vector<MovingObject> balls,
				   int timer_offset)
{
  return make_pair(0,0);
}
