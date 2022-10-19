/************************************************************************
	 File:        TrainView.cpp

	 Author:
				  Michael Gleicher, gleicher@cs.wisc.edu

	 Modifier
				  Yu-Chi Lai, yu-chi@cs.wisc.edu

	 Comment:
						The TrainView is the window that actually shows the
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within
						a TrainWindow
						that is the outer window with all the widgets.
						The TrainView needs
						to be aware of the window - since it might need to
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know
						about it (beware circular references)

	 Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"

#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif


//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l)
	: Fl_Gl_Window(x, y, w, h, l)
	//========================================================================
{
	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);

	resetArcball();
	t_time = 0;
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event))
			return 1;

	// remember what button was used
	static int last_push;

	switch (event) {
		// Mouse button being pushed event
	case FL_PUSH:
		last_push = Fl::event_button();
		// if the left button be pushed is left mouse button
		if (last_push == FL_LEFT_MOUSE) {
			doPick();
			damage(1);
			return 1;
		};
		break;

		// Mouse button release event
	case FL_RELEASE: // button release
		damage(1);
		last_push = 0;
		return 1;

		// Mouse button drag event
	case FL_DRAG:

		// Compute the new control point position
		if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
			ControlPoint* cp = &m_pTrack->points[selectedCube];

			double r1x, r1y, r1z, r2x, r2y, r2z;
			getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

			double rx, ry, rz;
			mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
				static_cast<double>(cp->pos.x),
				static_cast<double>(cp->pos.y),
				static_cast<double>(cp->pos.z),
				rx, ry, rz,
				(Fl::event_state() & FL_CTRL) != 0);

			cp->pos.x = (float)rx;
			cp->pos.y = (float)ry;
			cp->pos.z = (float)rz;
			damage(1);
		}
		break;

		// in order to get keyboard events, we need to accept focus
	case FL_FOCUS:
		return 1;

		// every time the mouse enters this window, aggressively take focus
	case FL_ENTER:
		focus(this);
		break;

	case FL_KEYBOARD:
		int k = Fl::event_key();
		int ks = Fl::event_state();
		if (k == 'p') {
			// Print out the selected control point information
			if (selectedCube >= 0)
				printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
					selectedCube,
					m_pTrack->points[selectedCube].pos.x,
					m_pTrack->points[selectedCube].pos.y,
					m_pTrack->points[selectedCube].pos.z,
					m_pTrack->points[selectedCube].orient.x,
					m_pTrack->points[selectedCube].orient.y,
					m_pTrack->points[selectedCube].orient.z);
			else
				printf("Nothing Selected\n");

			return 1;
		};
		break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, .3f, 0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200, 10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this, aspect);
#endif
	}
}

Pnt3f cubicSpline(Pnt3f g[4], float m[16], float t)
{
	glm::mat4x4 M = glm::make_mat4x4(m);
	glm::vec4 T(std::pow(t, 3), std::pow(t, 2), t, 1);
	glm::mat3x4 G(
		g[0].x, g[1].x, g[2].x, g[3].x,
		g[0].y, g[1].y, g[2].y, g[3].y,
		g[0].z, g[1].z, g[2].z, g[3].z);
	auto Q = T * (M * G);
	Pnt3f qt(Q[0], Q[1], Q[2]);
	return qt;
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	drawTrack(doingShadows);

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif

	drawTrain(doingShadows);
}

void TrainView::
drawTrack(bool doingShadows)
{
	//Spline Type
	switch (tw->splineBrowser->value())
	{
	case 1:				//Linear
	{
		float matrix[16] =
		{
			0, 0, 0, 0,
			0, 0, -1, 1,
			0, 0, 1, 0,
			0, 0, 0, 0
		};
		memcpy(m, matrix, sizeof(float) * 16);
		break;
	}
	case 2:				//Cardinal Cubic
	{
		float matrix[16] =
		{
			-1, 2, -1, 0,
			3, -5, 0, 2,
			-3, 4, 1, 0,
			1, -1, 0, 0
		};
		for (int i = 0; i < 16; i++)
			matrix[i] /= 2;
		memcpy(m, matrix, sizeof(float) * 16);
		break;
	}
	case 3:				//Cubic B-Spline
	{
		float matrix[16] =
		{
			-1, 3, -3, 1,
			3, -6, 0, 4,
			-3, 3, 3, 1,
			1, 0, 0, 0
		};
		for (int i = 0; i < 16; i++)
			matrix[i] /= 6;
		memcpy(m, matrix, sizeof(float) * 16);
		break;
	}
	default:
		break;
	}


	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		// pos
		Pnt3f pos[4];
		for (int n = 0; n < 4; n++)
			pos[n] = m_pTrack->points[(i + n) % m_pTrack->points.size()].pos;
		// orient
		Pnt3f orient[4];
		for (int n = 0; n < 4; n++)
			orient[n] = m_pTrack->points[(i + n) % m_pTrack->points.size()].orient;

		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		Pnt3f qt = cubicSpline(pos, m, t);

		//initialize¡K
		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			Pnt3f qt0 = qt;
			t += percent;
			qt = cubicSpline(pos, m, t);
			Pnt3f qt1 = qt;
			/*glLineWidth(3);
			glBegin(GL_LINES);
			if (!doingShadows)
				glColor3ub(32, 32, 64);
			glVertex3f(qt0.x, qt0.y, qt0.z);
			glVertex3f(qt1.x, qt1.y, qt1.z);
			glEnd();
			glLineWidth(1);*/

			// cross
			Pnt3f orient_t = cubicSpline(orient, m, t);
			orient_t.normalize();
			Pnt3f cross_t = (qt1 - qt0) * orient_t;
			cross_t.normalize();
			cross_t = cross_t * 2.5f;

			float railWidth = 0.2;
			Pnt3f cross_t_R = cross_t * (1 + railWidth);
			Pnt3f cross_t_L = cross_t * (1 - railWidth);

			static Pnt3f crossR0[2] = { qt0 + cross_t_R, qt0 - cross_t_R }, crossL0[2] = { qt0 + cross_t_L, qt0 - cross_t_L };
			Pnt3f crossR1[2] = { qt1 + cross_t_R, qt1 - cross_t_R }, crossL1[2] = { qt1 + cross_t_L, qt1 - cross_t_L };

			for (int n = 0; n < 2; n++)
			{
				glBegin(GL_POLYGON);
				if (!doingShadows)
					glColor3ub(32, 32, 64);
				glVertex3f(crossR0[n].x, crossR0[n].y, crossR0[n].z);
				glVertex3f(crossR1[n].x, crossR1[n].y, crossR1[n].z);
				glVertex3f(crossL1[n].x, crossL1[n].y, crossL1[n].z);
				glVertex3f(crossL0[n].x, crossL0[n].y, crossL0[n].z);
				glEnd();
			}

			crossR0[0] = crossR1[0];
			crossR0[1] = crossR1[1];
			crossL0[0] = crossL1[0];
			crossL0[1] = crossL1[1];
		}
	}
}

