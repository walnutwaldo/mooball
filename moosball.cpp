#include <bits/stdc++.h>
#include <cstdio>
#include <cstring>
#include "bot.h"
#include "moosball.h"
#include "graphics.h"
using namespace std;

static int Xsize = 600, Ysize = 72;
static int Ncols = 550, Nrows = 68, Npaddles = 3, paddle_size = 20, playing;
static int max_rvel = 100, max_cvel = 100;
static vector<Bot *> bots;
static MovingObject paddles[2];
static vector<MovingObject> balls;
static vector<int> lastbounce;
static int score[2];

void add_ball(void)
{
  MovingObject new_ball;
  new_ball.col = rand() % (Ncols * 1000/2) + Ncols * 1000/4;
  new_ball.row = rand() % (Nrows * 1000);
  new_ball.row_vel = 30 + rand() % 20;
  if (rand() % 2) new_ball.row_vel *= -1;
  new_ball.col_vel = 50 + rand() % 20;
  if (rand() % 2) new_ball.col_vel *= -1;
  balls.push_back(new_ball);
  lastbounce.push_back(-1);
}

void clip_vels(void)
{
  for (int i=0; i<2; i++) {
    paddles[i].row_vel = max(paddles[i].row_vel, -max_rvel);
    paddles[i].row_vel = min(paddles[i].row_vel, max_rvel);
    paddles[i].col_vel = max(paddles[i].col_vel, -max_cvel);
    paddles[i].col_vel = min(paddles[i].col_vel, max_cvel);
  } 
}

void keyhandler(int key)
{
  if (key == 'q') exit(0);
  if (bots.size() == 1) {
    if (key == ' ') { paddles[1].row_vel = paddles[1].col_vel = 0; }
    if (key == KEY_UP) { paddles[1].row_vel = min(paddles[1].row_vel,0) - 20; }
    if (key == KEY_DOWN) { paddles[1].row_vel = max(paddles[1].row_vel,0) + 20; }
    if (key == KEY_LEFT) { paddles[1].col_vel = min(paddles[1].col_vel,0) - 50; }
    if (key == KEY_RIGHT) { paddles[1].col_vel = max(paddles[1].col_vel,0) + 50; }
    clip_vels();
  }
  if (key == '+' || key == '=') add_ball();
}

int get_zoom(void);
void mousehandler(int x, int y)
{
  if (bots.size() == 1) {
    paddles[1].row_vel = (y/get_zoom() - Ysize/2) * 2;
    paddles[1].col_vel = (x/get_zoom() - Xsize/2) * 2;
    clip_vels();
  }
}			     

struct ppx {
  int r, c;
  int rv, cv;
  int player, id;
};

vector<ppx> get_paddle_pixels(void)
{
  vector<ppx> px;
  for (int n=0; n<2*Npaddles; n++) {
    int col_offset = 2*n*(Ncols / (4*Npaddles-1));
    ppx p;
    p.player = n%2;
    p.r = paddles[p.player].row / 1000;
    p.c = paddles[p.player].col / 1000 + col_offset;
    p.rv = paddles[p.player].row_vel;
    p.cv = paddles[p.player].col_vel;
    p.id = n;
    for (int i=0; i<paddle_size; i++) {
      px.push_back(p);
      p.r++;
    }
  }
  return px;
}

void render(void)
{
  set_color (1.0, 1.0, 0.5);
  if (bots[0]->bot_name) 
    draw_text(Xsize*get_zoom()/4 - strlen(bots[0]->bot_name)*12, Ysize*get_zoom()+20, (char *)bots[0]->bot_name);
  set_color (0.5, 1.0, 1.0);
  if (bots.size() == 1) 
    draw_text(Xsize*get_zoom()*3/4 - strlen("HUMAN")*12, Ysize*get_zoom()+20, (char *)"HUMAN");
  else if (bots[1]->bot_name) 
    draw_text(Xsize*get_zoom()*3/4 - strlen(bots[1]->bot_name)*12, Ysize*get_zoom()+20, (char *)bots[1]->bot_name);
  
  // Draw outline of playing field
  set_color (1.0, 0, 0);
  int border = 2, midx = Xsize/2, midy = Ysize/2, startx = midx-Ncols/2, starty = midy-Nrows/2;
  for (int i=0; i<Nrows+border*2; i++)
    for (int j=0; j<Ncols+border*2; j++)
      if (i<border || i>=Nrows+border || j<border || j>=Ncols+border)
	draw_pixel (startx-border+j, starty-border+i);

  // Draw balls
  set_color (1.0, 1.0, 1.0);
  for (int i=0; i<balls.size(); i++) {
    if (balls[i].col/1000 < 0 || balls[i].col/1000>=Ncols) continue;
    draw_pixel(startx + balls[i].col/1000, starty + balls[i].row/1000);
  }
  
  // Draw paddles
  vector<ppx> px = get_paddle_pixels();
  for (auto &p : px) {
    if (p.player == 0) set_color (1.0, 1.0, 0.5);
    else set_color (0.5, 1.0, 1.0);
    draw_pixel(startx + p.c, starty + p.r);
  }

  char buff[10];
  sprintf (buff, "%03d", score[0]);
  set_color (1.0, 1.0, 0.5);
  draw_text(0, 18, buff);
  sprintf (buff, "%03d", score[1]);
  set_color (0.5, 1.0, 1.0);
  draw_text(get_zoom()*Xsize-12*3, 18, buff);
}

