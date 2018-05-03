

//#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glut.h>  
#include <GL/glu.h>

#include "neural.cpp"

using namespace std;

typedef struct sq{
	int x;
	int y;
	int mx;
	int my;
	struct sq *nexploration_ratet;
} sq;

sq *snake_player= NULL;
sq *snake_ai = NULL;

neural *net;

// configuring NN
int num_layers = 2;
int num_inputs = 6;
int num_outputs = 1;
int iterations = 0;
float learning_rate = 1e-7;

int	  exploration_rate = 40;

int food_x = -6;
int food_y = -6;

int food_x2 = -6;
int food_y2 = -6;

int cnt = 0; 
int dir;
int num = 7;
int Score;
int Score0;

int mx;
int my;
int mx0;
int my0;

int fail_count = 0;
int sc = 0;
int fail_count0 = 0;
int sc0 = 0;

float old_q = 0.0;
int tmp = 50;

bool down = false;

int key1 = 4;

int dir_user;

int Scale = 25;
int N = 50, M = 30;
int SCREENW = Scale * N;
int SCREENH = Scale * M;

char sScore[15];
char sScore0[15];
char sSpeed[15];

bool dead_snake=false;
bool dead_snake0=false;

void drawString(float x,float y,float z,void *font,const char *string)
{
	const char *c;
	glRasterPos3f(x, y,z); 

	for (c=string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c); 
	}
}

void draw_string(void *font, const char* string)
{
    while(*string)
        glutStrokeCharacter(font, *string++);
}

void add_player(int x, int y){
	sq *tmp = (sq *)malloc(sizeof(sq)); 
	tmp -> x = x;
	tmp -> y = y;
	tmp -> mx = 1;
	tmp -> my = 0;
	tmp -> nexploration_ratet = snake_player;
	snake_player = tmp;
}

void add_ai(int x, int y){
	sq *tmp0 = (sq *)malloc(sizeof(sq)); 
	tmp0 -> x = x;
	tmp0 -> y = y;
	tmp0 -> mx = 1;
	tmp0 -> my = 0;
	tmp0 -> nexploration_ratet = snake_ai;
	snake_ai = tmp0;
}

void start_player(){
	snake_player = NULL;
    add_player(0, 0);
    add_player(1, 0);
    add_player(2, 0);
    add_player(3, 0);
    add_player(4, 0);
	mx = 1;
	my = 0;
}

void start_ai(){
	snake_ai = NULL;
    add_ai(0, 0);
    add_ai(1, 0);
    add_ai(2, 0);
    add_ai(3, 0);
    add_ai(4, 0);
	mx0 = 1;
	my0 = 0;
}

void set_f_player(){ 
	bool f = true;
	while(f){
		srand(time(NULL));
		food_x2 = (rand() % 34) - 17;
		srand(time(NULL));
		food_y2 = (rand() % 34) - 17;

		sq *p = snake_player; 

		while(p != NULL){ 
			if(p -> x == food_x2 && p -> y == food_y2){ 
				f = true;
				break;
			}
			f = false;
			p = p -> nexploration_ratet;
		}
	}
}

void set_f_ai(){ 
	bool f = true;
	while(f){
		srand(time(NULL));
		food_x = (rand() % 34) - 17;
		srand(time(NULL));
		food_y = (rand() % 34) - 17;

		sq *p2 = snake_ai; 
		while(p2 != NULL){ 
			if(p2 -> x == food_x && p2 -> y == food_y){ 
				f = true;
				break;
			}
			f = false;
			p2 = p2 -> nexploration_ratet;
		}
	}
}

/* Initialize OpenGL Graphics */
void init(){
	//glMatrixMode (GL_PROJECTION);
}

float check(int x, int y){
	sq *p = snake_player;
	cnt = 0;
	while(p != NULL){
		if( p -> y == y && p -> x == x) 
			return -1.0f; 
		p = p -> nexploration_ratet;
		cnt += 1;
		
		if( cnt == sc*2-3)
			return -1.0f;
	}
	if(x > 18 || x < -18 || y > 18 || y < -18) return -1.0f; 
	return 1.0; 
}

