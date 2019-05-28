#include "bot.h"

struct goalie_bot : public Bot
{
  int R, C, PPP, PS, LEFT;
  void init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left);
  std::pair<int,int> move(MovingObject me, MovingObject opponent,
			  std::vector<MovingObject> balls, int timer_offset);
};