void moosball_update(long long msec)
{
  MovingObject p2[2] = {paddles[0], paddles[1]};
  // we represent things in millipixels; convert to pixels for API...
  p2[0].row /= 1000; p2[0].col /= 1000; 
  p2[1].row /= 1000; p2[1].col /= 1000; 
  vector<MovingObject> b2 = balls;
  for (auto &b : b2) { b.row /= 1000; b.col /= 1000; }
  for (int i=0; i<bots.size(); i++) {
    pair<int,int> new_vels = bots[i]->move(p2[i], p2[1-i], b2, msec);
    paddles[i].row_vel = new_vels.first;
    paddles[i].col_vel = new_vels.second;
    clip_vels();
  }
  vector<ppx> px = get_paddle_pixels();
  for (int i=0; i<2; i++) {
    int T = msec * paddles[i].col_vel;
    double N = T!=0 ? abs(T) : 1;
    bool first = true;
    while (first || T != 0) {
      int x;
      first = false;
      if (T > 0) { x = min(T,1000); T -= x; }
      else { x = max(T,-1000), T -= x; }
      paddles[i].row += (x ? abs(x/N) * msec * paddles[i].row_vel : msec * paddles[i].row_vel);
      if (paddles[i].row < 0) { paddles[i].row = 0; paddles[i].row_vel = 0; }
      if (paddles[i].row/1000 + paddle_size >= Nrows) { paddles[i].row = (Nrows-paddle_size)*1000; paddles[i].row_vel = 0; }
      paddles[i].col += x;  
      int col_width = Ncols / (4*Npaddles-1);
      if (paddles[i].col < 0) { paddles[i].col = 0; paddles[i].col_vel = 0; T = 0; }
      if (paddles[i].col/1000 >= col_width) { paddles[i].col = (col_width)*1000-1; paddles[i].col_vel = 0; T = 0; }
      vector<ppx> px2 = get_paddle_pixels();
      for (auto &p : px2) px.push_back(p);
    }
  }
  for (int i=0; i<balls.size(); i++) {
    MovingObject &b = balls[i];
    if (b.col/1000 < 0 || b.col/1000>=Ncols) continue;
    int T = msec * b.col_vel;
    double N = T != 0 ? abs(T) : 1;
    bool first = true;
    while (T != 0 || first) {
      first = false;
      if (b.col/1000 < 0 || b.col/1000>=Ncols) break;
      int nrow, ncol, x;
      if (T > 0) { x = min(T,1000); T -= x; }
      else { x = max(T,-1000); T -= x; }
      nrow = b.row + (x ? abs(x/N) * msec * b.row_vel : msec * b.row_vel);
      ncol = b.col + x;
      for (auto &p : px) {
	if (nrow/1000 == p.r && ncol/1000 == p.c && p.id != lastbounce[i] /*&& p.cv * b.col_vel <= 0*/) {
	  lastbounce[i] = p.id;
	  b.col_vel *= -1;
	  b.col_vel += paddles[p.player].col_vel;
	  T = msec * b.col_vel;
	  N != 0 ? abs(T) : 1;
	  ncol = b.col + x;
	  b.row_vel += 0.25 * paddles[p.player].row_vel;
	}
      }
      if (nrow/1000 != b.row/1000 && (nrow/1000 < 0 || nrow/1000 >= Nrows)) {
	b.row_vel *= -1;
	nrow = b.row + (x ? abs(x/N) * msec * b.row_vel : msec * b.row_vel);
      }
      if (ncol/1000 != b.col/1000 && (ncol/1000 < 0 || ncol/1000 >= Ncols)) {
	b.col = ncol;
	if (ncol/1000 < 0) score[1]++;
	else if (ncol/1000 >= Ncols) score[0]++;
	continue;
      }
      b.row = nrow;
      b.col = ncol;
    }  
  }
}

void moosball_play(int argc, char *argv[])
{
  if (bots.size() < 1) {
    cout << "Error: You haven't added 1 or 2 bots to the game.\n";
    cout << "This requires calling moosball_addbot() once or twice in main().\n";
    exit(0);
  }
  playing = 1;
  bots[0]->init(Nrows, Ncols, Npaddles, paddle_size, true);
  if (bots.size() == 2)
    bots[1]->init(Nrows, Ncols, Npaddles, paddle_size, false);
  paddles[0].row = paddles[1].row = (Nrows/2 - paddle_size/2)*1000;
  paddles[0].col = paddles[1].col = Ncols*1000/(4*Npaddles-1)/2;
  for (int i=0; i<1; i++)
    add_ball();
  init_graphics(argc, argv, Xsize, Ysize + 15, render, keyhandler, mousehandler);
}

void moosball_addbot(Bot *b)
{
  if (playing) {
    cout << "You can't add bots once the game has already started!\n";
    exit(0);    
  }
  bots.push_back(b);
}
