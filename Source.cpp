#include<glew.h>
#include<freeglut.h>
#include<iostream>
#include<SOIL2.h>
#include <random>
#include<malloc.h>
#include <Mmsystem.h> // for play sound
#include "Structers.h"

using namespace std;

const int TOTAL_WIDTH = 480;
const int TOTAL_HEIGHT = 640;
const int PLANE_ARE_HEIGHT = 190;
const int PLANE_AREA_WIDTH = 430;
int remainingLife = 3;
int score = 0;
int plane_move_x = 0, plane_move_y = 0;
int level_speed = 1;
GLuint first_heli_id, second_heli_id, third_heli_id,fourth_heli_id, plane;
GLuint heli_id[] = { first_heli_id , second_heli_id, third_heli_id, fourth_heli_id };
bool isPlayed = false; // for preventing sound loop

void init2D();																// load images and start configurations
void loadTexture(const string& path, GLuint& object);						// create textures by using SOIL2 library
void display();																// draw needed objects
void drawHelis(Vehicle helicopters[]);										// draws helicopters
void drawPlane();															// draws plane 
void showScoreboard();														// draws scorboard to the screen
void renderBitmapString(float x, float y,void* font, const char* string);	// helper for rendering characters
void keyboard_activity(int key, int x, int y);								// keyboard listener
void keyboard_activity2(unsigned char key, int x, int y);					// keyboard listener
void checkLevelUpdate();													// it checks plane location for leveling up
void reshape(int w, int h);													// reshape function for windows resizing.
void updateLevelSpeed();													// after every level update the choppers speed
void randomizeColors(int i);												// generate new colors for textures
int randomizeScale();														// choose random flying direction
void randomizePlaneLocation();												// helper 
int randomizeStartSpeed();													// choose random start speed
void gameOver();															// it shows that game over		
void reverseTheAxis(int i);													// reverses the axises for reverse flow
void idlefunc();															// without any interference check intersections
void updateGameStatus();													// this func resets level after crush
bool intersectCheck();														// it checks collisions between plane and helicopters
bool checkIntersectedAreas(int xstart, int ystart, int partw, int parth);	// after reducing scan area this function scans pixel-perfect level
void regular_flow(int what);												// regular program flow 
Vehicle helicopters[] = {
	Vehicle({ 0, 525, 0 }, { 1, 1, 1 }, randomizeScale(), randomizeStartSpeed()),
	Vehicle({ 0, 430, 0 }, { 1, 1, 1 }, randomizeScale(), randomizeStartSpeed()),
	Vehicle({ 0, 335, 0 }, { 1, 1, 1 }, randomizeScale(), randomizeStartSpeed()),
	Vehicle({ 0, 240, 0 }, { 1, 1, 1 }, randomizeScale(), randomizeStartSpeed())
};
Vehicle planeV = Vehicle({ 215, 0, 0 }, { 1,1,1 }, 1 , 1);



/*
	entry point for our app
*/
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize(TOTAL_WIDTH, TOTAL_HEIGHT);
	glutCreateWindow("REACH TO THE TARGET");
	init2D();
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard_activity);
	glutKeyboardFunc(keyboard_activity2);
	glutIdleFunc(idlefunc);
	glutTimerFunc(0,regular_flow,0);
	glutDisplayFunc(display);
	glutMainLoop();
}

/*
	check intersections
*/
void idlefunc() {
		updateGameStatus();
}

/*
	reset level and take 1 heart from user
*/
void updateGameStatus() {
	
	if (remainingLife == 0) {/*WAIT FOR ENTER*/}
	else {
		if (intersectCheck()) {
			PlaySound("Sounds/plane_crash.wav",
				GetModuleHandle(NULL),
				SND_ASYNC | SND_FILENAME );
			remainingLife -= 1;
			plane_move_y = 0;
			randomizePlaneLocation(); }}}


