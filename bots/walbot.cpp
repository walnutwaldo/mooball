#include <bits/stdc++.h>
#include "walbot.h"

#define MO MovingObject
#define MP make_pair

#define F first
#define S second
#define PB push_back

using namespace std;

typedef pair<int, int> pii;

const int balls_considered = 100;
const double reversepnts = -0.001;
const int criticalInPlay = 30;

void walbot::init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left)
{
	r = Nrows;
	c = Ncols;
	ppp = Npaddles_per_player;
	ps = paddle_size;
	amleft = am_i_left;
	bot_name = "WALBOT";
	strip_width = c / (4 * ppp - 1);
	dis_bw_paddles = 4 * strip_width;
}

double walbot::paddleHits(int paddlePos, int ballPos) {
	double base = 0;
	if(ballPos >= paddlePos && ballPos < paddlePos + ps) base = 1;
	if(ballPos >= paddlePos + ps * 2 / 5 && ballPos < paddlePos + ps * 3 / 5) base *= 1.01;
	return base;
}

bool walbot::about_to_hit(MO me, vector<MO> balls, int timer_offset) {
	for(const MO o : balls) for(int i = 0; i < ppp; i++) {
		if(o.col_vel < 0 && abs(o.col - (me.col + i * dis_bw_paddles)) < (-o.col_vel) * timer_offset / 1000 && paddleHits(me.row, o.row)) return true;
	}
	return false;
}

bool walbot::should_smash(MO me, vector<MO> balls, int timer_offset) {
	int inPlay = 0;
	int cnt1 = 0, cnt2 = 0;
	for(const MO o : balls) {
		if(o.col >= 0 && o.col < c) inPlay++;
		if((o.col > me.col + (ppp - 1) * dis_bw_paddles && o.col_vel < 0) && paddleHits(me.row, o.row)) 
			if(o.col - me.col - (ppp - 1) * dis_bw_paddles <= (100 - o.col_vel) * timer_offset / 1000)
				cnt1++;
		if((o.col > me.col && o.col_vel < 0) && paddleHits(me.row, o.row))
			if(o.col - me.col <= (100 - o.col_vel) * timer_offset / 1000)
				cnt2++;
	}
	return (inPlay >= criticalInPlay && cnt1 > 0) || (cnt1 > 1.25 * cnt2);
}

vector<pair<double, pair<int, double>>> walbot::getHitLocs(MO me, vector<MO> balls, int timer_offset) {
	int enemypnts = 0;
	for(const MO ball : balls) if(ball.col < 0) enemypnts++;
	vector<pair<double, pair<int, double>>> res;
	for(const MO ball : balls) {
		if(ball.col < 0 || ball.col >= c) continue;
		if(ball.col < me.col) continue;
		if(ball.col_vel == 0) continue;
		int nearLoc = me.col, amtMove = 0;
		double pnts = 1;
		if(ball.col_vel > 0) {
			nearLoc = me.col + (ppp - 1) * dis_bw_paddles;
			pnts = reversepnts;
			while(amtMove < ppp - 1 && nearLoc - dis_bw_paddles > ball.col) {
				nearLoc -= dis_bw_paddles;
				amtMove++;
			}
		} else {
			while(amtMove < ppp - 1 && nearLoc + dis_bw_paddles < ball.col) {
				nearLoc += dis_bw_paddles;
				amtMove++;
			}
			if(amtMove == ppp - 1) pnts = 0.75;
			else if(amtMove != 0) pnts = 0.25;
			if(amtMove == 0 && enemypnts >= 40) pnts = 2;
			else if(amtMove == 0 && enemypnts >= 45) pnts = 5 + 10 * (enemypnts - 45);
		}
		double t = (double)abs(ball.col - nearLoc) / abs(ball.col_vel);
		t = (double)((int)(t * 1000) - ((int)(t * 1000) % timer_offset)) / 1000;
		int targetRow = (int)(ball.row + t * ball.row_vel);
		targetRow = ((targetRow % (2 * r)) + 2 * r) % (2 * r);
		if(targetRow >= r) 
			targetRow = 2 * r - targetRow;
		res.push_back(MP(t, MP(targetRow, pnts)));
	}
	sort(res.begin(), res.end());
	return res;
}

