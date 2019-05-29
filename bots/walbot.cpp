#include <bits/stdc++.h>
#include "walbot.h"

#define MO MovingObject
#define MP make_pair

#define F first
#define S second

using namespace std;

typedef pair<int, int> pii;

const int balls_considered = 100;
const double pntdiv = 2;
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
	if(ballPos >= paddlePos + 1 && ballPos < paddlePos + ps - 1) base = 1;
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
				pnts /= pntdiv;
			}
			if(amtMove == ppp - 1) pnts = 1.0 / pntdiv;
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
		for(MO& mo : balls) {
			mo.col = c - 1 - mo.col;
			mo.col_vel = -mo.col_vel;
		}
	}
	vector<pair<double, pair<int, double>>> v = getHitLocs(me, balls, timer_offset);
	int targetRow = getTargetRow(me, v);

	int r_mag = 0;
	if(targetRow == me.row) r_mag = 0;
	else if(abs(targetRow - me.row) < 1) r_mag = 10;
	else if(abs(targetRow - me.row) < 3) r_mag = 50;
	else r_mag = 100;
	int r_vel = 0;
	if(targetRow > me.row) r_vel = r_mag;
	else r_vel = -r_mag;
	me.row += r_vel * timer_offset / 1000;

	for (MO& o : balls) o.row += o.row_vel * timer_offset / 1000;

	if (about_to_hit(me, balls, timer_offset))
		r_vel = max(-100, min(100, 100 * (r_vel + 1)));
	int c_vel = 0;
	if (should_smash(me, balls, timer_offset)) {
		c_vel = 100;
	} else if(me.col > 4) c_vel = -25;
	
	pair<int, int> res = MP(r_vel, c_vel);
	if(!amleft) res.S *= -1;
	return res;
}