/*
	this function loads images and does some configuration for program
*/
void init2D() {
	glClearColor(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, TOTAL_WIDTH, TOTAL_HEIGHT);

	gluOrtho2D(0, TOTAL_WIDTH, 0, TOTAL_HEIGHT);

	loadTexture("Images/heli1.png", heli_id[0]);
	loadTexture("Images/heli2.png", heli_id[1]);
	loadTexture("Images/heli3.png", heli_id[2]);
	loadTexture("Images/heli4.png", heli_id[3]);
	loadTexture("Images/plane.png", plane);

}

/*
	draw objects and manage them
*/
void display() {
	glClear(GL_COLOR_BUFFER_BIT); // clear screen

	if (remainingLife > 0) {
		showScoreboard();

		// draw helicopters
		drawHelis(helicopters);

		// draw plane
		drawPlane();
	}
	else { gameOver(); }

	glFlush(); // refresh screen
	glutSwapBuffers(); // change frame buffers
}

/*
	reshape for window sizing
*/
void reshape(int w, int h)
{	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, TOTAL_WIDTH, 0,TOTAL_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutReshapeWindow(TOTAL_WIDTH, TOTAL_HEIGHT);
}

/*
	draws all given objects with spesifications
*/
void drawHelis(Vehicle helicopters[] ) {
	
	for (int i = 0; i < 4 ; i++)
	{
		glLoadIdentity();
		glTranslated(helicopters[i].speed, 0, 0);
		glColor3f(helicopters[i].color.x, helicopters[i].color.y, helicopters[i].color.z);
		glBindTexture(GL_TEXTURE_2D, heli_id[i]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); 
				glVertex2i(helicopters[i].position.x , helicopters[i].position.y);
			glTexCoord2f(1, 0); 
				glVertex2i(helicopters[i].position.x  + helicopters[i].width , helicopters[i].position.y);
			glTexCoord2f(1, 1); 
				glVertex2i(helicopters[i].position.x  + helicopters[i].width, helicopters[i].position.y + helicopters[i].height);
			glTexCoord2f(0, 1); 
				glVertex2i(helicopters[i].position.x, helicopters[i].position.y + helicopters[i].height);
		glEnd();
	}
}


/*
	draws plane
*/
void drawPlane() {
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, plane);
	glLoadIdentity();
	glTranslated(plane_move_x, plane_move_y, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
			glVertex2i(planeV.position.x, planeV.position.y );
		glTexCoord2f(1, 0);
			glVertex2i(planeV.position.x + planeV.width , planeV.position.y );
		glTexCoord2f(1, 1);
			glVertex2i(planeV.position.x + planeV.width, planeV.position.y + planeV.height );
		glTexCoord2f(0, 1);
			glVertex2i(planeV.position.x, planeV.position.y + planeV.height );
	glEnd();
}

/*
	this function increases coordinates for helicopter moves and resets, scales, recolors objects after reaching beyond wall limits
*/
void regular_flow(int what) {
	for (int i = 0; i < 4; i++)
	{
		if (helicopters[i].scale_factor == 1 && helicopters[i].speed <= TOTAL_WIDTH)
		{
			helicopters[i].speed += helicopters[i].start_speed + level_speed;
		}
		else if (helicopters[i].scale_factor == -1 && helicopters[i].speed >= -TOTAL_WIDTH ) {
			helicopters[i].speed += (helicopters[i].start_speed - level_speed);
		}
		else {
			int previous_scale_factor = helicopters[i].scale_factor;
			helicopters[i].scale_factor = randomizeScale();
			if (previous_scale_factor != helicopters[i].scale_factor)
			{
				reverseTheAxis(i);
			}
			else {
				helicopters[i].speed = 0;
			}
			randomizeColors(i);
		}
		glutPostRedisplay();}

	glutTimerFunc(20, regular_flow, 0);
}