int walbot::getTargetRow(MO me, vector<pair<double, pair<int, double>>> v) {
	int bc = min(balls_considered, (int)v.size());
	pair<double, int> dp[bc + 1][r - ps + 1];
	for (int i = 0; i < bc + 1; i++) for (int j = 0; j < r - ps + 1; j++) dp[i][j] = make_pair(-1e6, 0);
	dp[0][me.row] = make_pair(0.0, -1);
	for (int i = 0; i < bc; i++) {
		for (int paddlePos = 0; paddlePos < r - ps + 1; paddlePos++) {
			for (int k = 0; k < r - ps + 1; k++) {
				int lastPaddlePos = (me.row + k) % (r - ps + 1);
				if(lastPaddlePos == paddlePos || (int)((v[i].F - ((i == 0)?0:v[i - 1].F)) * 90) >= abs(paddlePos - lastPaddlePos)) {
					pair<double, int> np = make_pair(dp[i][lastPaddlePos].F + v[i].S.S * paddleHits(paddlePos, v[i].S.F), lastPaddlePos);
					if(dp[i + 1][paddlePos].F < np.F) dp[i + 1][paddlePos] = np;
				}
			}
		}
	}
	int targetRow = (r - ps) / 2;
	if(!v.empty()) {
		int curr = bc;
		pair<double, int> res = dp[curr][0];
		for (int i = 0; i < r - ps + 1; i++) if(dp[curr][i].F > res.F) {
			res = dp[curr][i];
			if(res.F > 0) targetRow = i;
		}
		while(curr > 1) {
			if(dp[curr - 1][res.S].F > 0) targetRow = res.S;
			res = dp[--curr][res.S];
		}
	}
	return targetRow;
}

pair<int,int> walbot::move(MO me, MO opponent, vector<MO> balls, int timer_offset)
{
	if(!amleft) {
		me.col = strip_width - 1 - me.col;
		opponent.col = strip_width - 1 - opponent.col;
		for(MO& mo : balls) {
			mo.col = c - 1 - mo.col;
			mo.col_vel = -mo.col_vel;
		}
	}
	vector<pair<double, pair<int, double>>> v = getHitLocs(me, balls, timer_offset);
	int targetRow = getTargetRow(me, v);
	int r_vel = max(-100, min(100, (targetRow - me.row) * 1000 / timer_offset));
	int c_vel;

	for (MO& o : balls) o.row += o.row_vel * timer_offset / 1000;

	if (about_to_hit(me, balls, timer_offset)) {
		int disToOpponent = 2 * strip_width - me.col + opponent.col;
		double bestV = -1;
		for(int cv = -25; cv <= 25; cv++) for(int rv = -25; rv <= 25; rv++) {
			vector<int> v;
			for(const MO ball : balls) if(ball.col >= me.col && ball.col < c) {
				int newR;
				if(ball.col_vel < 0) {
					for(int i = 0; i < 3; i++)
						if(ball.col > me.col + i * dis_bw_paddles && (ball.col - me.col) < (cv - ball.col_vel) * timer_offset / 1000 && paddleHits(me.row + rv, ball.row)) {
							int newCV = -ball.col_vel + cv;
							int newRV = ball.row_vel + 0.25 * rv;
							newR = disToOpponent * newRV / newCV + ball.row;
							if(newCV < 40) newR = opponent.row;
						}
				} else if(ball.col < me.col + disToOpponent + (ppp - 1) * dis_bw_paddles) {
					int nearestOpp = me.col + disToOpponent + (ppp - 1) * dis_bw_paddles;
					while(ball.col < nearestOpp - dis_bw_paddles)
						nearestOpp -= dis_bw_paddles;
					newR = (nearestOpp - ball.col) * ball.row_vel / ball.col_vel + ball.row;
					if(ball.col_vel < 40) newR = opponent.row;
				}
				newR = ((newR % (2 * r)) + 2 * r) % (2 * r);
				if(newR >= r) newR = 2 * r - newR;
				v.PB(max(0, newR - opponent.row + ps - 1) + max(0, opponent.row - newR));
			}
			double mean = 0;
			for(const int val : v) mean += val;
			mean /= ((int)v.size());
			double var = 0;
			for(const int val : v) var += (val - mean) * (val - mean);
			if(var > bestV) {
				bestV = var;
				r_vel = rv;
				c_vel = cv;
			}
		}
	} else {
		if(me.col > strip_width * 3 / 5) c_vel = -25;
		else if(me.col < strip_width * 2 / 5) c_vel = 25;
	}
	
	pair<int, int> res = MP(r_vel, c_vel);
	if(!amleft) res.S *= -1;
	return res;
}
