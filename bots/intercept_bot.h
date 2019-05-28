#include <bits/stdc++.h>
using namespace std;

struct intercept_bot : public Bot
{
	int R, C, PPP, PS, LEFT, W, FAST, SZ;

	void init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left) {
		R = Nrows; C = Ncols; PPP = Npaddles_per_player; PS = paddle_size; LEFT = am_i_left;
		SZ = C / (Npaddles_per_player * 4 - 1);
		bot_name = "INTERCEPT_BOT";
	}

	// Simulates t ms of vertical movement and returns the final row coordinate
	int simulate_row(double r, double rv, double time_millis) {
		for (int t = 0; t < (int) time_millis; t++) {
			r += rv / 1000;
			if (r < 0)
				rv = abs(rv);
			if (r >= R)
				rv = -abs(rv);
		}

		r += rv * (time_millis - (int) time_millis) / 1000;
		if (r < 0) return 0;
		if (r >= R) return R - 1;
		return r;
	}

	// Returns the column of the next intersection of a moving ball left and stationary paddle,
	// or -1 if not applicable. Also returns the priority.
	pair<int, double> next_intersect(MovingObject me, MovingObject ball) {
		if (!LEFT) {
			me.col_vel *= -1;
			ball.col_vel *= -1;
			me.col = SZ - 1 - me.col;
			ball.col = C - 1 - ball.col;
		}

		if (ball.col >= C || ball.col < 0 || ball.col_vel >= 0 || ball.col < me.col)
			return { -1, -1 };

		bool goalie = (ball.col < 2 * SZ);

		while (ball.col - 4 * SZ > me.col)
			ball.col -= 4 * SZ; // normalize to first segment

		// Idea: check if opponent might hit first before you?

		double time_millis = 1000 * (me.col - ball.col) / ball.col_vel;

		// Idea: try different priorities than just 1/t
		double priority = (1.0 / (time_millis + 1.0)) * (goalie ? 5 : 1); 
		return { simulate_row(ball.row, ball.row_vel, time_millis), priority };
	}

	// Returns the column of the next block of a moving ball right and stationary paddle,
	// or -1 if not applicable. Also returns the priority.
	pair<int, double> next_block(MovingObject me, MovingObject ball) {
		if (!LEFT) {
			me.col_vel *= -1;
			ball.col_vel *= -1;
			me.col = SZ - 1 - me.col;
			ball.col = C - 1 - ball.col;
		}

		if (ball.col >= C || ball.col < 0 || ball.col_vel <= 0 || ball.col > me.col + 8 * SZ)
			return { -1, -1 };

		while (ball.col > me.col)
			ball.col -= 4 * SZ; // normalize to first segment

		double time_millis = 1000 * (me.col - ball.col) / ball.col_vel;

		// Idea: try different priorities than just 1/t
		double priority = 1.0 / (time_millis + 1.0); 
		return { simulate_row(ball.row, ball.row_vel, time_millis), priority };
	}

	std::pair<int, int> move(MovingObject me, MovingObject opponent,
							std::vector<MovingObject> balls, int timer_offset) {
		
		vector<double> value(R, 0.0);
		bool hasball = false;
		for (auto b : balls) {
			auto p = next_intersect(me, b);
			if (p.first != -1) {
				hasball = true;
				double priority = p.second;
				for (int r = 0; r < R; r++) {
					int delta = r - p.first;
					double normalpdf = exp(-pow(delta, 2) /	8); // fudging factor
					value[r] += priority * normalpdf;
				}
			}
			p = next_block(me, b);
			if (p.first != -1) {
				hasball = true;
				double priority = p.second;
				for (int r = 0; r < R; r++) {
					int delta = r - p.first;
					double normalpdf = exp(-pow(delta, 2) /	8); // fudging factor
					value[r] -= priority * normalpdf;
				}
			}
		}

		int midpt = me.row + PS/2;

		if (!hasball) {
			if (midpt > R / 2 + 1)
				return { -100, 0 };
			if (midpt < R / 2 - 1)
				return { 100, 0 };
			return { 0, 0 };
		}

		int best = 0; double bestval = 0;
		for (int row = 0; row + PS <= R; row++) {
			double val = 0;
			for (int i = row; i < row + PS; i++)
				val += value[i];
			if (val > bestval)
				best = row, bestval = val;
		}

		if (LEFT)
			cout << fixed << setprecision(14) << bestval << endl;

		if (best == 0) return make_pair(-100, 0);
		if (best == R - PS) return make_pair(100, 0);

		if (me.row > best + 2) return make_pair(-100, 0);
		if (me.row > best + 1) return make_pair(-30, 0);
		if (me.row < best - 2) return make_pair(100, 0);
		if (me.row < best - 1) return make_pair(30, 0);
		return make_pair(0, 0);
	}
};