/*
	it returns a integer between selected numbers for random speed.
*/
int randomizeStartSpeed() {
	random_device rd;
	default_random_engine generator( rd() );
	uniform_int_distribution<int> distribution(1, 5);
	return distribution(generator);
}


/*
	it returns -1 or 1 for flying direction to use as a parameter in glScalef function.
*/
int randomizeScale() {
	int list[] = { -1 , 1 };
	random_device rd;
	default_random_engine generator(rd());
	uniform_int_distribution<int> distribution(0, 1);
	int index = distribution(generator);
	return list[index];

}

/*
	this function gives randomized float numbers between 0 and 1 for r,g,b values after everyn turn
*/
void randomizeColors(int i) {
	random_device rd;
	default_random_engine generator(rd());
	uniform_real_distribution<float> distribution(0, 1);
	helicopters[i].color.x = distribution(generator);
	helicopters[i].color.y = distribution(generator);
	helicopters[i].color.z = distribution(generator);
}

/*
	this function randomizes plane location after every try
*/
void randomizePlaneLocation() {
	random_device rd;
	default_random_engine width_generator(rd());
	default_random_engine height_generator(rd());
	uniform_int_distribution<int> width_distribution(0, PLANE_AREA_WIDTH);
	uniform_int_distribution<int> height_distribution(0, PLANE_ARE_HEIGHT);

	int xCoord = width_distribution(width_generator);
	int yCoord = height_distribution(height_generator);

	planeV.position.x = xCoord;
	planeV.position.y = yCoord;
}

