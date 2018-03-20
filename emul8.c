/* ESE 350, Spring 2018
 * ------------------
 * Starter code for Lab 4
 *
 * The main purpose is to provide all the FreeGLUT
 * code required to handle display and input.
 * 
 * You are encouraged and, in some cases, required to edit
 * the given functions to suit your implementation.
 *
 * You are by no means bound to use all/any of the given code
 * or required to stick to this style.
 * Feel free to experiment and give a better solution.
 *
 * This uses the FreeGLUT libraries installed in your VM.
 * If you choose to use a different set of third party libraries,
 * please consult a TA.
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamul.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

#define MEM_SIZE 4095
#define REG_SIZE 16

//architecture we are emulating, refer gamul.h
gamul8 gamer;

// method to draw a single pixel on the screen
void draw_square(float x_coord, float y_coord);

// update and render logic, called by glutDisplayFunc
void render();

// idling function for animation, called by glutIdleFunc
void idle();

// initializes GL 2D mode and other settings
void initGL();

// function to handle user keyboard input (when pressed) 
void your_key_press_handler(unsigned char key, int x, int y);

// function to handle key release
void your_key_release_handler(unsigned char key, int x, int y);


/*	FUNCTION: main
 *  --------------
 *	Main emulation loop. Loads ROM, executes it, and draws to screen.
 *  You may also want to call here the initialization function you have written
 *  that initializes memory and registers.
 *	PARAMETERS:
 *				argv: number of command line arguments
 *				argc[]: array of command line arguments
 *	RETURNS:	usually 0
 */
int main(int argc, char *argv[])
{
//fetch the data 
//allocate [] for mem -> 4096 bytes (0xFFF : 0x000), first 512 bytes reserved for interpreter data, such as character and letter sprites (0x200 : 0x000)
  unsigned char *mem;
  mem = malloc(MEM_SIZE, 2); 
  if (mem = null) { 
  	printf("%s\n", "Mem is null" );
  	exit(0);}

  //allocate [] for reg -> 16 general purpose 8-bit registers (Vx -> VF : V0), VF is reserved for instruction flag
  unsigned char *reg;
  reg = malloc(REG_SIZE, 1);
    if (reg = null) { 
  	printf("%s\n", "Reg is null" );
  	exit(0);}

  //allocate index register -> 16-bit index register (I), used to store mem addrs for loading from/storing to mem -> (0xFFF : 0x000)
  unsigned char *index_register;
  index_register = malloc(1, 2);
    if (index_register) = null) { 
  	printf("%s\n", "index_register is null" );
  	exit(0);}

  //allocate PC -> 16 bit, sote the currently executing addr (0xFFF : 0x000)
  unsigned char *PC;
  PC = malloc(1, 2);
    if (PC = null) { 
  	printf("%s\n", "PC is null" );
  	exit(0);}

  //allocate stack -> 16 16-bit values, used to store addr that interpreter should ret to when finished w/a subroutine; Stack Pointer (SP) is 8-bit and points to topmost mem loc of the stack
  unsigned char *stack;
  stack = malloc(16, 2); 
    if (stack = null) { 
  	printf("%s\n", "Stack is null" );
  	exit(0);}

//Stack Pointer (SP) is 8-bit and points to topmost mem loc of the stack
  unsigned char *stack_pointer; 
  stack_pointer = malloc(1, 1); 
    if (stack_pointer) = null) { 
  	printf("%s\n", "Stack Pointer is null" );
  	exit(0);}

  //allocate sound and delay -> both 8-bit; when nonzero, decrement at a rate of 60Hz
  unsigned char *av_io;
  av_io = malloc= (1, 1);
    if (av_io = null) { 
  	printf("%s\n", "Av_io is null" );
  	exit(0);}
  //allocate display -> each pixel is ON or OFF; 2048 pixels (64 x 32)
  unsigned char *disp;
  disp = malloc(2048, 1);
    if (disp) = null) { 
  	printf("%s\n", "Disp is null" );
  	exit(0);}

  //allocate keypad -> 16 keys (0xF : 0x0)
  unsigned char *keys;
  keys = malloc(16, 1);
    if (keys = null) { 
  	printf("%s\n", "Keys is null" );
  	exit(0);}









	// seed random variable for use in emulation
	srand(time(NULL));

	// initialize GLUT
	glutInit(&argc, argv);

	// initialize display and window
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("ESE 519 - Gamul8r");

	// initialize orthographic 2D view, among other things
	initGL();
	 
	// handle key presses and releases
	glutKeyboardFunc(your_key_press_handler);
	glutKeyboardUpFunc(your_key_release_handler);

	// GLUT draw function
	glutDisplayFunc(render);

	// GLUT idle function, causes screen to redraw
	glutIdleFunc(idle);

	// main loop, all events processed here
	glutMainLoop();

	return 0;
}

