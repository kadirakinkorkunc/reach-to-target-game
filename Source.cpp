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
int main_window;
int remainingLife = 3;
int score = 0;
int plane_move_x = 0, plane_move_y = 0;
int level_speed = 1;
int current_level = 1;
enum GAME_STATE { ON_START , ON_PLAY, ON_STOP};
GAME_STATE CURRENT_GAME_STATE = ON_START;
GLuint first_heli_id, second_heli_id, third_heli_id,fourth_heli_id, plane, start_button,resume_button, exit_button;
GLuint heli_id[] = { first_heli_id , second_heli_id, third_heli_id, fourth_heli_id };
bool isPlayed = false; // for preventing sound loop
bool isUpKeyPressed = false, isDownKeyPressed = false, isRightKeyPressed = false, isLeftKeyPressed = false;
bool isWelcomeScreenSoundPlayedRecently = false, isMainSoundPlaying = false;


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
void drawMenu();															// draws menu
void drawStartButton();														// draws start button in menu
void drawExitButton();														// draws exit button in menu
void drawResumeButton();													// draws resume button in menu
void mouseFunc(int bttn,int state,int x,int y);								// reflects against mouse actions
void showHelper();															// helper for ESC
void keyUp(int key, int x, int y);											// detects releasing a key
void moveHelis();															// moves helis 
void checkAndDoPlaneMove();													// moves plane via handlers
void playWelcomeScreenSound();												// plays welcome screen music
void playMainSound();														// plays main screen music
void showLevelBar();														// shows current level at left bottom corner

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
	int main_window = glutCreateWindow(
		"160322064-kadir akýn-korkunç");
	init2D();
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard_activity);
	glutKeyboardFunc(keyboard_activity2);
	glutSpecialUpFunc(keyUp);
	glutIdleFunc(idlefunc);
	glutTimerFunc(0,regular_flow,0);
	glutDisplayFunc(display);
	glutMouseFunc(mouseFunc);
	glutMainLoop();
}


/*
	check intersections
*/
void idlefunc() {
	if (CURRENT_GAME_STATE == ON_PLAY)
	{
		updateGameStatus();
	}
}

/*
	reset level and take 1 heart from user
*/
void updateGameStatus() {
	
	if (remainingLife == 0) {/*WAIT FOR ENTER*/}
	else {
		if (intersectCheck()) {
			randomizePlaneLocation(); 
			PlaySound("Sounds/plane_crash.wav",
				GetModuleHandle(NULL),
				SND_SYNC | SND_FILENAME );
			isMainSoundPlaying = false;
			remainingLife -= 1;
			plane_move_y = 0;
			plane_move_x = 0;
		}}}


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
	loadTexture("Images/start-button.png", start_button);
	loadTexture("Images/resume-button.png", resume_button);
	loadTexture("Images/exit-button.png", exit_button);
}


