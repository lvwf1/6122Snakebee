/*
 * ECE6122 Project
 * Author: 
 */

//#include <windows.h>  // for MS Windows
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
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

sq *snake = NULL;
sq *snake0 = NULL;
neural *net;

int num_layers = 2;
int num_inputs = 6;
int num_outputs = 1;

float learning_rate = 1e-7;

//bool  pus        =  false; //push button for keyboard hit

int iterations = 0;

int	  exploration_rate = 40;

int food_x = -6;
int food_y = -6;

int food_x2 = -6;
int food_y2 = -6;

int cnt = 0; // Check Body

int mx;
int my;
int mx0;
int my0;

int fail_count = 0;
int sc = 0;
int fail_count0 = 0;
int sc0 = 0;

float old_q = 0.0;
float old_q0 = 0.0;
int tmp = 50;
int tmp0 = 50;

//int wa,ha;

//int SCREENH=(25*30),SCREENW=(25*50);
//int SCREENH=450,SCREENW=450;

bool down = false;

int key1 = 4;

bool neural_check = false;

int dir;
int dir_user;

int Scale = 25;
int N = 50, M = 30;
int SCREENW = Scale * N;
int SCREENH = Scale * M;

int num = 7;

char sScore[15];
char sScore0[15];
char sSpeed[15];
int Score = 0;
int Score0 = 0;

bool dead_snake=false;
bool dead_snake0=false;

void drawString(float x,float y,float z,void *font,const char *string)
{
	const char *c;
	glRasterPos3f(x, y,z); //Specifies the raster position for pixel operations.

	for (c=string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c); //renders a bitmap character using OpenGL
	}
}

void draw_string(void *font, const char* string)
{
    while(*string)
        glutStrokeCharacter(font, *string++);
}

void add(int x, int y){
	sq *tmp = (sq *)malloc(sizeof(sq)); // Start sq snake tmp with Memory Allocation
	tmp -> x = x;
	tmp -> y = y;
	tmp -> mx = 1;
	tmp -> my = 0;
	tmp -> nexploration_ratet = snake;
	snake = tmp;
}

void add0(int x, int y){
	sq *tmp0 = (sq *)malloc(sizeof(sq)); // Start sq snake tmp with Memory Allocation
	tmp0 -> x = x;
	tmp0 -> y = y;
	tmp0 -> mx = 1;
	tmp0 -> my = 0;
	tmp0 -> nexploration_ratet = snake0;
	snake0 = tmp0;
}

void start(){
	snake = NULL;
    add(0, 0);
	add(1, 0);
	add(2, 0);
	add(3, 0);
	add(4, 0);
	mx = 1;
	my = 0;
}

void start0(){
	snake0 = NULL;
    add0(0, 0);
	add0(1, 0);
	add0(2, 0);
	add0(3, 0);
	add0(4, 0);
	mx0 = 1;
	my0 = 0;
}

void pause(){
	snake = NULL;
}

void pause0(){
	snake0 = NULL;
}



void set_f(){ //Setup food x,y cordinate and then make the snake p to move towards it's cordinate
	bool f = true;
	while(f){
		srand(time(NULL));
		food_x2 = (rand() % 34) - 17;
		srand(time(NULL));
		food_y2 = (rand() % 34) - 17;
		
		//food_x2 = food_x;
		//food_y2 = food_y;

		sq *p = snake; //take add(4,0) value

		while(p != NULL){ //fetch null 
			if(p -> x == food_x2 && p -> y == food_y2){ //if value of p is same as food
				f = true;
				break;
			}
			f = false;
			p = p -> nexploration_ratet;
		}

		// sq *p2 = snake0; //take add(4,0) value
		
		// while(p2 != NULL){ //fetch null 
		// 	if(p2 -> x == food_x2 && p2 -> y == food_y2){ //if value of p is same as food
		// 		f = true;
		// 		break;
		// 	}
		// 	f = false;
		// 	p2 = p2 -> nexploration_ratet;
		// }
	}
}

