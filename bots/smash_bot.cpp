#include "smash_bot.h"
#include <vector>
using namespace std;

void smash_bot::init(int Nrows, int Ncols, int Npaddles_per_player,
			  int paddle_size, bool am_i_left)
{
  R = Nrows; C = Ncols; PPP = Npaddles_per_player; PS = paddle_size; LEFT = am_i_left;
  W = C / (4*PPP-1);
  FAST = -1;
  bot_name = "SMASH_BOT";
}

pair<int,int> smash_bot::move(MovingObject me,
			     MovingObject opponent,
			     vector<MovingObject> balls,
			     int timer_offset)
{
  pair<int,int> current_vel = make_pair(me.row_vel, me.col_vel);
  if (LEFT) {
    if (me.col == 0 && FAST==0) { FAST=1; return make_pair(0, 100); }
    if (FAST==-1 || (me.col >= W-1 && FAST==1)) { FAST=0; return make_pair(0, -40); }
    return current_vel;
  }
  if (FAST==-1 || (me.col == 0 && FAST==1)) { FAST=0; return make_pair(0, 40); }
  if (me.col >= W-1 && FAST==0) { FAST=1; return make_pair(0, -100); }
  return current_vel;
}