/*
	draw objects and manage them
*/
void display() {
	glClear(GL_COLOR_BUFFER_BIT); // clear screen
	if (CURRENT_GAME_STATE == ON_START || CURRENT_GAME_STATE == ON_STOP)
	{
		drawMenu();
	}
	else if(CURRENT_GAME_STATE == ON_PLAY){
		if (remainingLife > 0) {
			playMainSound();
			showLevelBar();
			showScoreboard();

			// draw helicopters
			drawHelis(helicopters);

			// draw plane
			drawPlane();

			// show esc option in the right corner
			showHelper();
		}
		else { gameOver(); }
	}
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


void playMainSound() {
	if (!isMainSoundPlaying)
	{
		PlaySound("Sounds/main_sound.wav", NULL, SND_LOOP | SND_ASYNC);
		isMainSoundPlaying = true;
	}
}

/*
	plays welcome screen music
*/
void playWelcomeScreenSound() {
	PlaySound("Sounds/start_sound.wav",
		GetModuleHandle(NULL),
		SND_ASYNC | SND_FILENAME);
}

/*
	draws all given objects with spesifications
*/
void drawHelis(Vehicle helicopters[] ) {
	
	for (int i = 0; i < 4 ; i++)
	{
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glRotatef(10.0, 0, 0, 1);

		glMatrixMode(GL_MODELVIEW);
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
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
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
reflects against mouse actions
*/
void mouseFunc(int button, int state, int x, int y) {
	if (CURRENT_GAME_STATE == ON_STOP || CURRENT_GAME_STATE == ON_START)
	{
		if (button == GLUT_LEFT_BUTTON && x > 100 && x < 400 && y > 160 && y < 220)
		{
			CURRENT_GAME_STATE = ON_PLAY;
		}
		if (button == GLUT_LEFT_BUTTON && x > 100 && x < 400 && y > 360 && y < 420)
		{
			glutDestroyWindow(main_window);
			exit(0);
		}
	}
	glutPostRedisplay();

}
/*
	draws start menu button
*/
void drawStartButton() {
	
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, start_button);
	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(100,400);
	glTexCoord2f(1, 0);
	glVertex2i(400,400);
	glTexCoord2f(1, 1);
	glVertex2i(400,500);
	glTexCoord2f(0, 1);
	glVertex2i(100,500);
	glEnd();
}

/*
draws resume button for menu
*/
void drawResumeButton() {
	
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, resume_button);
	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(100, 400);
	glTexCoord2f(1, 0);
	glVertex2i(400, 400);
	glTexCoord2f(1, 1);
	glVertex2i(400, 500);
	glTexCoord2f(0, 1);
	glVertex2i(100, 500);
	glEnd();
}

/*
draws exit button for menu
*/
void drawExitButton() {

	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, exit_button);
	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(100, 200);
	glTexCoord2f(1, 0);
	glVertex2i(400, 200);
	glTexCoord2f(1, 1);
	glVertex2i(400, 300);
	glTexCoord2f(0, 1);
	glVertex2i(100, 300);
	glEnd();
}

/*
draws menu and its content with it
*/
void drawMenu() {
	if (CURRENT_GAME_STATE == ON_START)
	{
		drawStartButton();
		if (!isWelcomeScreenSoundPlayedRecently)
		{
			playWelcomeScreenSound();
			isWelcomeScreenSoundPlayedRecently = true;
		}
	}
	else if (CURRENT_GAME_STATE == ON_STOP) {
		drawResumeButton();
	}
	drawExitButton();
}

/*
	this function increases coordinates for helicopter moves and resets, scales, recolors objects after reaching beyond wall limits
*/
void regular_flow(int what) {
	if (CURRENT_GAME_STATE == ON_PLAY)
	{
		moveHelis();
		checkAndDoPlaneMove();
	}
	glutTimerFunc(20, regular_flow, 0);
}