void set_f0(){ //Setup food x,y cordinate and then make the snake p to move towards it's cordinate
	bool f = true;
	while(f){
		srand(time(NULL));
		food_x = (rand() % 34) - 17;
		srand(time(NULL));
		food_y = (rand() % 34) - 17;

		sq *p2 = snake0; //take add(4,0) value
		while(p2 != NULL){ //fetch null 
			if(p2 -> x == food_x && p2 -> y == food_y){ //if value of p is same as food
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
	sq *p = snake;
	while(p != NULL){
		if( p -> y == y && p -> x == x) //Check for each case until p's x,y is equal to x,y
			return -1.0; //return just 1 step form food
		p = p -> nexploration_ratet;
		cnt += 1;
		if( cnt == 20)
			return -1.0;
	}
	if(x > 18 || x < -18 || y > 18 || y < -18) return -1.0; //Border Case Decrement
	return 1.0; //reached food
}

float *get_q(int sx, int sy){
	float inputs[6];
	inputs[0] = sqrt((sx - food_x2) * (sx - food_x2) + (sy - food_y2) * (sy - food_y2)); // Root (x)^2 + (y)^2
	inputs[1] = check(sx	, sy	); // no increment
	inputs[2] = check(sx + 1, sy	); //Check x increment
	inputs[3] = check(sx    , sy + 1); //Check y increment
	inputs[4] = check(sx - 1, sy); //Check x decrement
	inputs[5] = check(sx    , sy - 1); //Check y decrement
	return net -> feed(inputs); //Send Feed inputs
}

void rev(){
	sq *snake2 = NULL;
	sq *p = snake;
	while(p != NULL){
		sq *tmp = (sq *)malloc(sizeof(sq));
		tmp -> x = p -> x;
		tmp -> y = p -> y;
		tmp -> mx = -1 * p -> mx; //reverse case of mx by multiplying -1
		tmp -> my = -1 * p -> my; //reverse case of mx by multiplying -1
		tmp -> nexploration_ratet = snake2; //nexploration_ratet to snake2
		snake2 = tmp;
		sq *x = p -> nexploration_ratet;
		free(p); //Memory Unallocate of P
		p = x; //put p=x
	}
	snake = snake2;
	mx = snake -> mx;
	my = snake -> my;
}

void rev0(){
	sq *snake3 = NULL;
	sq *p = snake0;
	while(p != NULL){
		sq *tmp0 = (sq *)malloc(sizeof(sq));
		tmp0 -> x = p -> x;
		tmp0 -> y = p -> y;
		tmp0 -> mx = -1 * p -> mx; //reverse case of mx by multiplying -1
		tmp0 -> my = -1 * p -> my; //reverse case of mx by multiplying -1
		tmp0 -> nexploration_ratet = snake3; //nexploration_ratet to snake2
		snake3 = tmp0;
		sq *x = p -> nexploration_ratet;
		free(p); //Memory Unallocate of P
		p = x; //put p=x
	}
	snake0 = snake3;
	mx0 = snake0 -> mx;
	my0 = snake0 -> my;
}

float max_q(int sx, int sy, int food_x, int food_y){

	float new_q = 0.0;

	int sx1 = sx + 1; // increment in x-side
	int sy1 = sy;
    float *out1 = get_q(sx1, sy1);

    sx1 = sx - 1; //decrement x-side
	sy1 = sy;
	float *out2 = get_q(sx1, sy1);

 	sx1 = sx;
	sy1 = sy + 1; //increment y-side
	float *out3 = get_q(sx1, sy1);

	sx1 = sx;
	sy1 = sy - 1; //decrement y-side
	float *out4 = get_q(sx1, sy1);

	//Now all conditions for value increment is checked, now compare each outcome values
    if(out1[0] > out2[0]){
		if(out1[0] > out3[0]){
			if(out1[0] > out4[0]){
				new_q = out1[0];
				if(mx == -1) rev();
				else{
					mx =  1;
					my =  0;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}else{
			if(out3[0] > out4[0]){
				new_q = out3[0];
				if(my == -1) rev();
				else{
					mx =  0;
					my =  1;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
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
				if(mx == 1) rev();
				else{
					mx = -1;
					my =  0;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}else{
			if(out3[0] > out4[0]){
				new_q = out3[0];
				if(my == -1) rev();
				else{
					mx =  0;
					my =  1;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}
	}
	return new_q;

}

void move(){
	sq *p = snake;
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
	snake -> mx = mx;
	snake -> my = my;
	snake -> x += mx; //Add with mx value depending un up down left right movement
	snake -> y += my;
    // Setup increment case from x to mx within nexploration_ratet
}

void move0(){
	sq *p = snake0;
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
	snake0 -> mx = mx0;
	snake0 -> my = my0;
	snake0 -> x += mx0; //Add with mx value depending un up down left right movement
	snake0 -> y += my0;
    // Setup increment case from x to mx within nexploration_ratet
}


bool tail(){
	sq *p = snake;
    //tail is touched by head
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == snake -> x && p -> nexploration_ratet -> y == snake -> y)
			return true;
		p = p -> nexploration_ratet;
	}
	return false;
}

bool tail0(){
	sq *p = snake0;
    //tail is touched by head
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == snake0 -> x && p -> nexploration_ratet -> y == snake0 -> y)
			return true;
		p = p -> nexploration_ratet;
	}
	return false;
}

float reward(int sx, int sy, int sx1, int sy1){
	if(snake -> x == food_x2 && snake -> y == food_y2){
		//if (neural_check){
			fail_count = 0;
			exploration_rate = exploration_rate / 3; //Rate decrement by 1/3 from 40%
		//}
		add(food_x2, food_y2);//Add value to tail with additional quad
		set_f();
		sc++; //Increment sc by 1
		return 1000.0;
	}else if(tail()){
			//if (neural_check){
				fail_count = 0;
				fail_count++;
		//}
		sc = 0;
		cout<<"AI dies"<<endl;
		dead_snake=true;
		start(); //restart
		return -100000.0;
	}else if(snake -> x > 17 || snake -> x < -17 || snake -> y > 17 || snake -> y < -17){
		//border hit
		sc = 0;
		cout<<"AI dies"<<endl;
		dead_snake=true;
        start();
		fail_count++;
		return -1000.0;
	}
	if(fail_count > 50){
        //decrease exploration_rate for hish fail_count
		exploration_rate = 20;
	}
	float re2 = sqrt((sx1 - food_x2) * (sx1 - food_x2) + (sy1 - food_y2) * (sy1 - food_y2)); //Root. (x)^2 + (y)^2
	return -re2;
}

float reward0(int sx, int sy, int sx1, int sy1){
	if(snake0 -> x == food_x && snake0 -> y == food_y){
		// if (neural_check){
		// 	fail_count = 0;
		// 	exploration_rate = exploration_rate / 3; //Rate decrement by 1/3 from 40%
		// }
		add0(food_x, food_y);//Add value to tail with additional quad
		set_f0();
		sc0++; //Increment sc by 1
		return 1000.0;
	}else if(tail0()){
		// 	if (neural_check){
		// 		fail_count = 0;
		// 		fail_count++;
		// }
		sc0 = 0;
		cout<<"Player dies"<<endl;
		dead_snake0=true;
		start0(); //restart
		return -100000.0;
	}else if(snake0 -> x > 17 || snake0 -> x < -17 || snake0 -> y > 17 || snake0 -> y < -17){
		//border hit
		sc0 = 0;
		cout<<"Player dies"<<endl;
		dead_snake0=true;
        start0();
		fail_count0++;
		return -1000.0;
	}
	if(fail_count0 > 50){
        //decrease exploration_rate for hish fail_count
		exploration_rate = 20;
	}
	float re2 = sqrt((sx1 - food_x) * (sx1 - food_x) + (sy1 - food_y) * (sy1 - food_y)); //Root. (x)^2 + (y)^2
	return -re2;
}

void Tick(){
	iterations++; //Increment iterations
    int sx = snake0 -> x;
	int sy = snake0 -> y;

	int sx1 = sx;
	int sy1 = sy;

    //Snake head movement:
    switch (dir_user) {
        case 0:
            sx1 = sx;
			sy1 = sy + 1;
			if(my0 == -1) rev0();
			else{
				mx0 = 0;
				my0 = 1;
			}
            break;
        case 1:
            sx1 = sx - 1;
			sy1 = sy;
			if(mx0 == 1) rev0();
			else{
				mx0 = -1;
				my0 = 0;
			}
            break;
        case 2:
            sx1 = sx + 1;
			sy1 = sy;
			if(mx0 == -1) rev0();
			else{
				mx0 = 1;
				my0 = 0;
			}
            break;
        case 3:
            sx1 = sx;
			sy1 = sy - 1;
			if(my0 == 1) rev0();
			else{
				mx0 = 0;
				my0 = -1;
			}
            break;
    }

	if(!dead_snake0){
		move0();

    sx1 = snake0 -> x; //takes the sx1 to snake value
	sy1 = snake0 -> y;

	float re = reward0(sx, sy, sx1, sy1);
	}
}

void itera(){
	iterations++; //Increment iterations
	int sx = snake -> x; //takes (4,0) value initially
	int sy = snake -> y;

	float inputs[6];
	int sx1 = sx;
	int sy1 = sy;

	float new_q;
	if(rand() % 100 > exploration_rate){
		new_q = max_q(sx, sy, food_x2, food_y2);
	}else{
		int a = rand() % 4;
        // same as in max_q
		if(a == 0){
			sx1 = sx + 1;
			sy1 = sy;
			if(mx == -1) rev();
			else{
				mx =  1;
				my =  0;
			}
		}else if(a == 1){
			sx1 = sx - 1;
			sy1 = sy;
			if(mx == 1) rev();
			else{
				mx = -1;
				my =  0;
			}
		}else if(a == 2){
			sx1 = sx;
			sy1 = sy + 1;
			if(my == -1) rev();
			else{
				mx =  0;
				my =  1;
			}
		}else{
			sx1 = sx;
			sy1 = sy - 1;
			if(my == 1) rev();
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
		move();

    sx1 = snake -> x;
	sy1 = snake -> y;
	}

    float dout[1];
	float re = reward(sx, sy, sx1, sy1);

	dout[0] =  re + 0.9 * new_q - old_q;
	net -> learn(dout);
	old_q = new_q;
}

void par(float x1, float x2, float y1, float y2, float z1, float z2){
	//glColor3f(0.3,0.56,0.84); //Blue

	glBegin(GL_POLYGON); // Each set of 4 vertices form a quad from polygon

	glColor3f(1.0,1.0,0.0);
	glVertex3f(x1, y1, z1); // 3 GLfloat parameters
	glColor3f(1.0,1.0,0.0);
	glVertex3f(x2, y1, z1);
	glColor3f(0.0,0.0,1.0);
	glVertex3f(x2, y2, z1);
	glColor3f(0.0,0.0,1.0);
	glVertex3f(x1, y2, z1);

	glEnd(); //Gl end
}

void pardead(float x1, float x2, float y1, float y2, float z1, float z2){
	//glColor3f(0.3,0.56,0.84); //Blue

	glBegin(GL_POLYGON); // Each set of 4 vertices form a quad from polygon

	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y1, z1); // 3 GLfloat parameters
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y1, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y2, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y2, z1);

	glEnd(); //Gl end
}

void parfood(float x1, float x2, float y1, float y2, float z1, float z2){
	//glColor3f(0.3,0.56,0.84); //Blue

	glBegin(GL_POLYGON); // Each set of 4 vertices form a quad from polygon

	glColor3f(0.5,0.5,0.5);
	glVertex3f(x1, y1, z1); // 3 GLfloat parameters
	glColor3f(0.5,0.5,0.5);
	glVertex3f(x2, y1, z1);
	glColor3f(0.5,0.5,0.5);
	glVertex3f(x2, y2, z1);
	glColor3f(0.5,0.5,0.5);
	glVertex3f(x1, y2, z1);

	glEnd(); //Gl end
}

void parb(float x1, float x2, float y1, float y2, float z1, float z2){
	//glColor3f(0.3,0.56,0.84); //Blue

	glBegin(GL_POLYGON); // Each set of 4 vertices form a quad from polygon

	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y1, z1); // 3 GLfloat parameters
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y1, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x2, y2, z1);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(x1, y2, z1);

	glEnd(); //Gl end
}

void DrawRules(){
		//glColor3f(0.3,0.56,0.84);   // background
        glColor3f(0.0,0.0,0.0);

		glBegin(GL_POLYGON);
		glVertex3f(0.0,0.0,0.0);
		//glColor3f(0.137,0.137,0.556);
		glColor3f(0.0,0.0,0.0);
		glVertex3f(100.0,0.0,0.0);
		glColor3f(0.0,0.4,0.0);
		glVertex3f(100.0,100.0,0.0);
		glVertex3f(0.0,100.0,0.0);
		glEnd();

		glPushMatrix(); // Save model-view matrix setting
		glScalef(0.8,0.8,0); //multiply the current matrix by a general scaling matrix
		glPopMatrix();  // Restore the model-view matrix

		glColor3f(0.4,0.6,0.4);
		glRectf(20.0,20.0,80.0,80.0); //draw a rectangle
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
		drawString(23,40,0,GLUT_BITMAP_HELVETICA_18,"- If snake hits it's body or boundary:restart");
		drawString(23,36,0,GLUT_BITMAP_HELVETICA_18,"- Use the arrow keys to control snake's movement");
		drawString(23,32,0,GLUT_BITMAP_HELVETICA_18,"- AI vs Human, the higher score wins");
		drawString(42,25,0,GLUT_BITMAP_HELVETICA_18," ENJOY THE GAME");

		glutPostRedisplay(); //marks the current window as needing to be redisplayed
}

void welcome(){
	glMatrixMode(GL_PROJECTION);
	glColor3f(0.0,0.0,0.0);   //welcome background

	glBegin(GL_POLYGON);
	glVertex3f(0.0,0.0,0.0);
	glColor3f(0.0,0.0,0.0);
	glVertex3f(100.0,0.0,0.0);
	glColor3f(0.0,0.4,0.0);
	glVertex3f(100.0,100.0,0.0);
	glVertex3f(0.0,100.0,0.0);
	glEnd();

	// button 1 .. User
	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,39.5,60.5,45.5);

	glColor3f(0.8,0.8,0.8);
	glRectf(40,40,60,45);
	glColor3f(0.0,0.2,0.0);
	drawString(43.5,42,0,GLUT_BITMAP_HELVETICA_18,"MAN vs AI MODE");

	// button 2 .. Network_play
	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,29.5,60.5,35.5);

	glColor3f(0.8,0.8,0.8);
	glRectf(40,30,60,35);
	glColor3f(0.0,0.2,0.0);
	drawString(42,31,0,GLUT_BITMAP_HELVETICA_18,"SINGLE PLAYER MODE");

	// button 3 .. How_to
	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,19.5,60.5,25.5);

	glColor3f(0.8,0.8,0.8);
	glRectf(40,20,60,25);
	glColor3f(0.0,0.2,0.0);
	drawString(44,21,0,GLUT_BITMAP_HELVETICA_18,"INSTRUCTIONS");

	// button 4 .. exit
	glColor3f(0.4,0.6,0.4);
	glRectf(39.5,9.5,60.5,15.5);
	glColor3f(0.8,0.8,0.8);
	glRectf(40,10,60,15);
	glColor3f(0.0,0.2,0.0);
	drawString(47,11,0,GLUT_BITMAP_HELVETICA_18,"QUIT");


	glPushMatrix(); // Save model-view matrix setting

	glColor3f(0.6,0.7,0.6); 
	drawString(40,85,0,GLUT_BITMAP_TIMES_ROMAN_24,"ECE 6122 PROJECT ");
	drawString(29,73,0,GLUT_BITMAP_TIMES_ROMAN_24,"SnakeBee: OpenGL based 2D Snake - Human vs AI");
	drawString(20,64,0,GLUT_BITMAP_TIMES_ROMAN_24,"The Snake pattern is inspired by GaTechs own Buzz the Bee. Go Jackets!");
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

// void DrawNeural(){

    
// 	glClearColor(0.0, 0.18, 0.0, 0); //BG-color
// 	glLoadIdentity();
// 	//SCREENW = SCREENW/(1.2);
// 	//SCREENH = SCREENH/(1.05);
// 	gluOrtho2D (0, SCREENW, 0, SCREENH);
// 	glClear(GL_COLOR_BUFFER_BIT);

// 	glBegin(GL_POLYGON);
// 	glColor3f (0.0, 0.3, 0.0);
// 	glVertex3f (0.0, 800.0, 0.0);
// 	glColor3f (0.0, 0.11, 0.0);
// 	glVertex3f (0, 700.0, 0.0);
// 	glColor3f (0.0, 0.11, 0.0);
// 	glVertex3f (1400.0, 700.0, 0.0);
// 	glColor3f (0.0, 0.3, 0.0);
// 	glVertex3f (1400.0, 800.0, 0.0);

// 	glEnd();

// 	DrawScore();

// 	neural_check = true;

// 	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer and depth buffer
// 	glLoadIdentity ();
// 	gluPerspective(45.0, (float)SCREENW/(float)SCREENH, 0.1f, 200.0);
// 	//gluLookAt(0.0, 0.0, 5.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0); 

// 	glTranslatef(-10.0, 0.0, -28.0); // Translate by -22 on z-axis
// 	//glTranslatef(SCREENW/(2), SCREENH/(2), 0);
// 	sq *p = snake;
// 	par(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
// 	par(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
// 	par(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
// 	par( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);
// 	while(p != NULL){
// 		par((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
// 		p = p -> nexploration_ratet; //p reach null nexplorartion
// 	}
// 	par(food_x/2.0, food_x/2.0 + 0.4 , food_y/2.0 , food_y/2.0 + 0.4, 0.0 , 0.0); //food decrement
// }

void DrawUser(){
	
	glClearColor(0.0, 0.18, 0.0, 0); //BG-color
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
	glTranslatef(0.0, 0.0, -28.0); // Translate by -22 on z-axis
	
	sq *p = snake0;
	parb(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	parb(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	parb(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	parb( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);

	if(dead_snake0){
	while(p != NULL){
		pardead((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
		p = p -> nexploration_ratet; //p reach null nexplorartion
	}
	///// START OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
	glPushMatrix();
	glColor3f (0.8, 0.0, 0.0);
    glTranslatef(-8.0,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "Snake Dies :(");
    glPopMatrix();
	///// END OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
	}
	else{
	while(p != NULL){
		par((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
		p = p -> nexploration_ratet; //p reach null nexplorartion
	}
	}
	parfood(food_x/2.0, food_x/2.0 + 0.4 , food_y/2.0 , food_y/2.0 + 0.4, 0.0 , 0.0); //food decrement
}

void DrawPlay(){
	
	glClearColor(0.0, 0.18, 0.0, 0); //BG-color
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
	glTranslatef(-10.0, 0.0, -28.0); // Translate by -22 on z-axis
	
	
	sq *p = snake0;
	
	parb(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	parb(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	parb(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	parb( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);

	if(dead_snake0){
	while(p != NULL){
		pardead((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
		p = p -> nexploration_ratet; //p reach null nexplorartion
	}
	}
	else{
		while(p != NULL){
		par((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
		p = p -> nexploration_ratet; //p reach null nexplorartion
	}
	}
	parfood(food_x/2.0, food_x/2.0 + 0.4 , food_y/2.0 , food_y/2.0 + 0.4, 0.0 , 0.0); //food decrement

	//DrawScore();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer and depth buffer
	glLoadIdentity ();
    gluPerspective(45.0, (float)SCREENW/(float)SCREENH, 0.1f, 200.0);
	//gluLookAt(0.0, 0.0, 5.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0); 

	glTranslatef(9.3, 0.0, -28.0); // Translate by -22 on z-axis
	sq *p2 = snake;
	parb(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	parb(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	parb(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	parb( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);
	if(dead_snake){
	while(p2 != NULL){
		pardead((p2 -> x)/2.0,(p2 -> x)/2.0 + 0.4,(p2 -> y)/2.0,(p2 -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
		p2 = p2 -> nexploration_ratet; //p reach null nexplorartion
	}
	}
	else{
		while(p2 != NULL){
		par((p2 -> x)/2.0,(p2 -> x)/2.0 + 0.4,(p2 -> y)/2.0,(p2 -> y)/2.0 + 0.4, 0.0, 0.0); //decrement
		p2 = p2 -> nexploration_ratet; //p reach null nexplorartion
	}
	}
	parfood(food_x2/2.0, food_x2/2.0 + 0.4 , food_y2/2.0 , food_y2/2.0 + 0.4, 0.0 , 0.0); //food decrement

	///// START OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
	if(dead_snake0){
	glPushMatrix();
	glColor3f (0.8, 0.0, 0.0);
    glTranslatef(-42.0,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "Player Snake Dies :(");
    glPopMatrix();
	}

	if(dead_snake){
    glPushMatrix();
	glColor3f (0.8, 0.0, 0.0);
    glTranslatef(1.0,-20.0, -28.0);
    glScalef(0.02f, 0.02f, 0.02f);
    draw_string(GLUT_STROKE_ROMAN, "AI Snake Dies :(");
    glPopMatrix();
	}
	///// END OF the code WHICH  DISPLAYS THE RED SNAE DIES ON THE GAME 
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
        // case 3:
        //     DrawNeural();
        //     break;
        case 4:
            welcome();
            break;
        case 5:
            DrawRules();
            break;
		
    }
    glFlush(); // Render now
	glutSwapBuffers(); //Swap front- and back framebuffer
}

/* Callback handler for special-key event */
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

/* Callback handler for normal-key event */
void keyboard(unsigned char key, int a, int b) {
	switch (key) {
        case 'w': //up
            dir = 0;
            break;
        case 'd': //right
            dir = 2;
            break;
        case 'a': //left
            dir = 1;
            break;
        case 's': //down
            dir = 3;
            break;
        case 'f': //set other food cordinate
            set_f();
            break;
        case 'o': //decrease timer
            tmp--;
            break;
        case 'p': //increase timer
			if(tmp < 0) tmp = 0;
            tmp++;
            break;
		case 'r': //restart
			start();
			sc =0; tmp = 50;
            break;
        case 27:
            exit(0);
            break;
    }
}

void mouse(int button, int state, int ax, int ay)            // takes input from mouse
{
	int mx=ax*100/SCREENW,my=(SCREENH-ay)*100/SCREENH;		// m = mouse cordinate to graphics
	
	down= button==GLUT_LEFT_BUTTON && state==GLUT_LEFT;
	if(down)
    {
        if (key1==4) //welcome
        {
            if(mx > (40) && mx < (60) && my > (10) && my < (15) )  //exit button
            {
                exit(0);
            }
            if(mx > (40) && mx < (60) && my > (20) && my < (25) ) //how_to
            {
                glClear(GL_COLOR_BUFFER_BIT);
                key1=5;
				display();
            }
            if(mx > (40) && mx < (60) && my > (40) && my < (45) ) //user
            {
                key1 = 1;
                num = 5;
                Score = 0;
				Score0 = 0;
                display();
            }
			if(mx > (40) && mx < (60) && my > (30) && my < (35) ) //neural_play
            {
                key1 = 2;
                display();
            }
        }
        if (key1==5) //back option in instruction
        {
            if(mx > (40) && mx < (60) && my > (5) && my < (10) )
            {
                key1=4;
                glClear(GL_COLOR_BUFFER_BIT);
                welcome();
            }
        }
    }
    glutMouseFunc(mouse); // Register callback handler for mouse event
}

void timer(int = 0){
	if (key1==1){
        itera();
		//Tick();
		//cout << "iterations : " << iterations << " speed : " << tmp << " score : " << sc << endl;
	}
	if (key1==2){
	 	Tick();
	}
		//cout << "speed : " << tmp << " score : " << sc << endl;
	glutPostRedisplay(); //marks the current window as needing to be redisplayed
	glutTimerFunc(tmp, timer, 0); //registers a timer callback to be triggered in a specified number of milliseconds
}

void timer2(int = 0){
	if (key1==1){
        Tick();
		//cout << "speed : " << tmp << " score : " << sc << endl;
	}
	glutPostRedisplay(); //marks the current window as needing to be redisplayed
	glutTimerFunc(tmp, timer2, 0); //registers a timer callback to be triggered in a specified number of milliseconds
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h); // Set the viewport to cover the new window
	// Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
	glLoadIdentity(); // Reset the model-view matrix
	if (key1 == 4){
		glOrtho(0.0, 100.0, 0.0, 100.0,	-5.0 , 10.0);
	}

}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv){
    //srand(time(0));// Set random variable as current time

	srand(time(NULL));
	net = new neural(num_inputs, num_outputs, num_layers, 10, learning_rate); //Send neural with initial values
	net -> init(); //reach neural_init

	start(); //Start snake layout with initial values
    start0();
    set_f(); //Setup food point cordinates
	set_f0(); //Setup food point cordinates

    glutInit(&argc, argv); // Initialize GLUT

    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); // Use RGBA color, enable double buffering and enable depth buffer
    glutInitWindowSize (SCREENW,SCREENH);  // Set the window's initial width & height
    glutInitWindowPosition(500, 0); // Position the window's initial top-left corner
    glutCreateWindow("SnakeBee: User and Neural Network game"); // Create a window with the given title

    init();

	glutTimerFunc (400,timer,0);     // First timer call immediately
	glutTimerFunc (400,timer2,0);
	glutReshapeFunc(myReshape);       // Register callback handler for window re-size event
    glutKeyboardFunc(keyboard);   // Register callback handler for special-key event
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);   // Register callback handler for mouse event

	glutDisplayFunc(display); // Register display callback handler for window re-paint

    glutMainLoop();           // Enter the event-processing loop
    
    return 0;
}