float *get_q(int sx, int sy){
	float inputs[6];
	inputs[0] = sqrt((sx - food_x2) * (sx - food_x2) + (sy - food_y2) * (sy - food_y2)); 
	inputs[1] = check(sx	, sy	); 
	inputs[2] = check(sx + 1, sy	); 
	inputs[3] = check(sx    , sy + 1); 
	inputs[4] = check(sx - 1, sy); 
	inputs[5] = check(sx    , sy - 1); 
	return net -> feed(inputs); 
}

void rev_player(){
	sq *snake2 = NULL;
	sq *p = snake_player;
	while(p != NULL){
		sq *tmp = (sq *)malloc(sizeof(sq));
		tmp -> x = p -> x;
		tmp -> y = p -> y;
		tmp -> mx = -1 * p -> mx; 
		tmp -> my = -1 * p -> my; 
		tmp -> nexploration_ratet = snake2; 
		snake2 = tmp;
		sq *x = p -> nexploration_ratet;
		free(p); 
		p = x; 
	}
	snake_player = snake2;
	mx = snake_player -> mx;
	my = snake_player -> my;
}

void rev_ai(){
	sq *snake3 = NULL;
	sq *p = snake_ai;
	while(p != NULL){
		sq *tmp0 = (sq *)malloc(sizeof(sq));
		tmp0 -> x = p -> x;
		tmp0 -> y = p -> y;
		tmp0 -> mx = -1 * p -> mx;
		tmp0 -> my = -1 * p -> my;
		tmp0 -> nexploration_ratet = snake3;
		snake3 = tmp0;
		sq *x = p -> nexploration_ratet;
		free(p);
		p = x;
	}
	snake_ai = snake3;
	mx0 = snake_ai -> mx;
	my0 = snake_ai -> my;
}

float max_q(int sx, int sy, int food_x, int food_y){

	float new_q = 0.0;

	int sx1 = sx + 1;
	int sy1 = sy;
    float *out1 = get_q(sx1, sy1);

    sx1 = sx - 1;
	sy1 = sy;
	float *out2 = get_q(sx1, sy1);

 	sx1 = sx;
	sy1 = sy + 1;
	float *out3 = get_q(sx1, sy1);

	sx1 = sx;
	sy1 = sy - 1;
	float *out4 = get_q(sx1, sy1);

    if(out1[0] > out2[0]){
		if(out1[0] > out3[0]){
			if(out1[0] > out4[0]){
				new_q = out1[0];
				if(mx == -1) rev_player();
				else{
					mx =  1;
					my =  0;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev_player();
				else{
					mx =  0;
					my = -1;
				}
			}
		}else{
			if(out3[0] > out4[0]){
				new_q = out3[0];
				if(my == -1) rev_player();
				else{
					mx =  0;
					my =  1;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev_player();
				else{
					mx =  0;
					my = -1;
				}
			}
		}
	}else{
		if(out2[0] > out3[0]){
			if(out2[0] > out4[0]){
				new_q = out2[0];
				if(mx == 1) rev_player();
				else{
					mx = -1;
					my =  0;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev_player();
				else{
					mx =  0;
					my = -1;
				}
			}
		}else{
			if(out3[0] > out4[0]){
				new_q = out3[0];
				if(my == -1) rev_player();
				else{
					mx =  0;
					my =  1;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev_player();
				else{
					mx =  0;
					my = -1;
				}
			}
		}
	}
	return new_q;

}

void move_player(){
	sq *p = snake_player;
	int x = p -> x;
	int y = p -> y;
	int tmx = p -> mx;
	int tmy = p -> my;
	while(p -> nexploration_ratet != NULL){
		sq *q = p -> nexploration_ratet;
		int tmp = q -> x;
		q -> x = x;
		x = tmp;

		tmp = q -> y;
		q -> y = y;
		y = tmp;

		tmp = q -> mx;
		q -> mx = tmx;
		tmx = tmp;

		tmp = q -> my;
		q -> my = tmy;
		tmy = tmp;

		p = p -> nexploration_ratet;
	}
	snake_player -> mx = mx;
	snake_player -> my = my;
	snake_player -> x += mx;
	snake_player -> y += my;
}

void move_ai(){
	sq *p = snake_ai;
	int x = p -> x;
	int y = p -> y;
	int tmx = p -> mx;
	int tmy = p -> my;
	while(p -> nexploration_ratet != NULL){
		sq *q = p -> nexploration_ratet;
		int tmp = q -> x;
		q -> x = x;
		x = tmp;

		tmp = q -> y;
		q -> y = y;
		y = tmp;

		tmp = q -> mx;
		q -> mx = tmx;
		tmx = tmp;

		tmp = q -> my;
		q -> my = tmy;
		tmy = tmp;

		p = p -> nexploration_ratet;
	}
	snake_ai -> mx = mx0;
	snake_ai -> my = my0;
	snake_ai -> x += mx0;
	snake_ai -> y += my0;
}


bool tail_player(){
	sq *p = snake_player;
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == snake_player -> x && p -> nexploration_ratet -> y == snake_player -> y)
			return true;
		p = p -> nexploration_ratet;
	}
	return false;
}

bool tail_ai(){
	sq *p = snake_ai;
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == snake_ai -> x && p -> nexploration_ratet -> y == snake_ai -> y)
			return true;
		p = p -> nexploration_ratet;
	}
	return false;
}

