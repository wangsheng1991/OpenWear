#define _CRT_SECURE_NO_WARNINGS
#include "mrgl.h"
#include "mropencv.h"
VideoCapture capture(0);
GLuint matToTexture(const Mat&img)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	cvtColor(img, img, CV_BGR2RGB);
	flip(img, img, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
	return textureID;
}

void drawFrame()
{
	glPushMatrix();	
	Mat img;
	capture >> img;
	GLuint texture = matToTexture(img);
	glTranslatef(0.0f, 0.0f, -3.6f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-3.2, -2.4, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-3.2, 2.4, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(3.2, 2.4, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(3.2, -2.4, 0.0);
	glEnd();
	glPopMatrix();
	glDeleteTextures(1, &texture);
	glutSwapBuffers();
}

void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	drawFrame();
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("MRCVGL");
	glEnable(GL_TEXTURE_2D);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}