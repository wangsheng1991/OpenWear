#include "mrgl.h"
#define CV_USE_ALL_MODULES 1
#include "mropencv.h"
#include "Glasses.h"
#include "iostream"
#include "../sdm/include/ldmarkmodel.h"
using namespace std;

VideoCapture capture(0);
CGlasses m_glasses;
float smoothingeyecenterx = 0;
float smoothingeyecentery = 0;
float smoothingscale = 1.1;
ldmarkmodel modelt;
std::string modelFilePath = "roboman-landmark-model.bin";
cv::Mat current_shape;
float learningrate = 0.9;
void init(void)
{
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat light_pos[4];
	light_pos[0] = 0;
	light_pos[1] = 0;
	light_pos[2] = -2.0;
	light_pos[3] = 0;
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glEnable(GL_LIGHT0);
	m_glasses.ReadData();
	if(!load_ldmarkmodel(modelFilePath, modelt)){
		std::cout << "无法加载SDM模型" << std::endl;
	}
}

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

void drawVideo(const Mat &img)
{
	GLuint texName = matToTexture(img);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-3.2, -2.4, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-3.2, 2.4, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(3.2, 2.4, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(3.2, -2.4, 0.0);
	glEnd();
	glDeleteTextures(1, &texName);
}

float getDistance(Point p1, Point p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mat img;
	capture >> img;
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -4);
	glPushMatrix();
	modelt.track(img, current_shape);	
	if (current_shape.cols == 136)
	{
		cv::Vec3d eav;		
		modelt.drawPose(img, current_shape, eav);
		Point lefteye = Point((current_shape.at<float>(36) + current_shape.at<float>(39)) / 2,
			(current_shape.at<float>(36 + 68) + current_shape.at<float>(39 + 68)) / 2);
		Point righteye = Point((current_shape.at<float>(42) + current_shape.at<float>(45))/2,
			(current_shape.at<float>(42 + 68) + current_shape.at<float>(45 + 68)) / 2);
		cv::circle(img, lefteye, 2, cv::Scalar(0, 255, 0), -1);
		cv::circle(img, righteye, 2, cv::Scalar(0, 255, 0), -1);
		float eyecenterx = (lefteye.x + righteye.x) / 2;
		float eyecentery = (lefteye.y + righteye.y) / 2;
		smoothingeyecenterx = smoothingeyecenterx*(1 - learningrate) + learningrate*eyecenterx;
		smoothingeyecentery = smoothingeyecentery*(1 - learningrate) + learningrate*eyecentery;
		float distance = getDistance(lefteye, righteye);
		float scale = 0.008*distance + 0.24;
		smoothingscale = learningrate*scale + (1 - learningrate)*smoothingscale;
		cout << smoothingeyecenterx << "," << smoothingeyecentery << endl;
//		cout << eav << endl;
		glTranslatef((smoothingeyecenterx - 320) / 320 * 2, (240 - smoothingeyecentery) / 240 * 2, 0);
		glRotatef(0.5*eav[0], 1, 0,0);
 		glRotatef(-0.5*eav[1], 0, 1, 0);
 		glRotatef(-0.5*eav[2], 0, 0, 1);
		glScalef(smoothingscale, smoothingscale, smoothingscale);		
	}
	m_glasses.Draw();
// 	if (fds.size() > 0)
// 	{

// 		float distance=getDistance(fds[0].points[0], fds[0].points[1]);
// 		float scale = 0.008*distance+0.24;
// 		smoothingscale = smoothingscale*0.1 + 0.9*scale;
// //		cout << eyecenterx << "," << eyecentery << endl;
// //		cout << distance <<" "<<scale<< endl;
// 		glTranslatef((smoothingeyex - 320) / 320 * 2, (240 - smoothingeyey) / 240 * 2, 0);
// 		glScalef(smoothingscale, smoothingscale, smoothingscale);		
// 	}
	glPopMatrix();
	glPushMatrix();
	drawVideo(img);
	glPopMatrix();	
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300,200);
	glutCreateWindow("OpenWear");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}