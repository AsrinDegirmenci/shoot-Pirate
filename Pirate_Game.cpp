
/*********
   CTIS164 - Template Source Program
----------
STUDENT : Mustafa Asrın Değirmenci
SECTION : 02
HOMEWORK: HOMEWORK-2
----------
PROBLEMS:   No problems observed!
----------
ADDITIONAL FEATURES: Multiple fires, score counter, timer countdown, changing colors of the cannonballs, winner-loser screens,
 objective for the player, height adjustable platform for the cannon,  different speed for every target, two-way  background color transition
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define RADIUS 10
#define TARGET_RADIUS 50 // hit marker's boundaries
#define START 0   //
#define GAME 1  // screen values
#define END 2    //
#define COUNT 60

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false,
spacebar = false; // activate or deactivate shooting the cannonball

int  winWidth, winHeight, state = 0;// current Window width and height


double timer = COUNT;

typedef struct {
    float x, y;
}loc_t;

typedef struct {
    loc_t pos;
    float   angle;
    float   r;
} cannon_t;


typedef struct {
    int r, g, b;
} color_t;

typedef struct {
    loc_t pos;
    bool active;
    float angle;
} cannonball_t;


typedef struct {
    loc_t pos;
    color_t color;
    float radius;
    float speed;
} pirate_t;

typedef struct {
    loc_t pos, pos2;
    color_t color;
    float inc;
}platform_t;

#define MAX_FIRE 10 // 5 fires at a time.
#define FIRE_RATE 16 // after 16 frames you can throw another one.

cannon_t cannon = { { -450,RADIUS }, 45, 20 };
cannonball_t   ammo[MAX_FIRE];
int fire_rate = 0, score = 0;
pirate_t pirate;
platform_t platform = {{-500,-400}, {-400, 0}, {170,250,15}, 5};





//
// to draw circle, center at (x,y)
// radius r
//
void circle( int x, int y, int r )
{
#define PI 3.1415
   float angle ;
   glBegin( GL_POLYGON ) ;
   for ( int i = 0 ; i < 100 ; i++ )
   {
      angle = 2*PI*i/100;
      glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
   }
   glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
   float angle;

   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++)
   {
      angle = 2 * PI*i / 100;
      glVertex2f(x + r*cos(angle), y + r*sin(angle));
   }
   glEnd();
}

void print(int x, int y, const char *string, void *font )
{
   int len, i ;

   glRasterPos2f( x, y );
   len = (int) strlen( string );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, string[i]);
   }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string , ...)
{
   va_list ap;
   va_start ( ap, string );
   char str[1024] ;
   vsprintf( str, string, ap ) ;
   va_end(ap) ;
   
   int len, i ;
   glRasterPos2f( x, y );
   len = (int) strlen( str );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, str[i]);
   }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
   va_list ap;
   va_start(ap, string);
   char str[1024];
   vsprintf(str, string, ap);
   va_end(ap);
   glPushMatrix();
      glTranslatef(x, y, 0);
      glScalef(size, size, 1);
      
      int len, i;
      len = (int)strlen(str);
      for (i = 0; i<len; i++)
      {
         glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
      }
   glPopMatrix();
}

void drawBg(){
    
//    background => two-way color transition
    
    glColor3ub(255, 255, 255);
    glBegin(GL_POLYGON);
    
    glVertex2f(500, 400);
    glVertex2f(-500, 400);
    
    glColor3ub(100, 255, 224);
    
    glVertex2f(-500, -400);
    glVertex2f(500, -400);
    
    glEnd();
    
//    Info label card => Name and Surname AND homework label
    glColor3f(1, 0, 0.4);
    glRectf(-470, 380, -280, 300);
    
    glColor3f(0, 0, 0);
    vprint(-460, 360, GLUT_BITMAP_HELVETICA_18, "Name: Mustafa Asrin");
    vprint(-460, 340, GLUT_BITMAP_HELVETICA_18, "Surname: Degirmenci");
    vprint(-440, 310, GLUT_BITMAP_HELVETICA_18, "HOMEWORK #2");
    
    //    if the player wants to restart his/her process, s/he can by pressing the 'k' button
        vprint(280, -370, GLUT_BITMAP_HELVETICA_12, "To restart, Press <k> button again");
    
//    x and y axis
    glColor3f(0.4, 0, 1);
    glLineWidth(2);
    glBegin(GL_LINES);
    
    for (int i = -500; i < 500; i += 10) {
        glVertex2f(i, 0);
    }
    
    for (int k = -400; k < 400; k += 10) {
        glVertex2f(0, k);
    }
    
    glEnd();
    glLineWidth(1);
    
    
}

void drawPirate(pirate_t p){
    
    glColor3ub(p.color.r, p.color.g, p.color.b);
    
//    pirate => human body -naked-
    
    circle(p.pos.x, p.pos.y + 30, 15);
    
    glLineWidth(5);
    glBegin(GL_LINES);
    
//    body
    glVertex2f(p.pos.x, p.pos.y +15);
    glVertex2f(p.pos.x, p.pos.y -30);
    
//    legs
    glVertex2f(p.pos.x - 10, p.pos.y -70);
    glVertex2f(p.pos.x, p.pos.y -30);

    glVertex2f(p.pos.x + 10, p.pos.y -70);
    glVertex2f(p.pos.x , p.pos.y -30);
    
//    arms
    glVertex2f(p.pos.x, p.pos.y);
    glVertex2f(p.pos.x - 25, p.pos.y - 10);
    
    glVertex2f(p.pos.x, p.pos.y);
    glVertex2f(p.pos.x + 25, p.pos.y - 10);
    
    glEnd();
    glLineWidth(1);
    
    
//    sword => steel part with custom colors
    
    glBegin(GL_TRIANGLES);
    
    glVertex2f(p.pos.x -35, p.pos.y + 5);
    glVertex2f(p.pos.x -24, p.pos.y + 5);
    glVertex2f(p.pos.x -50, p.pos.y + 40);
    
    glEnd();
    
//    pirate => eyepatch
    glColor3f(0, 0, 0);
    circle(p.pos.x + 5, p.pos.y + 27, 5);
    
    glLineWidth(3);
    glBegin(GL_LINES);
    
    glVertex2f(p.pos.x +5, p.pos.y +30);
    glVertex2f(p.pos.x +10, p.pos.y +20);
    
    glEnd();
    glLineWidth(1);
    
    //    pirate => pirate hat
        glColor3f(0, 0, 0);
        glLineWidth(5);
        glBegin(GL_LINES);
    //    pirate hat => bottom panel
        glVertex2f(p.pos.x-30, p.pos.y + 30);
        glVertex2f(p.pos.x+30, p.pos.y + 30);
        
        glEnd();
        
    //    pirate hat => upper panel
        glBegin(GL_TRIANGLES);
        
        glVertex2f(p.pos.x -20, p.pos.y +30);
        glVertex2f(p.pos.x +20, p.pos.y +30);
        glVertex2f(p.pos.x, p.pos.y +60);
                
        glEnd();
        glLineWidth(1);
        
    //    pirate hat => cross symbol
        glColor3f(1, 1, 1);
        glLineWidth(5);
        glBegin(GL_LINES);
        
        glVertex2f(p.pos.x -5 , p.pos.y +45);
        glVertex2f(p.pos.x +5 , p.pos.y +35);
        
        glVertex2f(p.pos.x -5 , p.pos.y +35);
        glVertex2f(p.pos.x +5 , p.pos.y +45);
        
        glEnd();
        glLineWidth(1);
        
    //    pirate => sword
        //sword => body
        glColor3f(1, 0, 0);
        glLineWidth(7);
        glBegin(GL_LINES);
        glVertex2f(p.pos.x - 15, p.pos.y -15);
        glVertex2f(p.pos.x - 30, p.pos.y + 5);
        glEnd();
        glLineWidth(1);
    
}

void drawPlatform(platform_t *p){
    glColor3ub(110, 44, 0);
    glRectf(p->pos.x, p->pos.y, p->pos2.x, p->pos2.y); // draws a height adjustable platform for the cannon
    
}

void drawCannon(cannon_t tp) {
    glColor3f(1, 1, 0);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(tp.pos.x, tp.pos.y);
    glVertex2f(tp.pos.x + 50 * cos(tp.angle * D2R), tp.pos.y + 50 * sin(tp.angle * D2R));
    glEnd();
    glLineWidth(1);

    glColor3f(0.3, 0.3, 1.0);
    circle(tp.pos.x, tp.pos.y, tp.r);
    glColor3f(1.0, 0.3, 0.3);
    circle(tp.pos.x, tp.pos.y, tp.r - 4);

    glColor3f(1, 1, 1);
    vprint(tp.pos.x - 12, tp.pos.y - 5, GLUT_BITMAP_8_BY_13, "%.0f", tp.angle);
}

void drawBullets() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (ammo[i].active) {
            glColor3ub(rand() % 256, rand() % 256, rand() % 256); // draw random colored bullets
            circle(ammo[i].pos.x, ammo[i].pos.y, 6); // draw the shape of the bullets (cannonball)
        }
    }
}


void drawEndWon(){
    
    glColor3f(1, 0, 0);
//    display the message for the winner screen
    vprint(-50, 20, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER!");
    vprint(-50, -20, GLUT_BITMAP_HELVETICA_18, "YOU HAVE WON!");
    vprint(-50, -60, GLUT_BITMAP_TIMES_ROMAN_10, "Press < k > button to start over!");
    
}
void drawEndLost(){
    
    glColor3f(1, 0, 0);
//    display the message for the loser screen
    vprint(-50, 20, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER!");
    vprint(-50, -20, GLUT_BITMAP_HELVETICA_18, "YOU HAVE LOST!");
    vprint(-50, -60, GLUT_BITMAP_TIMES_ROMAN_10, "Press < k > button to start over!");
    
}
void drawBgStart(){
//    draw the start screen with the game's background
    //    background => two-way color transition
        
    glColor3ub(255, 255, 255);
    glBegin(GL_POLYGON);
        
    glVertex2f(500, 400);
    glVertex2f(-500, 400);
        
    glColor3ub(100, 255, 224);
        
    glVertex2f(-500, -400);
    glVertex2f(500, -400);
        
    glEnd();
    //    Info label card => Name and Surname AND homework label
        glColor3f(1, 0, 0.4);
        glRectf(-470, 380, -280, 300);
        
        glColor3f(0, 0, 0);
        vprint(-460, 360, GLUT_BITMAP_HELVETICA_18, "Name: Mustafa Asrin");
        vprint(-460, 340, GLUT_BITMAP_HELVETICA_18, "Surname: Degirmenci");
        vprint(-440, 310, GLUT_BITMAP_HELVETICA_18, "HOMEWORK #2");
    
    glColor3f(1, 0, 0);
    vprint(-150, 20, GLUT_BITMAP_TIMES_ROMAN_24, "-=PIRATE OBLITERATOR=-");
    vprint(-150, -20, GLUT_BITMAP_HELVETICA_18, "To win obliterate at least 15 pirates!");
    vprint(-100, -60, GLUT_BITMAP_HELVETICA_18, "To play press < k > button");
}
void alterTargetLoc() {
    float yPos = rand() % 3 ? 300 -50 : -300 + 50; // give different height for the pirate to spawn
    pirate.pos = { 550, yPos };
    pirate.color.r = rand() % 256; // changes pirate's color everytime when this function is called
    pirate.color.g = rand() % 256;
    pirate.color.b = rand() % 256;
    pirate.radius = TARGET_RADIUS; // initialize the radius for the pirate's hit mark (every time)
    pirate.speed = 3.0 * (rand() % 100) / 100.0 + 2.0; // give different speed for the pirate (it will change every time when function gets called)
}


//
// To display onto window using OpenGL commands
//
void display() {
   //
   // clear window to black
   //
   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);
    
    if (state == START) {
        drawBgStart(); //display the start screen
        alterTargetLoc();
        timer = COUNT;
        score = 0;
        for (int i = 0; i < MAX_FIRE; i++) {
            ammo[i].active = false;
        }
    }
    if (state == GAME){ //display the game screen. This will be active by incrementing state integer's value
        drawBg();
        drawBullets(); //display the bullets on the screen
        drawPirate(pirate); //display the moving pirate
        drawPlatform(&platform); // display the platform
        drawCannon(cannon); // display the cannon itself
        glColor3ub(227, 20, 20);
//        display the score and the timer on to the sreen
            vprint(-450, 240, GLUT_BITMAP_HELVETICA_18, "COUNTER = %d", score);
            vprint(-450, 220, GLUT_BITMAP_HELVETICA_18, "TIMER = %.0f", timer );
    }
    if (timer < 0) { // when timer hits 0, the game will automatically stop
        timer = COUNT;
        state = END;
    }
    if (state == END) {
        if (score < 15) {
            drawEndLost();
            //      when changing between screens, reset the locations, delete the bullets that are active and initialize the score to 0
                    alterTargetLoc();
                    
                    for (int i = 0; i < MAX_FIRE; i++) {
                        ammo[i].active = false;
                    }
        }
        if (score >= 15) { // if you win, it will display the winner screen
            drawEndWon();
            alterTargetLoc();
            
            for (int i = 0; i < MAX_FIRE; i++) {
                ammo[i].active = false;
            }
        }

    }
   glutSwapBuffers();
}

int fireCheck() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (ammo[i].active == false) return i;
    }
    return -1;
}

void rotateCannon(cannon_t *c, float inc) {
    c->angle += inc; // when pressed, change the angle according to the button that pressed
    if (c->angle > 180) c->angle += 10; // limiting the angle to not fire inside the platform itself
    if (c->angle < 0) c->angle += 10;
}


//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);
    if (key == ' ') {
        spacebar = true; // when spacebar is true, the cannon will start firing
    }
    if (key == 'k' || key == 'K') {
        state++; // change between screens
        if (state > 1) { // stop and restart the game by changing the screen to the start screen
            state = 0;
        }
    }
    
   
    
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);
    if (key == ' ') {
        spacebar = false; // when spacebar is false, the cannon will stop firing
    }
  
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown( int key, int x, int y )
{
   // Write your codes here.
   switch (key) {
   case GLUT_KEY_UP: up = true; break;
   case GLUT_KEY_DOWN: down = true; break;
   case GLUT_KEY_LEFT: left = true; break;
   case GLUT_KEY_RIGHT: right = true; break;
   }

   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp( int key, int x, int y )
{
   // Write your codes here.
   switch (key) {
   case GLUT_KEY_UP: up = false; break;
   case GLUT_KEY_DOWN: down = false; break;
   case GLUT_KEY_LEFT: left = false; break;
   case GLUT_KEY_RIGHT: right = false; break;
   }

   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick( int button, int stat, int x, int y )
{
   // Write your codes here.


   
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize( int w, int h )
{
   winWidth = w;
   winHeight = h;
   glViewport( 0, 0, w, h ) ;
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
   glMatrixMode( GL_MODELVIEW);
   glLoadIdentity();
   display(); // refresh window.
}

void onMoveDown( int x, int y ) {
   // Write your codes here.


   
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove( int x, int y ) {
   // Write your codes here.


   
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}
bool testImpact(cannonball_t fire, pirate_t p) {
    float dx = p.pos.x - fire.pos.x; // test the coordinates of the pirate and the cannonball and return true if it is less than the hit mark of pirate.
    float dy = p.pos.y - fire.pos.y;
    float d = sqrt(dx*dx + dy * dy);
    if (d <= p.radius)
        return true;
    else
        return false;
}

#if TIMER_ON == 1
void onTimer( int v ) {
    
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   // Write your codes here.
    
    if (state == GAME) {
        if (right) {
                rotateCannon(&cannon, -3); // turn the players angle to the right
            }
        
        if (left) {
            rotateCannon(&cannon, 3); // turn the players angle to the left
            }
        if (up) {
            if (platform.pos2.y < 250 && platform.pos2.y > -300) { // give limitation for the platform's height
                platform.pos2.y += 5; // increment the height of the platform and the cannon itself
                cannon.pos.y += 5;
            }
            else {
                platform.pos2.y -= 5; // when on limitation, change the height so that you will stiil be in the loop
                cannon.pos.y -= 5;
            }
        }
        if (down) {
            if (platform.pos2.y < 250 && platform.pos2.y > -300) { // give limitation for the platform's height
                platform.pos2.y -= 5; // decrement the height of the platform and the cannon itself
                cannon.pos.y -= 5;
            }
            else {
                platform.pos2.y += 10; // when on limitation, change the height so that you will stiil be in the loop
                cannon.pos.y += 10;
            }
        }
        if (spacebar && fire_rate == 0) { // find available fire and initialize the ultiple cannonballs
                int availFire = fireCheck();
                if (availFire != -1) {
                    ammo[availFire].pos = cannon.pos;
                    ammo[availFire].angle =cannon.angle;
                    ammo[availFire].active = true;
                    fire_rate = FIRE_RATE;
                }
            }
        if (fire_rate > 0) fire_rate--; // decrement the fire rate to limit the ammo
        for (int i = 0; i< MAX_FIRE; i++) {
                if (ammo[i].active) {
                    ammo[i].pos.x += 10 * cos(ammo[i].angle * D2R); // fire according to the cannon's angle and do not change course
                    ammo[i].pos.y += 10 * sin(ammo[i].angle * D2R); // when cannon's location or angle changes

                    if (ammo[i].pos.x > 550 || ammo[i].pos.x < -550 || ammo[i].pos.y>550 || ammo[i].pos.y < -550) {
                        ammo[i].active = false; // when outside of the limits, cannonball will reset
                    }

                    if (testImpact(ammo[i], pirate)) {
                        ammo[i].active = false; // when outside of the limits, cannonball will reset
                        alterTargetLoc(); // if hit, reset the pirate
                        score++; // increment the score
                    }
                }
            }
        
        pirate.pos.x -= pirate.speed; // move the pirate to the left
            if (pirate.pos.x < -550) { // if not hit, reset the pirate without incrementing the score's value
                alterTargetLoc();
            }
            if (timer > 0)
            {
                timer -= 0.03; // start counting the time according to the initialized ms
            }
       
    }
   // to refresh the window it calls display() function
   glutPostRedisplay() ; // display()

}
#endif

void Init() {
   
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    alterTargetLoc(); // call the function again to initialize
}

int main( int argc, char *argv[] ) {
   glutInit(&argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   //glutInitWindowPosition(100, 100);
   glutCreateWindow("(1000 x 800) Pirate Obliterator");

   glutDisplayFunc(display);
   glutReshapeFunc(onResize);

   //
   // keyboard registration
   //
   glutKeyboardFunc(onKeyDown);
   glutSpecialFunc(onSpecialKeyDown);

   glutKeyboardUpFunc(onKeyUp);
   glutSpecialUpFunc(onSpecialKeyUp);

   //
   // mouse registration
   //
   glutMouseFunc(onClick);
   glutMotionFunc(onMoveDown);
   glutPassiveMotionFunc(onMove);
   
   #if  TIMER_ON == 1
   // timer event
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   #endif

   Init();
   
   glutMainLoop();
}

