#include <bits/stdc++.h>
#include "walbot.h"

#define MO MovingObject

#define F first
#define S second

using namespace std;

typedef pair<int, int> pii;

const int speed = 10;
const int balls_considered = 100;
const double pntdiv = 2000;
const double reversepnts = -0.001;
const int criticalInPlay = 30;
const int smash_dis = 5;

void walbot::init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left)
{
	r = Nrows;
	c = Ncols;
	ppp = Npaddles_per_player;
	ps = paddle_size;
	amleft = am_i_left;
	bot_name = "WALBOT";
}

double walbot::paddleHits(int paddlePos, int ballPos) {
	double base = 0;
	if(ballPos >= paddlePos + 1 && ballPos < paddlePos + ps - 1) base = 1;
	if(ballPos >= paddlePos + ps * 2 / 5 && ballPos < paddlePos + ps * 3 / 5) base *= 1.01;
	return base;
}

bool walbot::should_smash(MO me, vector<MO> balls) {
	int closest = 100;
	int strip_width = c / (4 * ppp - 1);
	int dis_bw_paddles = 4 * strip_width;
	int loc = me.col;
	if(!amleft) loc = c - strip_width + loc;

	int inPlay = 0;
	for(const MO o : balls) {
		if(o.col >= 0 && o.col < c) inPlay++;
		if(((amleft && o.col > loc) || (!amleft && o.col < loc)) && paddleHits(me.row, o.row)){
			closest = min(closest, abs(o.col - loc));
		}
	}
	return inPlay >= criticalInPlay && closest < smash_dis;
}

pair<int,int> walbot::move(MO me, MO opponent, vector<MO> balls, int timer_offset)
{
	int strip_width = c / (4 * ppp - 1);
	int dis_bw_paddles = 4 * strip_width;

	int loc = me.col;
	if(!amleft) loc = c - strip_width + loc;
	vector<pair<double, pair<int, double>>> v;
	
	for (int i = 0; i < balls.size(); i++) {
		if (balls[i].col < 0 || balls[i].col >= c) continue;	
		if(amleft ^ (balls[i].col > loc)) continue;
		if(balls[i].col_vel == 0) continue;
		if(amleft ^ (balls[i].col_vel < 0)) { // moving away
			int nearLoc = loc + ((amleft)?1 : -1) * dis_bw_paddles;
			double pnts = reversepnts;
			int amtMove = 0;
			if (amleft) {
				while(nearLoc - dis_bw_paddles > balls[i].col && amtMove < ppp - 1) {
					nearLoc -= dis_bw_paddles;
					amtMove++;
				}
			} else {
				while(nearLoc + dis_bw_paddles < balls[i].col && amtMove < ppp - 1) {
					nearLoc += dis_bw_paddles;
					amtMove++;
				}
			}
			double t = (double)abs(balls[i].col - nearLoc) / abs(balls[i].col_vel);
			int targetRow = (int)(balls[i].row + t * balls[i].row_vel);
			targetRow = ((targetRow % (2 * r)) + 2 * r) % (2 * r);
			if(targetRow >= r) 
				targetRow = 2 * r - targetRow;
			v.push_back(make_pair(t, make_pair(targetRow, pnts)));
		} else {
			int nearLoc = loc;
			double pnts = 1;
			int amtMove = 0;
			if(amleft) {
				while(nearLoc + dis_bw_paddles < balls[i].col && amtMove < ppp - 1) {
					nearLoc += dis_bw_paddles;
					pnts /= pntdiv;
					amtMove++;
				}
			} else {
				while(nearLoc - dis_bw_paddles > balls[i].col && amtMove < ppp - 1) {
					nearLoc -= dis_bw_paddles;
					pnts /= pntdiv;
					amtMove++;
				}
			}
			if (amtMove == ppp - 1) pnts *= pntdiv;
			
			double t = (double)abs(balls[i].col - nearLoc) / abs(balls[i].col_vel);
			int targetRow = (int)(balls[i].row + t * balls[i].row_vel);
			targetRow = ((targetRow % (2 * r)) + 2 * r) % (2 * r);
			if(targetRow >= r) 
				targetRow = 2 * r - targetRow;
			v.push_back(make_pair(t, make_pair(targetRow, pnts)));
			cout << targetRow << endl;
		}
	}
	sort(v.begin(), v.end()); // time, targetrow, pnts
	int bc = min(balls_considered, (int)v.size());
	pair<double, int> dp[bc + 1][r - ps + 1];
	for (int i = 0; i < bc + 1; i++) for (int j = 0; j < r - ps + 1; j++) dp[i][j] = make_pair(-1e6, 0);
	dp[0][me.row] = make_pair(0.0, -1);
	for (int i = 0; i < bc; i++) {
		for (int paddlePos = 0; paddlePos < r - ps + 1; paddlePos++) {
			for (int k = 0; k < r - ps + 1; k++) {
				int lastPaddlePos = (me.row + k) % (r - ps + 1);
				if(lastPaddlePos == paddlePos || (int)floor(30 * max(0.0, v[i].F - ((i == 0)?0:v[i - 1].F))) * 3 >= abs(paddlePos - lastPaddlePos)) {
					pair<double, int> np = make_pair(dp[i][lastPaddlePos].F + v[i].S.S * paddleHits(paddlePos, v[i].S.F), lastPaddlePos);
					if(dp[i + 1][paddlePos].F < np.F) dp[i + 1][paddlePos] = np;
				}
			}
		}
	}
	int targetRow = r / 2 - ps / 2;
	if(!v.empty()) {
		int curr = bc;
		pair<double, int> res = dp[curr][0];
		for (int i = 0; i < r - ps + 1; i++) if(dp[curr][i].F > res.F) {
			res = dp[curr][i];
			if(res.F > 0) targetRow = i;
		}
		cout << res.F << " ";
		while(curr > 1) {
			if(dp[curr - 1][res.S].F > 0) targetRow = res.S;
			res = dp[--curr][res.S];
		}
		cout << targetRow << endl;
	}
	int r_mag = 0;
	if(targetRow == me.row) r_mag = 0;
	else if(abs(targetRow - me.row) < 1) r_mag = 10;
	else if(abs(targetRow - me.row) < 3) r_mag = 50;
	else r_mag = 100;
	int r_vel = 0;
	if(targetRow > me.row) r_vel = r_mag;
	else r_vel = -r_mag;
	
	int c_vel = 0;
	if (should_smash(me, balls)) {
		c_vel = (amleft)?100:-100;
		r_vel = (rand() % 2) * 200 - 100;
	}
	else if(amleft && me.col > 4) c_vel = -100;
	else if(!amleft && me.col < c - 5) c_vel = 100;
	return make_pair(r_vel, c_vel);
}