float reward_player(int sx, int sy, int sx1, int sy1){
	if(snake_player -> x == food_x2 && snake_player -> y == food_y2){
			fail_count = 0;
			exploration_rate = exploration_rate / 3;
        add_player(food_x2, food_y2);
        set_f_player();
		sc++;
		return 1000.0;
	}else if(tail_player()){
				fail_count = 0;
				fail_count++;
		sc = 0;
		cout<<"AI dead"<<endl;
		dead_snake=true;
        start_player();
		return -100000.0f;
	}else if(snake_player -> x > 17 || snake_player -> x < -17 || snake_player -> y > 17 || snake_player -> y < -17){
		sc = 0;
		cout<<"AI dead"<<endl;
		dead_snake=true;
        start_player();
		fail_count++;
		return -1000.0f;
	}
	if(fail_count > 50){
		exploration_rate = 20;
	}
	float re2 = sqrt((sx1 - food_x2) * (sx1 - food_x2) + (sy1 - food_y2) * (sy1 - food_y2));
	return -re2;
}

float reward_ai(int sx, int sy, int sx1, int sy1){
	if(snake_ai -> x == food_x && snake_ai -> y == food_y){
        add_ai(food_x, food_y);
        set_f_ai();
		sc0++; 
		return 1000.0;
	}else if(tail_ai()){
		sc0 = 0;
		cout<<"Player dead"<<endl;
		dead_snake0=true;
        start_ai(); 
		return -100000.0f;
	}else if(snake_ai -> x > 17 || snake_ai -> x < -17 || snake_ai -> y > 17 || snake_ai -> y < -17){
		sc0 = 0;
		cout<<"Player dead"<<endl;
		dead_snake0=true;
        start_ai();
		fail_count0++;
		return -1000.0f;
	}
	if(fail_count0 > 50){
		exploration_rate = 20;
	}
	float re2 = sqrt((sx1 - food_x) * (sx1 - food_x) + (sy1 - food_y) * (sy1 - food_y));
	return -re2;
}

