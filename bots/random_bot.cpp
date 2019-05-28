#include "random_bot.h"
#include <cstdlib>
#include <vector>
using namespace std;

void random_bot::init(int Nrows, int Ncols, int Npaddles_per_player,
			  int paddle_size, bool am_i_left)
{
  elapsed = 0;
  bot_name = "RANDOM_BOT";
}

pair<int,int> random_bot::move(MovingObject me,
				   MovingObject opponent,
				   vector<MovingObject> balls,
				   int timer_offset)
{
  elapsed += timer_offset;
  // change velocity once per second
  if (elapsed > 1000) {
    elapsed -= 1000;
    return make_pair(rand()%100-50, rand()%100-50);
  }
  pair<int,int> current_vel = make_pair(me.row_vel, me.col_vel);
  return current_vel;
}
