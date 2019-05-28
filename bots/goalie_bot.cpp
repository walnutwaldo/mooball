#include "goalie_bot.h"
#include <vector>
using namespace std;

void goalie_bot::init(int Nrows, int Ncols, int Npaddles_per_player,
			  int paddle_size, bool am_i_left)
{
  R = Nrows; C = Ncols; PPP = Npaddles_per_player; PS = paddle_size; LEFT = am_i_left;
  bot_name = "GOALIE_BOT";
}

pair<int,int> goalie_bot::move(MovingObject me,
				   MovingObject opponent,
				   vector<MovingObject> balls,
				   int timer_offset)
{
  int closest_ball = -1;
  for (int i=0; i<balls.size(); i++) {
    if (balls[i].col < 0 || balls[i].col >= C) continue;
    if (closest_ball == -1 || (LEFT ^ (balls[i].col > balls[closest_ball].col)))
      closest_ball = i;
  }
  if (closest_ball == -1) {
    pair<int,int> current_vel = make_pair(me.row_vel, me.col_vel);
    return current_vel;
  }
  int midpt = me.row + PS/2;
  if (midpt > balls[closest_ball].row+1) return make_pair(-100, 0);
  else if (midpt < balls[closest_ball].row-1) return make_pair(100, 0);
  else return make_pair(0,0);
}