void TrainView::
drawTrain(bool doingShadows)
{
	int i = m_pTrack->trainU;
	float t = m_pTrack->trainU - i;
	
	Pnt3f g[4];
	for (int n = 0; n < 4; n++)
		g[n] = m_pTrack->points[(i + n) % m_pTrack->points.size()].pos;
	Pnt3f trainPos = cubicSpline(g, m, t);
	for (int n = 0; n < 4; n++)
		g[n] = m_pTrack->points[(i + n) % m_pTrack->points.size()].orient;
	Pnt3f trainOrient = cubicSpline(g, m, t);

	trainOrient.normalize();

	if (!tw->trainCam->value())
	{

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(trainPos.x - 5, trainPos.y, trainPos.z - 5);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(trainPos.x + 5, trainPos.y, trainPos.z - 5);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(trainPos.x + 5, trainPos.y + 10, trainPos.z - 5);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(trainPos.x - 5, trainPos.y + 10, trainPos.z - 5);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(trainPos.x - 5, trainPos.y, trainPos.z + 5);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(trainPos.x + 5, trainPos.y, trainPos.z + 5);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(trainPos.x + 5, trainPos.y + 10, trainPos.z + 5);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(trainPos.x - 5, trainPos.y + 10, trainPos.z + 5);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(trainPos.x - 5, trainPos.y, trainPos.z - 5);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(trainPos.x + 5, trainPos.y, trainPos.z - 5);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(trainPos.x + 5, trainPos.y, trainPos.z + 5);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(trainPos.x - 5, trainPos.y, trainPos.z + 5);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(trainPos.x - 5, trainPos.y + 10, trainPos.z - 5);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(trainPos.x + 5, trainPos.y + 10, trainPos.z - 5);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(trainPos.x + 5, trainPos.y + 10, trainPos.z + 5);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(trainPos.x - 5, trainPos.y + 10, trainPos.z + 5);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(trainPos.x - 5, trainPos.y, trainPos.z - 5);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(trainPos.x - 5, trainPos.y + 10, trainPos.z - 5);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(trainPos.x - 5, trainPos.y + 10, trainPos.z + 5);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(trainPos.x - 5, trainPos.y, trainPos.z + 5);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(trainPos.x + 5, trainPos.y, trainPos.z - 5);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(trainPos.x + 5, trainPos.y + 10, trainPos.z - 5);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(trainPos.x + 5, trainPos.y + 10, trainPos.z + 5);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(trainPos.x + 5, trainPos.y, trainPos.z + 5);
		glEnd();
	}
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();

	// where is the mouse?
	int mx = Fl::event_x();
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my),
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		glLoadName((GLuint)(i + 1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3] - 1;
	}
	else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n", selectedCube);
}