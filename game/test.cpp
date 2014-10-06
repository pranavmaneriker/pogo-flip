//
// This code was created by Jeff Molofee '99 (ported to Linux/GLUT by Richard Campbell '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe 
// (email Richard Campbell at ulmont@bellsouth.net)
//
#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.
#include <unistd.h>     // needed to sleep.
#include <SOIL/SOIL.h>
#include<iostream>

/* ascii code for the escape key */
#define ESCAPE 27
using namespace std;
/* The number of our GLUT window */
int window; 

GLfloat rotx=0;
GLfloat roty=0;
GLfloat rotz=0;
GLfloat dist=-10.0f;
GLuint texture[1];




int LoadGLTextures()
{
	texture[0] = SOIL_load_OGL_texture
        (
        "test.jpg",//"Data/lesson6/NeHe.bmp",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
        ); 
    if(texture[0] == 0)
        return 0;
 
 
    // Typical Texture Generation Using Data From The Bitmap
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
 
    return 1;  
}

  void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
  {
      LoadGLTextures();				// Load The Texture(s) 
      glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping
      glClearColor(0.0f, 0.0f, 1.0f, 0.0f);	// Clear The Background Color To Blue 
      glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
      glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
      glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
      glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();				// Reset The Projection Matrix
      
      gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
      
      glMatrixMode(GL_MODELVIEW);
  }
  
/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
	Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();				// Reset The View

    glTranslatef(0.0f,0.0f,dist);              // move 5 units into the screen.
    
    glRotatef(rotx,1.0f,0.0f,0.0f);		// Rotate On The X Axis
    glRotatef(roty,0.0f,1.0f,0.0f);		// Rotate On The Y Axis
    glRotatef(rotz,0.0f,0.0f,1.0f);		// Rotate On The Z Axis

    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

    glBegin(GL_QUADS);		                // begin drawing a cube
    
    // Front Face (note that the texture's corners have to match the quad's corners)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    
    // Bottom Face       
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    
    glEnd();                                    // done with the polygon.


    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}

void keyPress(unsigned char key, int x, int y)
{
	cout<<"Key Pressed:"<<key<<endl;
	if(key == 27)
	{
		//glutDestroyWindow();
		exit(0);
	}
	else if(key == 'a')
	{
		roty=roty>359?roty-360:roty+2;
	}
	else if(key == 'd')
	{
		roty=roty>359?roty-360:roty-2;
	}

	else if(key == 's')
	{
		dist-=0.6;
	}
	else if(key == 'w')
	{
		dist+=0.6;
	}
	glutPostRedisplay();
}
void specialKeyPress(int key, int x,int y)
{

	cout<<"Key Pressed:"<<key<<endl;
	 if(key == GLUT_KEY_UP)
	{
		rotx=rotx>359?rotx-360:rotx-2;
	}
	else if(key == GLUT_KEY_DOWN)
	{
		rotx=rotx>359?rotx-360:rotx+2;
	}

	else if(key == GLUT_KEY_RIGHT)
	{	
		rotz=rotz>359?rotz-360:rotz-2;
	}
	else if(key == GLUT_KEY_LEFT)
	{
		rotz=rotz>359?rotz-360:rotz+2;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv) 
{  
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or 
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */  
    glutInit(&argc, argv);  

    /* Select type of Display mode:   
     Double buffer 
     RGBA color
     Alpha components supported 
     Depth buffer */  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

    /* get a 640 x 480 window */
    glutInitWindowSize(640, 480);  

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);  

    /* Open a window */  
    window = glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99");  

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);  

    /* Go fullscreen.  This is as soon as possible. */
    glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    //glutIdleFunc(&DrawGLScene);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPress);
    glutSpecialFunc(&specialKeyPress);

    /* Initialize our window. */
    InitGL(640,480);
  
    /* Start Event Processing Engine */  
    glutMainLoop();  

    return 1;
}