void Tick(){
	iterations++;
    int sx = snake_ai -> x;
	int sy = snake_ai -> y;

	int sx1 = sx;
	int sy1 = sy;

    switch (dir_user) {
        case 0:
            sx1 = sx;
			sy1 = sy + 1;
			if(my0 == -1) rev_ai();
			else{
				mx0 = 0;
				my0 = 1;
			}
            break;
        case 1:
            sx1 = sx - 1;
			sy1 = sy;
			if(mx0 == 1) rev_ai();
			else{
				mx0 = -1;
				my0 = 0;
			}
            break;
        case 2:
            sx1 = sx + 1;
			sy1 = sy;
			if(mx0 == -1) rev_ai();
			else{
				mx0 = 1;
				my0 = 0;
			}
            break;
        case 3:
            sx1 = sx;
			sy1 = sy - 1;
			if(my0 == 1) rev_ai();
			else{
				mx0 = 0;
				my0 = -1;
			}
            break;
    }

	if(!dead_snake0){
        move_ai();

    sx1 = snake_ai -> x;
	sy1 = snake_ai -> y;

	float re = reward_ai(sx, sy, sx1, sy1);
	}
}

void itera(){
	iterations++;
	int sx = snake_player -> x;
	int sy = snake_player -> y;

	float inputs[6];
	int sx1 = sx;
	int sy1 = sy;

	float new_q;
	if(rand() % 100 > exploration_rate){
		new_q = max_q(sx, sy, food_x2, food_y2);
	}else{
		int a = rand() % 4;
		if(a == 0){
			sx1 = sx + 1;
			sy1 = sy;
			if(mx == -1) rev_player();
			else{
				mx =  1;
				my =  0;
			}
		}else if(a == 1){
			sx1 = sx - 1;
			sy1 = sy;
			if(mx == 1) rev_player();
			else{
				mx = -1;
				my =  0;
			}
		}else if(a == 2){
			sx1 = sx;
			sy1 = sy + 1;
			if(my == -1) rev_player();
			else{
				mx =  0;
				my =  1;
			}
		}else{
			sx1 = sx;
			sy1 = sy - 1;
			if(my == 1) rev_player();
			else{
				mx =  0;
				my = -1;
			}
		}

		float *out1 = get_q(sx1, sy1);
		new_q = out1[0];
	}
	get_q(sx1, sy1);
    if(!dead_snake){
        move_player();

    sx1 = snake_player -> x;
	sy1 = snake_player -> y;
	}

    float dout[1];
	float re = reward_player(sx, sy, sx1, sy1);

	dout[0] =  re + 0.9 * new_q - old_q;
	net -> learn(dout);
	old_q = new_q;
}

void par(float x1, float x2, float y1, float y2, float z1, float z2){

	glBegin(GL_POLYGON); 

	glColor3f(1.0,1.0,0.0);
	glVertex3f(x1, y1, z1);
	glColor3f(1.0,1.0,0.0);
	glVertex3f(x2, y1, z1);
	glColor3f(0.0,0.0,1.0);
	glVertex3f(x2, y2, z1);
	glColor3f(0.0,0.0,1.0);
	glVertex3f(x1, y2, z1);

	glEnd();
}

void pardead(float x1, float x2, float y1, float y2, float z1, float z2){

	glBegin(GL_POLYGON);

	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y1, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y1, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y2, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y2, z1);

	glEnd();
}

void parfood(float x1, float x2, float y1, float y2, float z1, float z2){

	glBegin(GL_POLYGON);

	glColor3f(0.5,0.5,0.5);
	glVertex3f(x1, y1, z1);
	glColor3f(0.5,0.5,0.5);
	glVertex3f(x2, y1, z1);
	glColor3f(0.5,0.5,0.5);
	glVertex3f(x2, y2, z1);
	glColor3f(0.5,0.5,0.5);
	glVertex3f(x1, y2, z1);

	glEnd();
}

void parb(float x1, float x2, float y1, float y2, float z1, float z2){

	glBegin(GL_POLYGON);

	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y1, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y1, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y2, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y2, z1);

	glEnd();
}