/*
	helper for rendering characters 
*/
void renderBitmapString(float x, float y,void* font, const char* string) {
	const char* c;
	glColor3f(1, 1, 1);
	glRasterPos2f(x,y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
/*
	draws scorboard to right top
*/
void showScoreboard() {
	glLoadIdentity();
	glDisable(GL_BLEND);

	char buffer[20] = { '\0' };
	char buffer2[20] = { '\0' };
	sprintf_s(buffer, "SCORE:%d", score);
	sprintf_s(buffer2, "HEARTS:%d", remainingLife); 
	renderBitmapString(300,620,GLUT_BITMAP_9_BY_15, buffer);
	renderBitmapString(400,620,GLUT_BITMAP_9_BY_15, buffer2);

	glEnable(GL_BLEND);

}

/*
	this function loads textures with soil2 library and it gives some spesific corrections on texture
*/
void loadTexture(const string& path, GLuint& object) {
	/* load an image file directly as a new OpenGL texture */
	object = SOIL_load_OGL_texture
	(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y  | SOIL_FLAG_COMPRESS_TO_DXT
	);
	//  SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA |
	glBindTexture(GL_TEXTURE_2D, object);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* check for an error during the load process */
	if (0 == object)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
}

/*
	this function does required actions after keyboard activities
*/
void keyboard_activity(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		plane_move_y += 10;
		checkLevelUpdate();
	}
	if (key == GLUT_KEY_DOWN && (plane_move_y + planeV.position.y) > 0) {
		plane_move_y -= 10;
	}
	if (key == GLUT_KEY_RIGHT && (planeV.position.x + planeV.width + plane_move_x) < TOTAL_WIDTH) {
		plane_move_x += 10;
	}
	if (key == GLUT_KEY_LEFT && (planeV.position.x + plane_move_x) > 0) {
		 plane_move_x -= 10;
	}
	glutPostRedisplay(); }

/*
	this function does required actions after keyboard activities ( ENTER)
*/
void keyboard_activity2(unsigned char key, int x, int y) {
	if (key == 13 && remainingLife == 0) {
		cout << "PRESSED ENTER" << endl;
		remainingLife = 3;
		plane_move_x = 0;
		plane_move_y = 0;
		score = 0;
		isPlayed = false;
		randomizePlaneLocation();
		glutPostRedisplay();
	}
}

/*
this function checks if plane reached the target place
*/
void checkLevelUpdate() {
	if (plane_move_y+planeV.position.y > TOTAL_HEIGHT)
	{
		updateLevelSpeed();
		PlaySound("Sounds/level_up.wav", 
			GetModuleHandle(NULL),
			SND_ASYNC | SND_FILENAME);
		cout << "LEVEL UP" << endl;
		score += 10;
		plane_move_y = 0;
		plane_move_x = 0;
		randomizePlaneLocation();
	}}

/*
	after every level this function increases choppers speed
*/
void updateLevelSpeed() {
	level_speed += 1;
}

/*
	this function detects collision between QUADS via their coordinates for optimizing issues 
*/
bool intersectCheck() {
	bool is_x_have_collision, is_y_have_collision;
	int px0, px1, py0, py1, hx0, hx1, hy0, hy1;

	for (int i = 0; i < 4; i++) {
		px0 = planeV.position.x + plane_move_x; px1 = planeV.position.x + plane_move_x + planeV.width;
		py0 = planeV.position.y + plane_move_y; py1 = planeV.position.y + plane_move_y + planeV.height;
		hx0 = helicopters[i].position.x + helicopters[i].speed; hx1 = helicopters[i].position.x + helicopters[i].speed + helicopters[i].width;
		// notes page 3
		if (helicopters[i].scale_factor == -1) {
			int temp = hx0;
			hx0 = hx1;
			hx1 = temp;
		}
		hy0 = helicopters[i].position.y; hy1 = helicopters[i].position.y + helicopters[i].height;

		is_x_have_collision = px1 >= hx0 && hx1 >= px0; is_y_have_collision = py1 >= hy0 && hy1 >= py0;
		
		//if both axis are true means there is a collision // intersected area info in case of intersection
		if (is_x_have_collision && is_y_have_collision) {
			int left, bottom, right, top;
			left = max(px0, hx0); right = min(px1, hx1);
			top = min(py1, hy1); bottom = max(py0, hy0);
			return checkIntersectedAreas(left, bottom, fabs(right - left) == 0 ? 1 : fabs(right - left), fabs(top-bottom) == 0 ? 1 : fabs(top-bottom));
		} } return false; }
 
/*
	if there intersections this functions uses more spesific scan on pixel colors with alpha blending
*/
bool checkIntersectedAreas(int xstart, int ystart, int partw, int parth) {
	int areasize = partw * parth;
	int neededspace = areasize * 4;
	float* pixel;
	pixel = (float*)malloc(neededspace * sizeof(float));
	if (!pixel)
	{
		cout << "memory allocation failed" << endl;
		exit(0);
	}
	glReadPixels(xstart, ystart, partw, parth, GL_RGBA, GL_FLOAT, pixel);
	
	for (int i = 1; i < neededspace + 1; i++)
	{
		if (pixel[i] == 0) return true;
	}
	return false;
}

/*
	RENDERS SCORE AND RE-PLAY INFO TO THE SCREEN
*/
void gameOver() {
	glLoadIdentity();
	glDisable(GL_BLEND);
	char buffer[100] = { '\0' };
	sprintf_s(buffer, "GAME OVER | SCORE:%d | 'ENTER' FOR PLAY AGAIN", score);
	renderBitmapString(40, 320, GLUT_BITMAP_9_BY_15, buffer);
	glEnable(GL_BLEND);
	if (!isPlayed) PlaySound("Sounds/game_over.wav", GetModuleHandle(NULL), SND_ASYNC | SND_FILENAME); isPlayed = true;
}

/*
	reverse axis in case of negative scaling
*/
void reverseTheAxis(int i ) {
	helicopters[i].width *= -1; // because head is changing axis so points must too 
	helicopters[i].position.x = TOTAL_WIDTH - helicopters[i].position.x; // primitive reflection
	helicopters[i].start_speed = -(helicopters[i].start_speed);
	helicopters[i].speed = 0;
}