/*	FUNCTION: draw_square
 *  ----------------------
 *	Draws a square. Represents a single pixel
 *  (Up-scaled to a 640 x 320 display for better visibility)
 *	PARAMETERS: 
 *	x_coord: x coordinate of the square
 *	y_coord: y coordinate of the square
 *	RETURNS: none
 */
void draw_square(float x_coord, float y_coord)
{
	// draws a white 10 x 10 square with the coordinates passed

	glBegin(GL_QUADS);  //GL_QUADS treats the following group of 4 vertices 
						//as an independent quadrilateral

		glColor3f(1.0f, 1.0f, 1.0f); 	//color in RGB
										//values between 0 & 1
										//E.g. Pure Red = (1.0f, 0.0f, 0.0f)
		glVertex2f(x_coord, y_coord);			//vertex 1
		glVertex2f(x_coord + 10, y_coord);		//vertex 2
		glVertex2f(x_coord + 10, y_coord + 10);	//vertex 3
		glVertex2f(x_coord, y_coord + 10);		//vertex 4

		//glVertex3f lets you pass X, Y and Z coordinates to draw a 3D quad
		//only if you're interested

	glEnd();
}

/*	FUNCTION: render
 *  ----------------
 *	GLUT render function to draw the display. Also emulates one
 *	cycle of emulation.
 *	PARAMETERS: none
 *	RETURNS: none
 */
void render()
{

	// clears screen
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();

	display_func(&gamer);

	// draw a pixel for each display bit
	int i, j;
	for (i = 0; i < SCREEN_WIDTH; i++) {
		for (j = 0; j < SCREEN_HEIGHT; j++) {
			if (gamer.display[i][j] == 1) {
				draw_square((float)(i * 10), (float)(j * 10));
			}
		}
	}

	// swap buffers, allows for smooth animation
	glutSwapBuffers();
}

/*	FUNCTION: idle
 *  -------------- 
 *	GLUT idle function. Instructs GLUT window to redraw itself
 *	PARAMETERS: none
 *	RETURNS: none
 */
void idle()
{
	// gives the call to redraw the screen
	glutPostRedisplay();
}

/*	FUNCTION: initGL
 *  ----------------
 *	Initializes GLUT settings.
 *	PARAMETERS: none
 *	RETURN VALUE: none
 */
void initGL()
{
	// sets up GLUT window for 2D drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// clears screen color (RGBA)
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

/*	FUNCTION: your_key_press_handler
 *  --------------------------------
 *	Handles all keypresses and passes them to the emulator.
 *  This is also where you will be mapping the original GAMUL8
 *  keyboard layout to the layout of your choice
 *  Something like this:
 *  |1|2|3|C|		=>		|1|2|3|4|
 *	|4|5|6|D|		=>		|Q|W|E|R|
 *	|7|8|9|E|		=>		|A|S|D|F|
 *	|A|0|B|F|		=>		|Z|X|C|V|
 *	PARAMETERS: 
 *	key: the key that is pressed.
 *	x: syntax required by GLUT
 *	y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used, 
 *  but still pass them as glutKeyboardFunc needs it.) 
 *	RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void your_key_press_handler(unsigned char key, int x, int y)
{

}

/*	FUNCTION: your_key_release_handler
 *  --------------------------------
 *	Tells emulator if any key is released. You can maybe give
 *  a default value if the key is released.
 *	PARAMETERS: 
 *	key: the key that is pressed.
 *	x: syntax required by GLUT
 *	y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used, 
 *  but still pass them as glutKeyboardFunc needs it.) 
 *	RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void your_key_release_handler(unsigned char key, int x, int y)
{

}