void DrawRules(){
        glColor3f(0.0,0.0,0.0);

		glBegin(GL_POLYGON);
		glVertex3f(0.0,0.0,0.0);
		glColor3f(0.0,0.0,0.0);
		glVertex3f(100.0,0.0,0.0);
		glColor3f(0.0,0.4,0.0);
		glVertex3f(100.0,100.0,0.0);
		glVertex3f(0.0,100.0,0.0);
		glEnd();

		glPushMatrix();
		glScalef(0.8,0.8,0);
		glPopMatrix();

		glColor3f(0.4,0.6,0.4);
		glRectf(20.0,20.0,80.0,80.0);
		glColor3f(0.8,0.8,0.8);
		glRectf(21.0,21.0,79.0,79.0);
		glColor3f(0.4,0.6,0.4);
		glRectf(40,5,60,10);
		glColor3f(0.8,0.8,0.8);
		glRectf(40.5,5.5,59.5,9.5);

		glColor3f(0.0,0.4,0.0);
		drawString(46,6,0,GLUT_BITMAP_TIMES_ROMAN_24,"BACK");

		glColor3f(0.0,0.4,0.0);
		drawString(42,74,0,GLUT_BITMAP_TIMES_ROMAN_24,"HOW TO PLAY:");
		drawString(23,68,0,GLUT_BITMAP_HELVETICA_18,"Click PLAY on the main menu to reach the game screen");
		drawString(23,64,0,GLUT_BITMAP_HELVETICA_18,"Click INSTRUCTIONS on the main screen to reach this screen");
		drawString(23,60,0,GLUT_BITMAP_HELVETICA_18,"Click QUIT on the main screen to exit the menu");
		drawString(23,56,0,GLUT_BITMAP_HELVETICA_18,"After entering the game screen:");
		drawString(23,52,0,GLUT_BITMAP_HELVETICA_18,"- The right window is neural AI player");
		drawString(23,48,0,GLUT_BITMAP_HELVETICA_18,"- The left window is Human player");
		drawString(23,44,0,GLUT_BITMAP_HELVETICA_18,"- Food is a grey spot which appears randomly on the screens");
		drawString(23,40,0,GLUT_BITMAP_HELVETICA_18,"- If snake_player hits it's body or boundary:restart");
		drawString(23,36,0,GLUT_BITMAP_HELVETICA_18,"- Use the arrow keys to control snake_player's movement,'r' for replay");
		drawString(23,32,0,GLUT_BITMAP_HELVETICA_18,"- AI vs Human, the higher score wins");
		drawString(42,25,0,GLUT_BITMAP_HELVETICA_18," ENJOY THE GAME");

		glutPostRedisplay();
}

void welcome(){
	glMatrixMode(GL_PROJECTION);
	glColor3f(0.0,0.0,0.0);  

	glBegin(GL_POLYGON);
	glVertex3f(0.0,0.0,0.0);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(100.0,0.0,0.0);
	glColor3f(0.0,0.4,0.0);
	glVertex3f(100.0,100.0,0.0);
	glVertex3f(0.0,100.0,0.0);
	glEnd();

	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,39.5,60.5,45.5);

	glColor3f(0.8,0.8,0.8);
	glRectf(40,40,60,45);
	glColor3f(0.0,0.2,0.0);
	drawString(43.5,42,0,GLUT_BITMAP_HELVETICA_18,"MAN vs AI MODE");

	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,29.5,60.5,35.5);

	glColor3f(0.8,0.8,0.8);
	glRectf(40,30,60,35);
	glColor3f(0.0,0.2,0.0);
	drawString(42,31,0,GLUT_BITMAP_HELVETICA_18,"SINGLE PLAYER MODE");

	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,19.5,60.5,25.5);

	glColor3f(0.8,0.8,0.8);
	glRectf(40,20,60,25);
	glColor3f(0.0,0.2,0.0);
	drawString(44,21,0,GLUT_BITMAP_HELVETICA_18,"INSTRUCTIONS");

	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,9.5,60.5,15.5);
	glColor3f(0.8,0.8,0.8);
	glRectf(40,10,60,15);
	glColor3f(0.0,0.2,0.0);
	drawString(48,11,0,GLUT_BITMAP_HELVETICA_18,"QUIT");


	glPushMatrix();

	glColor3f(0.6,0.7,0.6); 
	drawString(38,85,0,GLUT_BITMAP_TIMES_ROMAN_24,"ECE 6122 FINALPROJECT ");
	drawString(28,73,0,GLUT_BITMAP_TIMES_ROMAN_24,"SnakeBee: A Neural Network - Human vs AI Snake Game");
	drawString(22,64,0,GLUT_BITMAP_TIMES_ROMAN_24,"The Snake pattern is inspired by GaTechs own Buzz the Bee. Go Jackets!");
	glPopMatrix();
	glColor3f(0.137,0.137,0.556);

}

