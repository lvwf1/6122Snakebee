# 6122Snakebee

Prerequisite: Linux or Mac operating system. Make sure that your system has Opengl, Glut/ FreeGlut and respective Mesa 3d Graphics Library installed and functioning. 

Procedure on Linux: 

Step 1 open ubuntu

Step 2 open terminal

Step 3 $ sudo apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev libxmu-dev libxi-dev

Step 4 $ git clone https://github.com/lvwf1/6122Snakebee.git

Step 5 $ cd 6122Snakebee

Step 6 $ mkdir build

Step 7 $ cd build

Step 8 $ cmake ..

Step 9 $ make

Step 10 $ ./main

Procedure on Mac (beta)

Step 1 change #include <GL/gl.h>、#include <GL/glut.h>、#include <GL/glu.h> to #include <OpenGL/gl.h>、#include <OpenGL/glu.h>、#include <GLUT/glut.h>

Step 2 run command: g++ -framework OpenGL -framework GLUT -framework Foundation -o test main.cpp

Step 3 run command: ./test

The conventional Snake game is among the most popular games in the owing to the Nokia mobile phones. It is played by sole player who controls moving direction of a snake and tries to eat food items by running into them with the head of the snake, while the food items are programmed to emerge in random positions inside the bounded area. Each eaten item makes the snake longer, so maneuvering is progressively more difficult. In our project, we have slightly changed the game by turning it into a Human vs AI game. In the Human vs AI mode, the human player snake which is controlled manually by the player competes with the AI player snake which is continuously trained using various neural network algorithms, in parallel bounded areas on the same OpenGL window. The speed of both the snakes remains equal throughout the game and the constraints are the same as the conventional snake game – the snake dies if it hits its own body or the boundary of the bounded area with its head, while the reward is that the snake grows longer with every food item ingested and the score of the corresponding snake increases. The snake with higher points wins the game while the first snake to die loses. For the sake of practice, a Single Player Mode is also added to the game interface, which is nothing but the conventional snake game. The analysis and comparison of various machine learning algorithms is performed on the AI Snake and the results are observed on the scores obtained.
