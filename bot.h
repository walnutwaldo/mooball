#ifndef BOT_H
#define BOT_H

#include <cstdio>
#include <vector>

// Game board column structure (e.g., with Npaddles_per_player == 3)
// [-1-][---][-2-][---][-1-][---][-2-][---][-1-][---][-2-]
//
// [-x-] means a bank of columns within which player x can move
// [---] means a bank of empty columns
// All of these will have equal size

struct MovingObject {
  int row, col;         // in pixels, starting at (0,0)
  int row_vel, col_vel; // pixels per second (max magnitude 100 for ecah)
};

struct Bot {
  const char *bot_name;

  // Ncols will be evenly divisible by (2*Npaddles_per_player-1)
  virtual void init(int Nrows, int Ncols, int Npaddles_per_player,
		    int paddle_size, bool am_i_left) {bot_name = NULL;}

  // Returns the new row_vel, col_vel values for this bot (max magnitude for these is 100)
  // if a ball has a column index off the board, it's an old ball that's now out of play; ignore it
  // timer_offset is # of milliseconds since last call to move
  // "me" and "opponent" describe the cell that is the top pixel of each respective paddle
  virtual std::pair<int,int> move(MovingObject me,
				  MovingObject opponent,
				  std::vector<MovingObject> balls,
				  int timer_offset) { return std::make_pair(0,0); }
  Bot() {}
  virtual ~Bot() {}
};

#endif
