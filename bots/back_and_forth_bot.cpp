#include "back_and_forth_bot.h"
#include <cstdlib>
#include <vector>
using namespace std;

void back_forth_bot::init(int Nrows, int Ncols, int Npaddles_per_player,
			  int paddle_size, bool am_i_left)
{
  R = Nrows; C = Ncols; PPP = Npaddles_per_player; PS = paddle_size; LEFT = am_i_left;
  bot_name = "BACK_FORTH_BOT";
}

pair<int,int> back_forth_bot::move(MovingObject me,
				   MovingObject opponent,
				   vector<MovingObject> balls,
				   int timer_offset)
{
  pair<int,int> current_vel = make_pair(me.row_vel, me.col_vel);
  if (me.row_vel == 0) return make_pair(rand()%2 ? 100 : -100, 0);
  if (me.row <= 0) return make_pair(100, 0);
  if (me.row >= R - PS) return make_pair(-100, 0);
  return current_vel;
}
