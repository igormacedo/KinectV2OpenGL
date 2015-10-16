#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "KinectHandler.h"
#include <iostream>

using namespace std;

#define screen_width 1024
#define screen_height 848
#define color_width 1920
#define color_height 1080
#define depth_width 512
#define depth_height 424

GLuint window, sub;

KinectHandler* kinect = new KinectHandler();
RGBQUAD* ColorData = NULL;
RGBQUAD* DepthGrayScaleData = NULL;
UINT16* DepthBuffer = NULL;
GLuint textureColor, textureDepth;

int one = 1;

int t = 0;

void display();

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(screen_width, screen_height);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	window = glutCreateWindow("OpenGL First Window");

	glClearColor(0.5f, 0.5f, 0.5f, 1); //Set background color to gray
	glClearDepth(1.0f);
	glEnable(GL_TEXTURE_2D); // Enables 2D textures


	//Generate texture color id
	glGenTextures(1, &textureColor);
	glBindTexture(GL_TEXTURE_2D, textureColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, color_width, color_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)ColorData); // Creates the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generates texture depth id
	glGenTextures(1, &textureDepth);
	glBindTexture(GL_TEXTURE_2D, textureDepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depth_width, depth_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)DepthGrayScaleData); // Creates the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	glViewport(0, 0, screen_width, screen_height);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0, 1.0, 0, 1.0, 0, 1.0); //Defines the space in which the polygons are created
	//gluPerspective((float)60, screen_width / (GLdouble)screen_height, 0.1, 1000);
	gluLookAt(-0.3, 0, -1, 0, 0, 2, 0, 1, 0);

	
	glMatrixMode(GL_TEXTURE); // The next three lines invert the texture for visualization purposes, othewise it will be upside-down.
	glLoadIdentity();
	glScalef(1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	GLfloat mat_amb_difuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 0.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_difuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	GLfloat light_ambient_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_position[] = { -0.3f, 0.0f, -1.5f, 0.0f };
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light_ambient_diffuse);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//glEnable(GL_COLOR_MATERIAL);

	kinect->KinectInit();
	glutMainLoop();
}

void rotateCamera() {
	static double angle = 0.;
	static double radius = 2.;
	double x = radius*sin(angle);
	double z = radius*(1 - cos(angle)) - radius / 2;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, 0, z, 0, 0, radius, 0, 1, 0);
	cout << "Look at " << x << ' ' << 0 << ' ' << z << endl;
	angle -= 0.05;
}

void display()
{
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1.0, 0, 1.0, 0, 1.0);	

	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

	/*if (ColorData && DepthGrayScaleData && DepthBuffer)*/	one = 0;

	/*if (one)*/ kinect->GetColorAndDepth(ColorData, DepthGrayScaleData , DepthBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);


	glBindTexture(GL_TEXTURE_2D, textureColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, color_width, color_height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)ColorData);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 	glVertex2f(0.0f, 0.75f);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(0.37f, 0.75f);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(0.37f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.0f, 1.0f);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, textureDepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depth_width, depth_height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)DepthGrayScaleData);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 	glVertex2f(0.75f, 0.75f);	
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(1.0f, 0.75);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.75f, 1.0f);	
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((float)60, screen_width/(GLdouble)screen_height, 0.001, 1000);
	gluLookAt(-0.3, 0, -1, 0, 0, 2, 0, 1, 0);
	//rotateCamera();


	glPointSize(1);
	if (DepthBuffer != NULL)
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		cout << "======================NEW FRAME===============================" << endl;
		for (int i = 0; i < depth_height; i++)
		{
			for (int j = 0; j < depth_width; j++)
			{

				DepthSpacePoint depthSpacePoint = { static_cast<float>(j), static_cast<float>(i) };
				UINT16 depth = DepthBuffer[i * depth_width + j];
				ColorSpacePoint colorSpacePoint = { 0.0f, 0.0f };
				kinect->m_pCoordinateMapper->MapDepthPointToColorSpace(depthSpacePoint, depth, &colorSpacePoint);
				int colorX = static_cast<int>(std::floor(colorSpacePoint.X + 0.5f));
				int colorY = static_cast<int>(std::floor(colorSpacePoint.Y + 0.5f));

				if ((0 <= colorX) && (colorX < color_width) && (0 <= colorY) && (colorY < color_height)){
					RGBQUAD color = ColorData[colorY * color_width + colorX];
					//cout << "R" << (int) color.rgbRed << " G" << (int)color.rgbGreen << " B" << (int)color.rgbBlue;
					//glColor3i((int)color.rgbRed, (int)color.rgbGreen, (int)color.rgbBlue);

					CameraSpacePoint cameraSpacePoint = { 0.0f, 0.0f, 0.0f };
					kinect->m_pCoordinateMapper->MapDepthPointToCameraSpace(depthSpacePoint, depth, &cameraSpacePoint);
					glBegin(GL_POINTS);
						glVertex3f(cameraSpacePoint.X * 5, cameraSpacePoint.Y * 5 -3, cameraSpacePoint.Z * 5 + 5);
					glEnd();
				}

				//CameraSpacePoint cameraSpacePoint = { 0.0f, 0.0f, 0.0f };
				//kinect->m_pCoordinateMapper->MapDepthPointToCameraSpace(depthSpacePoint, depth, &cameraSpacePoint);


				//if ((0 <= colorX) && (colorX < color_width) && (0 <= colorY) && (colorY < color_height)){
				//	glBegin(GL_POINTS);
				//		glVertex3f(cameraSpacePoint.X*4, cameraSpacePoint.Y*4, cameraSpacePoint.Z*4 + 5);
				//		//float color[4];
				//		//glGetFloatv(GL_CURRENT_COLOR, color);
				//		//cout << "color: " << color[0] << " " << color[1] << " " << color[2] << " " << color[3] << endl;
				//		//cout << " X:" << cameraSpacePoint.X << " Y:" << cameraSpacePoint.Y << " Z:" << -cameraSpacePoint.Z << endl;
				//	glEnd();
				//}
			}
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glPointSize(10);
	
	glBegin(GL_POINTS);
		glColor3i(255, 0, 0);
		glVertex3f(0, 0, 0);
	glEnd();
	
	glutSwapBuffers();
}