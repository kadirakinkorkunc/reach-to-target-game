// collision, 3 hak bitince oyunu durdur
#include<glew.h>
#include<freeglut.h>
#include<iostream>
#include<SOIL2.h>
#include <random>
#include <functional>
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

void init2D();																// load images and start configurations
void loadTexture(const string& path, GLuint& object);						// create textures by using SOIL2 library
void display();																// draw needed objects
void drawHelis(Vehicle helicopters[]);										// draws helicopters
void drawPlane();															// draws plane 
void showScoreboard();														// draws scorboard to the screen
void regular_flow(int what);												// regular program flow 
void keyboard_activity(int key, int x, int y);								// keyboard listener
void keyboard_activity2(unsigned char key, int x, int y);					// keyboard listener
void reshape(int w, int h);													// reshape function for windows resizing.
void checkLevelUpdate();													// it checks plane location for leveling up
void updateLevelSpeed();													// after every level update the choppers speed
void randomizeColors(int i);												// generate new colors for textures
int randomizeScale();														// choose random flying direction
int randomizeStartSpeed();													// choose random start speed
bool intersectCheck();														// it checks collisions between plane and helicopters
void renderBitmapString(float x, float y,void* font, const char* string);	// helper for rendering characters

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(TOTAL_WIDTH, TOTAL_HEIGHT);
	glutCreateWindow("REACH TO THE TARGET");
	init2D();
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard_activity);
	glutKeyboardFunc(keyboard_activity2);
	glutTimerFunc(0,regular_flow,0);
	glutDisplayFunc(display);
	glutMainLoop();
}

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
	glClear(GL_COLOR_BUFFER_BIT); // ekraný temizliyoruz

	glLoadIdentity();
	glDisable(GL_BLEND);
	showScoreboard();
	glEnable(GL_BLEND);

	// draw helicopters
	drawHelis(helicopters);

	// draw plane
	drawPlane();

	glFlush(); // refresh screen
	glutSwapBuffers(); // change frame buffers
}

/*
	reshape for window sizing
*/
void reshape(int w, int h)
{
	if (h == 0) h = 1; // 0 division corner case
	GLfloat aspect = (GLfloat)TOTAL_WIDTH / (GLfloat)TOTAL_HEIGHT;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int width = h * aspect ;
	int left = (w - width) / 2;
	glViewport(left, 0, width, h);
	gluOrtho2D(0, TOTAL_WIDTH, 0,TOTAL_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

/*
	draws all given objects with spesifications
*/
void drawHelis(Vehicle helicopters[] ) {
	
	for (int current_heli = 0; current_heli < 4 ; current_heli++)
	{
		glColor3f(helicopters[current_heli].color.x, helicopters[current_heli].color.y, helicopters[current_heli].color.z);
		glLoadIdentity();
		glScalef(helicopters[current_heli].scale_factor, 1 , 1);
		glBindTexture(GL_TEXTURE_2D, heli_id[current_heli]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); 
				glVertex2i(helicopters[current_heli].position.x + helicopters[current_heli].speed, helicopters[current_heli].position.y);
			glTexCoord2f(1, 0); 
				glVertex2i(helicopters[current_heli].position.x + helicopters[current_heli].speed + helicopters[current_heli].width , helicopters[current_heli].position.y);
			glTexCoord2f(1, 1); 
				glVertex2i(helicopters[current_heli].position.x + helicopters[current_heli].speed + helicopters[current_heli].width, helicopters[current_heli].position.y + helicopters[current_heli].height);
			glTexCoord2f(0, 1); 
				glVertex2i(helicopters[current_heli].position.x + helicopters[current_heli].speed, helicopters[current_heli].position.y + helicopters[current_heli].height);
		glEnd();

	}
}


/*
	draws plane
*/
void drawPlane() {
	glColor3f(1, 1, 1);
	glLoadIdentity();	
	glBindTexture(GL_TEXTURE_2D, plane);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
			glVertex2i(planeV.position.x + plane_move_x, planeV.position.y + plane_move_y);
		glTexCoord2f(1, 0);
			glVertex2i(planeV.position.x + plane_move_x + planeV.width , planeV.position.y + plane_move_y);
		glTexCoord2f(1, 1);
			glVertex2i(planeV.position.x + plane_move_x + planeV.width, planeV.position.y + planeV.height + plane_move_y);
		glTexCoord2f(0, 1);
			glVertex2i(planeV.position.x + plane_move_x, planeV.position.y + planeV.height + plane_move_y);
	glEnd();
}

/*
	this function increases coordinates for helicopter moves and resets, scales, recolors objects after reaching beyond wall limits
*/
void regular_flow(int what) {
	for (int i = 0; i < 4; i++)
	{
		if (helicopters[i].speed < TOTAL_WIDTH)
		{
			helicopters[i].speed += (helicopters[i].start_speed + level_speed);
		}
		else {
			randomizeColors(i);
			helicopters[i].scale_factor = randomizeScale();
			helicopters[i].speed = helicopters[i].start_speed;
		}
		glutPostRedisplay();
	}
	if (intersectCheck())
	{
		cout << "collision detected!!" << endl;
	}
	glutTimerFunc(15, regular_flow, 0);
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
	cout << "plane spawn pos x: " << planeV.position.x << " || plane spawn pos y: " << planeV.position.y << endl;
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
	char buffer[20] = { '\0' };
	char buffer2[20] = { '\0' };
	sprintf_s(buffer, "SCORE:%d", score);
	sprintf_s(buffer2, "HEARTS:%d", remainingLife); 
	renderBitmapString(300,620,GLUT_BITMAP_9_BY_15, buffer);
	renderBitmapString(400,620,GLUT_BITMAP_9_BY_15, buffer2);

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
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y  | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	);
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
	if (key == GLUT_KEY_DOWN) {
		//plane_move_y -= 0.03f;
		plane_move_y -= 10;
	}
	if (key == GLUT_KEY_RIGHT) {
		plane_move_x += 10;
	}
	if (key == GLUT_KEY_LEFT) {
		plane_move_x -= 10;
	}
	
	glutPostRedisplay();

}

/*
	this function does required actions after keyboard activities ( ENTER)
*/
void keyboard_activity2(unsigned char key, int x, int y) {
	if (key == 13 && remainingLife == 0) {
		cout << "enter" << endl;
		randomizePlaneLocation();
		plane_move_x = 0;
		plane_move_y = 0;
		score = 0;
		glutPostRedisplay();
	}
}

/*
this function checks if plane reached the target place
*/
void checkLevelUpdate() {
	if (plane_move_y + planeV.position.y > TOTAL_HEIGHT)
	{
		updateLevelSpeed();
		cout << "LEVEL UP" << endl;
		score += 10;
		randomizePlaneLocation();
		plane_move_y = 0;
		plane_move_x = 0;
	}
}

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
	bool is_x_have_collision;
	bool is_y_have_collision;
	// in x-axis
	for (int current_heli = 0; current_heli < 4; current_heli++) {
		is_x_have_collision = planeV.position.x + planeV.width + plane_move_x >= helicopters[current_heli].position.x + helicopters[current_heli].speed && helicopters[current_heli].position.x +  helicopters[current_heli].width + helicopters[current_heli].speed >= planeV.position.x+ plane_move_x;
		is_y_have_collision = planeV.position.y + planeV.height + plane_move_y >= helicopters[current_heli].position.y && helicopters[current_heli].position.y + helicopters[current_heli].width >= planeV.position.y + plane_move_y;
		if (is_x_have_collision && is_y_have_collision) return true;
	}

	//if both axis are true means there is a collision 
	return is_x_have_collision && is_y_have_collision;
}