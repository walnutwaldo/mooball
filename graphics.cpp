#include <iostream>
#include <sys/time.h>
#include <GL/glut.h> 
#include <GL/freeglut.h>
#include "graphics.h"

static int window_id, window_xsize, window_ysize, zoom = 3;
static double window_scale = 1.0;
static void (*my_render)(void);
static void (*my_keyhandler)(int key);
static void (*my_mousehandler)(int x, int y);

void draw_text(int x, int y, char *text)
{
  glRasterPos2i(x, y);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)text);
}

static void gr_change_size(int w, int h)
{
  glViewport(0, 0, w, h);
  window_xsize = w;
  window_ysize = h;
}

void gr_set_orthographic_projection( void )
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, window_xsize, 0, window_ysize);
  glScalef(1, -1, 1);
  glTranslatef(0, -window_ysize, 0);	
  glMatrixMode(GL_MODELVIEW);
}

void gr_process_normal_keys(unsigned char key, int x, int y)
{
  my_keyhandler(key);
  glutPostRedisplay();
}

void gr_process_special_keys(int key, int x, int y)
{
  my_keyhandler(key | 0x100);
  glutPostRedisplay();		
}

void gr_process_mouse(int x, int y)
{
  my_mousehandler(x, y);
  //  glutPostRedisplay();
}

void set_color(double r, double g, double b)
{
  glColor3f(r,g,b);
}

void draw_line (double x1, double y1, double x2, double y2)
{
  glBegin(GL_LINES);
  glVertex2f(x1*window_scale,y1*window_scale);
  glVertex2f(x2*window_scale,y2*window_scale);
  glEnd();
}

void draw_pixel (int x, int y)
{
  glBegin(GL_LINES);
  for (int y2=y*zoom; y2<y*zoom+zoom; y2++) {
    glVertex2f(x*zoom,y2);
    glVertex2f(x*zoom+zoom,y2);
  }
  glEnd();
}

void gr_render( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gr_set_orthographic_projection();
  glPushMatrix();
  glLoadIdentity();

  my_render();
 
  glPopMatrix();
  glFlush();
  glutSwapBuffers();
}

void moosball_update(long long time_chg);

int get_zoom(void) { return zoom; }

long long last_timer = -1;
long long last_refresh = -1;
void gr_timerhandler(int v) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long long current_timer = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  if (last_timer != -1) {
    moosball_update(current_timer - last_timer);
  }
  last_timer = current_timer;
  //  if (last_refresh == -1 || current_timer - last_refresh > 100) {
  //    last_refresh = current_timer;
    glutPostRedisplay();
    //  }
  glutTimerFunc(30, gr_timerhandler, 0);
}

int init_graphics(int argc, char **argv,
		  int window_xsize, int window_ysize,
		  void (* render)(void),
		  void (* keyhandler)(int key),
		  void (* mousehandler)(int x, int y)) 		  
{
  my_render = render;
  my_keyhandler = keyhandler;
  my_mousehandler = mousehandler;
  
  glutInit(&argc, argv);
  //glEnable(GL_DEPTH_TEST);
  //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(20,400);
  glutInitWindowSize(window_xsize*zoom, window_ysize*zoom);
  window_id = glutCreateWindow("MoosBall");
	
  glutSetWindow( window_id );
  glutReshapeFunc( gr_change_size ); 
  glutDisplayFunc( gr_render ); 
  glutKeyboardFunc( gr_process_normal_keys );
  glutSpecialFunc( gr_process_special_keys );
  glutPassiveMotionFunc( gr_process_mouse );
  glutTimerFunc(500, gr_timerhandler, 0);
	
  glutMainLoop();
  return 0;
}

