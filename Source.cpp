#include<glew.h>
#include<freeglut.h>
#include<iostream>
#include<SOIL2.h>
#include <time.h>
#include <string.h>
using namespace std;


int total_width = 480, total_height = 640;
int plane_area_width = 480, plane_area_height = 240;

GLuint first_heli, second_heli, third_heli,fourth_heli, plane;

static GLfloat plane_move_x = 0, plane_move_y = 0;
static GLfloat heli_move_at_x = 0;
float level_speed = 3;



void loadTexture(const string& path, GLuint& object);
void init2D(); // load images and start configurations
void display(); // draw needed objects
void keyboard_activity(int key, int x, int y); // keyboard listener
void updateLevelSpeed(); // after every level update the choppers speed
void heli_movement(int what); // regular program flow for chopper 
void randomizeColors(); // generate new colors for textures
int randomizeScale(); // choose flow size
GLfloat r, g, b;


int main(int argc, char* argv[]) {
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(total_width, total_height);
	glutCreateWindow("aa");
	init2D();
	glutSpecialFunc(keyboard_activity);
	glutTimerFunc(0,heli_movement,0);
	glutDisplayFunc(display);
	glutMainLoop();
}
int randomizeScale() {
	int randompick = rand() % 2;
	if (randompick == 1)
	{
		return -1;
	}
	else {
		return 1;
	}

}
void randomizeColors() {
	r = ((double)rand() / (RAND_MAX));
	g = ((double)rand() / (RAND_MAX));
	b = ((double)rand() / (RAND_MAX));
	cout << r << ": " << g << ": " << b << endl;
}

void heli_movement(int what) {
	if (heli_move_at_x < total_width)
	{
		heli_move_at_x += level_speed;
	}
	else {
		heli_move_at_x = 0;
		randomizeColors();
	}
	glutPostRedisplay();
	glutTimerFunc(15, heli_movement, 0);
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT); // ekraný temizliyoruz
	/*
		heli 1
	*/
	glMatrixMode(GL_PROJECTION);
	glColor3f(r, g, b);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, first_heli);
//	glScalef(randomizeScale(), 1, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0,0); 
		glVertex2i(0 + heli_move_at_x, 525 );
		glTexCoord2f(1, 0); 
		glVertex2i(50 + heli_move_at_x, 525 );
		glTexCoord2f(1, 1); 
		glVertex2i(50 + heli_move_at_x, 575 );
		glTexCoord2f(0, 1);
		glVertex2i(0 + heli_move_at_x, 575 );
	glEnd();

	/*
	heli 2
	*/
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, second_heli);
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2i(0 + heli_move_at_x, 430);
		glTexCoord2f(1, 0);
		glVertex2i(50 + heli_move_at_x, 430);
		glTexCoord2f(1, 1);
		glVertex2i(50 + heli_move_at_x, 480);
		glTexCoord2f(0, 1);
		glVertex2i(0 + heli_move_at_x, 480);
	glEnd();

	/*
	heli 3
	*/
	glColor3f(r, g, b);
	glLoadIdentity();

	//glTranslatef(-0.9f, 0.5f, 0.0f);
	//glTranslatef(heli_move_at_x, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, third_heli);
	glScalef(-1, 1, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
		glVertex2i(0 + heli_move_at_x, 335);
		glTexCoord2f(1, 0);
		glVertex2i(50 + heli_move_at_x, 335);
		glTexCoord2f(1, 1);
		glVertex2i(50 + heli_move_at_x, 385);
		glTexCoord2f(0, 1);
		glVertex2i(0 + heli_move_at_x, 385);
	glEnd();

	/*
	heli 4
	*/
	glColor3f(r, g, b);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, fourth_heli);
	glScalef(-1, 1, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2i(0 + heli_move_at_x, 240);
		glTexCoord2f(1, 0);
		glVertex2i(50 + heli_move_at_x, 240);
		glTexCoord2f(1, 1);
		glVertex2i(50 + heli_move_at_x, 290);
		glTexCoord2f(0, 1);
		glVertex2i(0 + heli_move_at_x, 290);
	glEnd();

	

	/*
	  plane
	*/
	glColor3f(1,1,1);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, plane);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2i(215 + plane_move_x, 10 + plane_move_y);
		glTexCoord2f(1, 0);
		glVertex2i(265 + plane_move_x, 10 + plane_move_y);
		glTexCoord2f(1, 1);
		glVertex2i(265 + plane_move_x, 60 + plane_move_y);
		glTexCoord2f(0, 1);
		glVertex2i(215 + plane_move_x, 60 + plane_move_y);
	glEnd();



	glFlush(); // ekraný yenile
	glutSwapBuffers(); // yeni çizim yaptýktan sonra frame bufferlarý deðiþ.
}

void updateLevelSpeed() {
	level_speed += 2;
}

// create plane object 
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
void loadTexture2() {
	int width;
	int height;
	int channels;
	unsigned char* image = SOIL_load_image("Images/heli3.png", &width, &height, &channels, SOIL_LOAD_RGBA); // image load from dir with params

	if (image == NULL) {
		throw(std::string("FAILED TO LOAD IMAGE"));
	}

	glGenTextures(1, &first_heli);
	glBindTexture(GL_TEXTURE_2D, first_heli);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
}
*/


void init2D() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1, 1, 1, 0.5);
	gluOrtho2D(0, total_width, 0, total_height);
	loadTexture("Images/heli1.png", first_heli);
	loadTexture("Images/heli2.png", second_heli);
	loadTexture("Images/heli3.png", third_heli);
	loadTexture("Images/heli4.png", fourth_heli);
	loadTexture("Images/plane.png", plane);

}


// ekrana çizimleri gerçekleþtiriyoruz

void keyboard_activity(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		plane_move_y += 5;
	}
	if (key == GLUT_KEY_DOWN) {
		//plane_move_y -= 0.03f;
		plane_move_y -= 5;
	}
	if (key == GLUT_KEY_RIGHT) {
		plane_move_x += 5;
	}
	if (key == GLUT_KEY_LEFT) {
		plane_move_x -= 5;
	}
	glutPostRedisplay();

}