/*
in regular flow this function moves helis.
*/
void moveHelis() {
	for (int i = 0; i < 4; i++)
	{
		if (helicopters[i].scale_factor == 1 && helicopters[i].speed <= TOTAL_WIDTH)
		{
			helicopters[i].speed += helicopters[i].start_speed + level_speed;
		}
		else if (helicopters[i].scale_factor == -1 && helicopters[i].speed >= -TOTAL_WIDTH) {
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
		glutPostRedisplay();
	}


}

/*
this function handles smooth moving via key press handlers
*/
void checkAndDoPlaneMove() {
	if (isUpKeyPressed ) {
		plane_move_y += 5;
		checkLevelUpdate();
	}
	if (isDownKeyPressed && (plane_move_y + planeV.position.y) > 0) {
		plane_move_y -= 5;
	}
	if (isRightKeyPressed && (planeV.position.x + planeV.width + plane_move_x) < TOTAL_WIDTH) {
		plane_move_x += 5;
	}
	if (isLeftKeyPressed && (planeV.position.x + plane_move_x) > 0) {
		plane_move_x -= 5;
	}
	
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
	glColor3f(1, 0, 1);

	char buffer[20] = { '\0' };
	char buffer2[20] = { '\0' };
	sprintf_s(buffer, "SCORE:%d", score);
	sprintf_s(buffer2, "HEARTS:%d", remainingLife); 
	renderBitmapString(300,620,GLUT_BITMAP_9_BY_15, buffer);
	renderBitmapString(400,620,GLUT_BITMAP_9_BY_15, buffer2);

	glEnable(GL_BLEND);

}
/*
	shows users current level at left bottom corner
*/
void showLevelBar() {
	glLoadIdentity();
	glDisable(GL_BLEND);
	glColor3f(1, 0, 1);

	char buffer[30] = { '\0' };
	sprintf_s(buffer, "Level:%d", current_level);
	renderBitmapString(10, 10, GLUT_BITMAP_9_BY_15, buffer);

	glEnable(GL_BLEND);
}

/*
shows esc option in the right corner of the screen
*/
void showHelper() {

	glLoadIdentity();
	glDisable(GL_BLEND);
	glColor3f(1, 0, 1);

	char buffer[200] = { '\0' };
	sprintf_s(buffer, "press ESC to pause");
	renderBitmapString(TOTAL_WIDTH - 100, 10, GLUT_BITMAP_HELVETICA_10, buffer);

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
		isUpKeyPressed = true;
	}
	if (key == GLUT_KEY_DOWN ) {
		isDownKeyPressed = true;
	}
	if (key == GLUT_KEY_RIGHT) {
		isRightKeyPressed = true;
	}
	if (key == GLUT_KEY_LEFT ) {
		isLeftKeyPressed = true;
	}
 }

/*
	this function does required actions after keyboard activities ( ENTER)
*/
void keyboard_activity2(unsigned char key, int x, int y) {

	if (key == 13 && remainingLife == 0) {
		remainingLife = 3;
		plane_move_x = 0;
		plane_move_y = 0;
		score = 0;
		isPlayed = false;
		randomizePlaneLocation();
		glutPostRedisplay();
	}
	if (key == 27) // in case of ESCAPE press
	{
		if (CURRENT_GAME_STATE == ON_PLAY) 
			CURRENT_GAME_STATE = ON_STOP;
		else if(CURRENT_GAME_STATE == ON_STOP) 
			CURRENT_GAME_STATE = ON_PLAY;
	}
}

/*
	this function checks and re assigns the variables for plane move
*/
void keyUp(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		isUpKeyPressed = false;
	}
	if (key == GLUT_KEY_DOWN) {
		isDownKeyPressed = false;
		}
	if (key == GLUT_KEY_RIGHT ) {
		isRightKeyPressed = false;
		}
	if (key == GLUT_KEY_LEFT ) {
		isLeftKeyPressed = false;
		}
	glutPostRedisplay();
}


/*
this function checks if plane reached the target place
*/
void checkLevelUpdate() {
	if (plane_move_y+planeV.position.y > TOTAL_HEIGHT)
	{
		current_level++;
		randomizePlaneLocation();
		updateLevelSpeed();
		PlaySound("Sounds/level_up.wav", 
			GetModuleHandle(NULL),
			SND_SYNC | SND_FILENAME);
		score += 10;
		isMainSoundPlaying = false;
		plane_move_y = 0;
		plane_move_x = 0;


		
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
		px0 = planeV.position.x + plane_move_x + 10 ; px1 = planeV.position.x + plane_move_x + planeV.width - 10;
		py0 = planeV.position.y + plane_move_y + 5; py1  = planeV.position.y + plane_move_y + planeV.height - 5;
		hx0 = helicopters[i].position.x + helicopters[i].speed; hx1 = helicopters[i].position.x + helicopters[i].speed + helicopters[i].width;
		// notes page 3
		// revert heads and tails because of scaling
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
	glColor3f(1, 0, 1);

	char buffer[100] = { '\0' };
	sprintf_s(buffer, "GAME OVER | SCORE:%d | 'ENTER' FOR PLAY AGAIN", score);
	renderBitmapString(40, 320, GLUT_BITMAP_9_BY_15, buffer);
	glEnable(GL_BLEND);
	level_speed = 1;
	current_level = 1;
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