void DrawScore()
{
	glLineWidth(1.5f);
    glColor3f (1.0,1.0,1.0);

    glPushMatrix();
    glTranslatef(SCREENW/(18.4), SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, "Your score:");
    glPopMatrix();

    sprintf(sScore, "%9d", sc0);

    glPushMatrix();
    glTranslatef(SCREENW/18.3, SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, sScore);
    glPopMatrix();

	glPushMatrix();
    glTranslatef(SCREENW/(2.45), SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, "Speed:");
    glPopMatrix();

	sprintf(sSpeed, "%d", tmp);

    glPushMatrix();
    glTranslatef(SCREENW/1.9, SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, sSpeed);
    glPopMatrix();

	glPushMatrix();
    glTranslatef(SCREENW/(1.4), SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, "AI's score:");
    glPopMatrix();

    sprintf(sScore0, "%9d", sc);

    glPushMatrix();
    glTranslatef(SCREENW/1.45, SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, sScore0);
    glPopMatrix();

    glFinish();
}

void DrawScoreSingle()
{
	glLineWidth(1.5f);
    glColor3f (1.0,1.0,1.0);

    glPushMatrix();
    glTranslatef(SCREENW/(5.4), SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, "Your score:");
    glPopMatrix();

    sprintf(sScore, "%9d", sc0);

    glPushMatrix();
    glTranslatef(SCREENW/5, SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, sScore);
    glPopMatrix();

	glPushMatrix();
    glTranslatef(SCREENW/(1.6), SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, "Your Speed:");
    glPopMatrix();

	sprintf(sSpeed, "%d", tmp);

    glPushMatrix();
    glTranslatef(SCREENW/1.2, SCREENH/(1.05), 0);
    glScalef(0.3f, 0.3f, 0.3f);
    draw_string(GLUT_STROKE_ROMAN, sSpeed);
    glPopMatrix();

    glFinish();
}

void DrawUser(){
	
	glClearColor(0.0, 0.18, 0.0, 0);
	glLoadIdentity();
	gluOrtho2D (0, SCREENW, 0, SCREENH);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POLYGON);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (0.0, 800.0, 0.0);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (0, 700.0, 0.0);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (1400.0, 700.0, 0.0);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (1400.0, 800.0, 0.0);

	glEnd();

	DrawScoreSingle();
	
	glLoadIdentity ();
	gluPerspective(45.0, (float)SCREENW/(float)SCREENH, 0.1f, 200.0);
	glTranslatef(0.0, 0.0, -28.0);
	
	sq *p = snake_ai;
	parb(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	parb(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	parb(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	parb( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);

	if(dead_snake0){
	while(p != NULL){
		pardead((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0);
		p = p -> nexploration_ratet;
	}
	///// START OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
	glPushMatrix();
	glColor3f (0.8, 0.0, 0.0);
    glTranslatef(-8.0,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "Snake Dead :(");
	glColor3f (1.0, 1.0, 1.0);
    glTranslatef(-1100.0, 1000.0, -28.0);
    glScalef(2.0f, 2.0f, 2.0f);
    draw_string(GLUT_STROKE_ROMAN, "YOU LOSE");
    glPopMatrix();
	///// END OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
	}
	else{
	while(p != NULL){
		par((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0);
		p = p -> nexploration_ratet;
	}
	}
	parfood(food_x/2.0, food_x/2.0 + 0.4 , food_y/2.0 , food_y/2.0 + 0.4, 0.0 , 0.0);

	glPushMatrix();
	glColor3f (0.0, 0.4, 0.0);
    glTranslatef(-2,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "Back");
    glPopMatrix();
}

void DrawPlay(){
	
	glClearColor(0.0, 0.18, 0.0, 0); 
	glLoadIdentity();
	gluOrtho2D (0, SCREENW, 0, SCREENH);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POLYGON);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (0.0, 800.0, 0.0);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (0, 700.0, 0.0);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (1400.0, 700.0, 0.0);
	glColor3f (0.0, 0.0, 0.0);
	glVertex3f (1400.0, 800.0, 0.0);

	glEnd();

	DrawScore();

	glLoadIdentity ();
	gluPerspective(45.0, (float)SCREENW/(float)SCREENH, 0.1f, 200.0);
	glTranslatef(-10.0, 0.0, -28.0);
	
	
	sq *p = snake_ai;
	
	parb(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	parb(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	parb(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	parb( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);

	if(dead_snake0){
	while(p != NULL){
		pardead((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0);
		p = p -> nexploration_ratet;
	}
	}
	else{
		while(p != NULL){
		par((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0);
		p = p -> nexploration_ratet;
	}
	}
	parfood(food_x/2.0, food_x/2.0 + 0.4 , food_y/2.0 , food_y/2.0 + 0.4, 0.0 , 0.0);

	glLoadIdentity ();
    gluPerspective(45.0, (float)SCREENW/(float)SCREENH, 0.1f, 200.0);

	glTranslatef(9.3, 0.0, -28.0);
	sq *p2 = snake_player;
	parb(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	parb(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	parb(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	parb( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);
	if(dead_snake){
	while(p2 != NULL){
		pardead((p2 -> x)/2.0,(p2 -> x)/2.0 + 0.4,(p2 -> y)/2.0,(p2 -> y)/2.0 + 0.4, 0.0, 0.0);
		p2 = p2 -> nexploration_ratet;
	}
	}
	else{
		while(p2 != NULL){
		par((p2 -> x)/2.0,(p2 -> x)/2.0 + 0.4,(p2 -> y)/2.0,(p2 -> y)/2.0 + 0.4, 0.0, 0.0);
		p2 = p2 -> nexploration_ratet;
	}
	}
	parfood(food_x2/2.0, food_x2/2.0 + 0.4 , food_y2/2.0 , food_y2/2.0 + 0.4, 0.0 , 0.0);

	if(dead_snake0 && !dead_snake){
	glPushMatrix();
	glColor3f (0.8, 0.0, 0.0);
    glTranslatef(-42.0,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "Player Snake Dead :(");
	glColor3f (1.0, 1.0, 1.0);
    glTranslatef(-500.0, 1000.0, -28.0);
    glScalef(2.0f, 2.0f, 2.0f);
    draw_string(GLUT_STROKE_ROMAN, "YOU LOSE");
    glPopMatrix();
	}

	if(dead_snake && !dead_snake0){
    glPushMatrix();
	glColor3f (0.8, 0.0, 0.0);
    glTranslatef(1.0,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "AI Snake Dead :(");
	glColor3f (1.0, 1.0, 1.0);
    glTranslatef(-2300.0, 1000.0, -28.0);
    glScalef(2.0f, 2.0f, 2.0f);
    draw_string(GLUT_STROKE_ROMAN, "YOU WIN!");
    glPopMatrix();
	}
	
///// END OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
	glPushMatrix();
	glColor3f (0.0, 0.4, 0.0);
    glTranslatef(-12.6,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "Back");
    glPopMatrix();

}

void display(){

	switch (key1)
    {
		case 1:
		    DrawPlay();
            break;
        case 2:
            DrawUser();
			break;
    
        case 4:
            welcome();
            break;
        case 5:
            DrawRules();
            break;
    }
    glFlush(); 
	glutSwapBuffers(); 
}


void specialKeyboard(int key, int a, int b) {
	switch(key) {
		case GLUT_KEY_UP:
			dir_user = 0;
			break;
		case GLUT_KEY_RIGHT:
			dir_user = 2;
			break;
		case GLUT_KEY_LEFT:
			dir_user = 1;
			break;
		case GLUT_KEY_DOWN:
			dir_user = 3;
			break;
	}
}


void keyboard(unsigned char key, int a, int b) {
	switch (key) {
        case 'w': 
            dir = 0;
            break;
        case 'd': 
            dir = 2;
            break;
        case 'a': 
            dir = 1;
            break;
        case 's': 
            dir = 3;
            break;
        case 'f': 
            set_f_player();
            break;
        case 'o':
            tmp--;
            break;
        case 'p': 
			if(tmp < 0) tmp = 0;
            tmp++;
            break;
		case 'r': 
            start_player();
			sc =0; tmp = 50;
            break;
        case 27:
            exit(0);
            break;
    }
}

void mouse(int button, int state, int ax, int ay)            
{
	int mx=ax*100/SCREENW,my=(SCREENH-ay)*100/SCREENH;		
	
	down= button==GLUT_LEFT_BUTTON && state==GLUT_LEFT;
	if(down)
    {
        if (key1==4) 
        {
            if(mx > (40) && mx < (60) && my > (10) && my < (15) )  
            {
                exit(0);
            }
            if(mx > (40) && mx < (60) && my > (20) && my < (25) ) 
            {
                glClear(GL_COLOR_BUFFER_BIT);
                key1=5;
				display();
            }
            if(mx > (40) && mx < (60) && my > (40) && my < (45) ) 
            {
                key1 = 1;
                num = 5;
                Score = 0;
				Score0 = 0;
                display();
            }
			if(mx > (40) && mx < (60) && my > (30) && my < (35) ) 
            {
                key1 = 2;
                display();
            }
        }
        if (key1==5) 
        {
            if(mx > (40) && mx < (60) && my > (5) && my < (10) )
            {
                key1=4;
                glClear(GL_COLOR_BUFFER_BIT);
                welcome();
            }
        }
    }
    glutMouseFunc(mouse); 
}

void timer(int = 0){
	if (key1==1){
        itera();
		
	}
	if (key1==2){
	 	Tick();
	}
		
	glutPostRedisplay(); 
	glutTimerFunc(tmp, timer, 0); 
}

void timer2(int = 0){
	if (key1==1){
        Tick();
		
	}
	glutPostRedisplay(); 
	glutTimerFunc(tmp, timer2, 0); 
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h); 
	
    glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	if (key1 == 4){
		glOrtho(0.0, 100.0, 0.0, 100.0,	-5.0 , 10.0);
	}

}

int main(int argc, char** argv){
    

	srand(time(NULL));
	net = new neural(num_inputs, num_outputs, num_layers, 10, learning_rate); 
	net -> init(); 

    start_player();
    start_ai();
    set_f_player();
    set_f_ai(); 

    glutInit(&argc, argv); // Initialize GLUT

    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize (SCREENW,SCREENH);  
    glutInitWindowPosition(500, 0); 
    glutCreateWindow("SnakeBee: Human vs AI Snake game"); 

    init();

	glutTimerFunc (400,timer,0);     
	glutTimerFunc (400,timer2,0);
	glutReshapeFunc(myReshape);       
    glutKeyboardFunc(keyboard);   
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);  

	glutDisplayFunc(display);

    glutMainLoop();           
    
    return 0;
}
