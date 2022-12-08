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
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>
#include <time.h>
#include <math.h>

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"

#define PI 3.14159265

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
			pickSurface();
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
		static int lastW = w(), lastH = h();

		if (!this->framebuffer)
		{
			this->framebuffer = new FBO;
			//gen framebuffer
			glGenFramebuffers(1, &this->framebuffer->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);
			//gen texture
			glGenTextures(1, this->framebuffer->textures);
			glBindTexture(GL_TEXTURE_2D, this->framebuffer->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->framebuffer->textures[0], 0);
			//gen rbo
			glGenRenderbuffers(1, &this->framebuffer->rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, this->framebuffer->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->framebuffer->rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				std::cout << "FBO complite\n";
			else
				std::cout << "FBO uncomplite\n";
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		//use frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);
		//update w & h
		if (lastW != w() || lastH != h())
		{
			glBindTexture(GL_TEXTURE_2D, this->framebuffer->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, this->framebuffer->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h());
		}

		if (!this->frame)
		{
			GLfloat frameVertices[12] =
			{
				1.0f, -1.0f,
				-1.0f, -1.0f,
				-1.0f, 1.0f,

				1.0f, 1.0f,
				1.0f, -1.0f,
				-1.0f, 1.0f
			};
			this->frame = new VAO;
			glGenVertexArrays(1, &this->frame->vao);
			glGenBuffers(2, this->frame->vbo);

			glBindVertexArray(this->frame->vao);
			glBindBuffer(GL_ARRAY_BUFFER,this->frame->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), frameVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);
		}

		if (!this->frameShader)
		{
			this->frameShader = new
				Shader(
					"../shaders/frame.vert",
					nullptr, nullptr, nullptr,
					"../shaders/frame.frag");

			this->frameShader->Use();
			glUniform1i(glGetUniformLocation(frameShader->Program, "tex"), 0);
			Shader::Unuse();
		}

		if (!this->pixelation)
		{
			this->pixelation = new
				Shader(
					"../shaders/pixelation.vert",
					nullptr, nullptr, nullptr,
					"../shaders/pixelation.frag");

			this->pixelation->Use();
			glUniform1i(glGetUniformLocation(pixelation->Program, "tex"), 0);
			Shader::Unuse();
		}

		if (!this->waterSinShader)
			this->waterSinShader = new
			Shader(
				"../shaders/waterSinWave.vert",
				nullptr, nullptr, nullptr,
				"../shaders/waterSinWave.frag");
		if (!this->waterHeightShader)
			this->waterHeightShader = new
			Shader(
				"../shaders/waterHeightMap.vert",
				nullptr, nullptr, nullptr,
				"../shaders/waterHeightMap.frag");
		if (!this->waterSimShader)
			this->waterSimShader = new
			Shader(
				"../shaders/waterSimulate.vert",
				nullptr, nullptr, nullptr,
				"../shaders/waterSimulate.frag");

		if (!this->commom_matrices)
		{
			this->commom_matrices = new UBO();
			this->commom_matrices->size = 2 * sizeof(glm::mat4);
			glGenBuffers(1, &this->commom_matrices->ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
			glBufferData(GL_UNIFORM_BUFFER, this->commom_matrices->size, NULL, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		if (!this->waterSurface) {

			std::vector<GLuint>	waterElement((widthDivid - 1) * (heightDivid - 1) * 2 * 3);
			std::vector<GLfloat>  waterTextureCoordinate((widthDivid * heightDivid) * 2);
			waterVertices = new GLfloat[widthDivid * heightDivid * 3];
			waterNormal = new GLfloat[widthDivid * heightDivid * 3];

			for (int i = 0; i < heightDivid; i++)
			{
				for (int j = 0; j < widthDivid; j++)
				{
					waterVertices[(i * widthDivid + j) * 3 + 0] = (float)j * widthLong / (widthDivid - 1) - (widthLong / 2);
					waterVertices[(i * widthDivid + j) * 3 + 1] = 0.0f;
					waterVertices[(i * widthDivid + j) * 3 + 2] = (float)i * heightLong / (heightDivid - 1) - (heightLong / 2);

					waterNormal[(i * widthDivid + j) * 3 + 0] = 0.0f;
					waterNormal[(i * widthDivid + j) * 3 + 1] = 1.0f;
					waterNormal[(i * widthDivid + j) * 3 + 2] = 0.0f;

					waterTextureCoordinate[(i * widthDivid + j) * 2 + 0] = (1.0f / (heightDivid - 1)) * i;
					waterTextureCoordinate[(i * widthDivid + j) * 2 + 1] = (1.0f / (widthDivid - 1)) * j;

					if (i + 1 != heightDivid && j + 1 != widthDivid)
					{
						waterElement[(i * (widthDivid - 1) + j) * 3 + 0] = i * widthDivid + j;
						waterElement[(i * (widthDivid - 1) + j) * 3 + 1] = i * widthDivid + j + 1;
						waterElement[(i * (widthDivid - 1) + j) * 3 + 2] = (i + 1) * widthDivid + j;

						waterElement[((widthDivid - 1) * (heightDivid - 1) * 3) + (i * (widthDivid - 1) + j) * 3 + 0] = (i + 1) * widthDivid + j + 1;
						waterElement[((widthDivid - 1) * (heightDivid - 1) * 3) + (i * (widthDivid - 1) + j) * 3 + 1] = (i + 1) * widthDivid + j;
						waterElement[((widthDivid - 1) * (heightDivid - 1) * 3) + (i * (widthDivid - 1) + j) * 3 + 2] = i * widthDivid + j + 1;
					}
				}
			}

			this->waterSurface = new VAO;
			this->waterSurface->element_amount = waterElement.size();
			glGenVertexArrays(1, &this->waterSurface->vao);
			glGenBuffers(3, this->waterSurface->vbo);
			glGenBuffers(1, &this->waterSurface->ebo);

			glBindVertexArray(this->waterSurface->vao);

			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->waterSurface->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, widthDivid * heightDivid * 3 * sizeof(float), waterVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Normal attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->waterSurface->vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, widthDivid * heightDivid * 3 * sizeof(float), waterNormal, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);

			// Texture Coordinate attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->waterSurface->vbo[2]);
			glBufferData(GL_ARRAY_BUFFER, (waterTextureCoordinate.size() * sizeof(float)), &waterTextureCoordinate[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(2);

			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->waterSurface->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (waterElement.size() * sizeof(int)), &waterElement[0], GL_STATIC_DRAW);

			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->waterTex)
			this->waterTex = new Texture2D("../Images/church.png");

		if (!this->heightMapTex)
		{
			this->heightMapTex = new std::vector<Texture2D*>(200);
			for (int i = 0; i < 200; i++)
			{
				std::string num = std::to_string(i);
				if (num.size() == 1) num = "00" + num;
				else if (num.size() == 2) num = "0" + num;
				std::string path = std::string("../Images/waves5/");
				path = path + num + ".png";
				this->heightMapTex->at(i) = new Texture2D(path.c_str(),Texture2D::TEXTURE_HEIGHT);
			}
		}

		if (!this->waterFBO0)
		{
			this->waterFBO0 = new FBO;
			//gen framebuffer
			glGenFramebuffers(1, &this->waterFBO0->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO0->fbo);
			//gen texture
			glGenTextures(1, this->waterFBO0->textures);
			glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterFBO0->textures[0], 0);
			//gen rbo
			//glGenRenderbuffers(1, &this->waterFBO0->rbo);
			//glBindRenderbuffer(GL_RENDERBUFFER, this->waterFBO0->rbo);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, widthDivid, heightDivid);
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->waterFBO0->rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				std::cout << "water FBO0 complite\n";
			else
				std::cout << "water FBO0 uncomplite\n";

			glClearColor(0.5f, 0, 0, 0);		// background should be black
			glClear(GL_COLOR_BUFFER_BIT);

		}
		//glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO0->fbo);
		if (lastW != w() || lastH != h())
		{
			glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO0->fbo);
			glClearColor(0.5f, 0, 0, 0);		// background should be black
			//glClearColor(0.5, 0, 0, 0);		// background should be black
			glClear(GL_COLOR_BUFFER_BIT);
		}


		if (!this->waterFBO1)
		{
			this->waterFBO1 = new FBO;
			//gen framebuffer
			glGenFramebuffers(1, &this->waterFBO1->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO1->fbo);
			//gen texture
			glGenTextures(1, this->waterFBO1->textures);
			glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterFBO1->textures[0], 0);
			//gen rbo
			//glGenRenderbuffers(1, &this->waterFBO1->rbo);
			//glBindRenderbuffer(GL_RENDERBUFFER, this->waterFBO1->rbo);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, widthDivid, heightDivid);
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->waterFBO1->rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				std::cout << "water FBO1 complite\n";
			else
				std::cout << "water FBO1 uncomplite\n";

			glClearColor(0.5f, 0, 0, 0);		// background should be black
			//glClearColor(0.5, 0, 0, 0);		// background should be black
			glClear(GL_COLOR_BUFFER_BIT);

		}
		//glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO1->fbo);
		if (lastW != w() || lastH != h())
		{
			glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO1->fbo);
			glClearColor(0.5f, 0, 0, 0);		// background should be black
			//glClearColor(0.5, 0, 0, 0);		// background should be black
			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (!this->waterUpdateShader)
		{
			this->waterUpdateShader = new Shader(
				"../shaders/update.vert",
				nullptr, nullptr, nullptr,
				"../shaders/update.frag");
		}

		if (!this->waterDropShader)
		{
			this->waterDropShader = new Shader(
				"../shaders/drop.vert",
				nullptr, nullptr, nullptr,
				"../shaders/drop.frag");
		}

		if (!this->waterPickFBO)
		{
			this->waterPickFBO = new FBO;
			//gen framebuffer
			glGenFramebuffers(1, &this->waterPickFBO->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->waterPickFBO->fbo);
			//gen texture
			glGenTextures(1, this->waterPickFBO->textures);
			glBindTexture(GL_TEXTURE_2D, this->waterPickFBO->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterPickFBO->textures[0], 0);
			//gen rbo
			glGenRenderbuffers(1, &this->waterPickFBO->rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, this->waterPickFBO->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, widthDivid, heightDivid);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->waterPickFBO->rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				std::cout << "pick FBO complite\n";
			else
				std::cout << "pick FBO uncomplite\n";
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		if (lastW != w() || lastH != h())
		{
			//update w & h
			glBindTexture(GL_TEXTURE_2D, this->waterPickFBO->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, this->waterPickFBO->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h()); 
		}
		//clear
		glBindFramebuffer(GL_FRAMEBUFFER, this->waterPickFBO->fbo);
		glClearColor(0, 0, 0, 0);		// background should be black
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (!this->skyboxShader)
			this->skyboxShader = new Shader(
				"../shaders/skybox.vert",
				nullptr, nullptr, nullptr,
				"../shaders/skybox.frag");

		if (!this->skybox)
		{
			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};


			this->skybox = new VAO;
			glGenVertexArrays(1, &this->skybox->vao);
			glGenBuffers(1, this->skybox->vbo);

			glBindVertexArray(this->skybox->vao);

			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->skybox->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), skyboxVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->skyboxTex)
		{
			std::vector<std::string> faces
			{
				"../Images/skybox/right.jpg",
				"../Images/skybox/left.jpg",
				"../Images/skybox/top.jpg",
				"../Images/skybox/bottom.jpg",
				"../Images/skybox/front.jpg",
				"../Images/skybox/back.jpg"
			};
			this->skyboxTex = new Texture3D(faces);
		}

		if (!this->tilesShader)
			this->tilesShader = new Shader(
				"../shaders/tiles.vert",
				nullptr, nullptr, nullptr,
				"../shaders/tiles.frag");

		if (!this->tiles)
		{
			float tilesVertices[] = {
				// positions          
				//-1.0f,  1.0f, -1.0f,
				//-1.0f, -1.0f, -1.0f,
				// 1.0f, -1.0f, -1.0f,
				// 1.0f, -1.0f, -1.0f,
				// 1.0f,  1.0f, -1.0f,
				//-1.0f,  1.0f, -1.0f,

				//-1.0f, -1.0f,  1.0f,
				//-1.0f, -1.0f, -1.0f,
				//-1.0f,  1.0f, -1.0f,
				//-1.0f,  1.0f, -1.0f,
				//-1.0f,  1.0f,  1.0f,
				//-1.0f, -1.0f,  1.0f,

				// 1.0f, -1.0f, -1.0f,
				// 1.0f, -1.0f,  1.0f,
				// 1.0f,  1.0f,  1.0f,
				// 1.0f,  1.0f,  1.0f,
				// 1.0f,  1.0f, -1.0f,
				// 1.0f, -1.0f, -1.0f,

				//-1.0f, -1.0f,  1.0f,
				//-1.0f,  1.0f,  1.0f,
				// 1.0f,  1.0f,  1.0f,
				// 1.0f,  1.0f,  1.0f,
				// 1.0f, -1.0f,  1.0f,
				//-1.0f, -1.0f,  1.0f,

				//-1.0f,  1.0f, -1.0f,
				// 1.0f,  1.0f, -1.0f,
				// 1.0f,  1.0f,  1.0f,
				// 1.0f,  1.0f,  1.0f,
				//-1.0f,  1.0f,  1.0f,
				//-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};


			this->tiles = new VAO;
			glGenVertexArrays(1, &this->tiles->vao);
			glGenBuffers(1, this->tiles->vbo);

			glBindVertexArray(this->tiles->vao);

			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->tiles->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(tilesVertices), tilesVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->tilesCubeTex)
		{
			std::vector<std::string> faces
			{
				"../Images/tiles.jpg",
				"../Images/tiles.jpg",
				"../Images/tiles.jpg",
				"../Images/tiles.jpg",
				"../Images/tiles.jpg",
				"../Images/tiles.jpg"
			};
			this->tilesCubeTex = new Texture3D(faces);
		}

		if (!this->tilesTex)
		{
			this->tilesTex = new Texture2D("../Images/tiles.jpg");
		}

		if (!this->reflectionFBO)
		{
			this->reflectionFBO = new FBO;
			//gen framebuffer
			glGenFramebuffers(1, &this->reflectionFBO->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
			//gen texture
			glGenTextures(1, this->reflectionFBO->textures);
			glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->reflectionFBO->textures[0], 0);
			//gen rbo
			glGenRenderbuffers(1, &this->reflectionFBO->rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, this->reflectionFBO->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->reflectionFBO->rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				std::cout << "reflectionFBO complite\n";
			else
				std::cout << "reflectionFBO uncomplite\n";
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		//use frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
		//update w & h
		if (lastW != w() || lastH != h())
		{
			glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, this->reflectionFBO->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h());
		}
		

		if (!this->refractionFBO)
		{
			this->refractionFBO = new FBO;
			//gen framebuffer
			glGenFramebuffers(1, &this->refractionFBO->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
			//gen texture
			glGenTextures(1, this->refractionFBO->textures);
			glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->refractionFBO->textures[0], 0);
			//gen rbo
			glGenRenderbuffers(1, &this->refractionFBO->rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, this->refractionFBO->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->refractionFBO->rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				std::cout << "refractionFBO complite\n";
			else
				std::cout << "refractionFBO uncomplite\n";
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		//use frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
		//update w & h
		if (lastW != w() || lastH != h())
		{
			glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w(), h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, this->refractionFBO->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w(), h());
		}

		if (!this->box)
		{
			float boxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};


			this->box = new VAO;
			glGenVertexArrays(1, &this->box->vao);
			glGenBuffers(1, this->box->vbo);

			glBindVertexArray(this->box->vao);

			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->box->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), boxVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->boxShader)
		{
			this->boxShader = new Shader(
				"../shaders/box.vert",
				nullptr, nullptr, nullptr,
				"../shaders/box.frag");
		}

		if (!this->device) {
			//Tutorial: https://ffainelli.github.io/openal-example/
			this->device = alcOpenDevice(NULL);
			if (!this->device)
				puts("ERROR::NO_AUDIO_DEVICE");

			ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
			if (enumeration == AL_FALSE)
				puts("Enumeration not supported");
			else
				puts("Enumeration supported");

			this->context = alcCreateContext(this->device, NULL);
			if (!alcMakeContextCurrent(context))
				puts("Failed to make context current");

			this->source_pos = glm::vec3(0.0f, 5.0f, 0.0f);

			ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
			alListener3f(AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alListener3f(AL_VELOCITY, 0, 0, 0);
			alListenerfv(AL_ORIENTATION, listenerOri);

			alGenSources((ALuint)1, &this->source);
			alSourcef(this->source, AL_PITCH, 1);
			alSourcef(this->source, AL_GAIN, 1.0f);
			alSource3f(this->source, AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alSource3f(this->source, AL_VELOCITY, 0, 0, 0);
			alSourcei(this->source, AL_LOOPING, AL_TRUE);

			alGenBuffers((ALuint)1, &this->buffer);

			ALsizei size, freq;
			ALenum format;
			ALvoid* data;
			ALboolean loop = AL_TRUE;

			//Material from: ThinMatrix
			alutLoadWAVFile((ALbyte*)PROJECT_DIR "/Audios/bounce.wav", &format, &data, &size, &freq, &loop);
			alBufferData(this->buffer, format, data, size, freq);
			alSourcei(this->source, AL_BUFFER, this->buffer);

			if (format == AL_FORMAT_STEREO16 || format == AL_FORMAT_STEREO8)
				puts("TYPE::STEREO");
			else if (format == AL_FORMAT_MONO16 || format == AL_FORMAT_MONO8)
				puts("TYPE::MONO");

			//alSourcePlay(this->source);

			// cleanup context
			//alDeleteSources(1, &source);
			//alDeleteBuffers(1, &buffer);
			//device = alcGetContextsDevice(context);
			//alcMakeContextCurrent(NULL);
			//alcDestroyContext(context);
			//alcCloseDevice(device);
		}
		lastW = w();
		lastH = h();
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);

	// Set up the view port
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	//glClearColor(0, 0, .3f, 0);		// background should be blue
	glClearColor(0, 0, 0, 0);		// background should be black

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

	//gl_ClipDistance[0]
	glEnable(GL_CLIP_DISTANCE0);

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
	GLfloat lightPosition1[] = { -1,1,-1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	// set linstener position 
	if (selectedCube >= 0)
		alListener3f(AL_POSITION,
			m_pTrack->points[selectedCube].pos.x,
			m_pTrack->points[selectedCube].pos.y,
			m_pTrack->points[selectedCube].pos.z);
	else
		alListener3f(AL_POSITION,
			this->source_pos.x,
			this->source_pos.y,
			this->source_pos.z);


	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	//setupFloor();
	//glDisable(GL_LIGHTING);
	//drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	//drawStuff();

	// this time drawing is for shadows (except for top view)
	//if (!tw->topCam->value()) {
	//	setupShadows();
	//	drawStuff(true);
	//	unsetupShadows();
	//}

	//*********************************************************************
	// 
	// draw wave and animate
	// 
	//*********************************************************************
	
	int waveType = tw->waveBrowser->value();

	//setup sin wave
	static unsigned long lastClock = 0;
	static float t = 0;

	if (clock() - lastClock > CLOCKS_PER_SEC / 144 && tw->runButton->value()) {
		lastClock = clock();
		if (waveType == 1)	//sin wave
		{
			t += 0.2 * tw->speed->value();
			if (t > tw->wavelength->value())t = 0;
		}
		else if (waveType == 2)	//height map
		{
			t += 0.2 * tw->speed->value();
			if (t >= 200) t = 0;
		}
		else if (waveType == 3)
		{
			t += 0.2 * tw->speed->value();
		}
	}

	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);

	//bind shader
	GLuint program;
	switch (waveType)
	{
	case 1:
		this->waterSinShader->Use();
		program = this->waterSinShader->Program;
		glUniform1f(glGetUniformLocation(program, "t"), t);
		glUniform1f(glGetUniformLocation(program, "k"), 2 * PI / tw->wavelength->value()); 
		glUniform2f(glGetUniformLocation(program, "direction"), cos(tw->waveDir->value() * PI / 180), sin(tw->waveDir->value() * PI / 180));

		break;
	case 2:
		this->waterHeightShader->Use();
		program = this->waterHeightShader->Program;
		this->heightMapTex->at((int)t)->bind(0);
		glUniform1i(glGetUniformLocation(program, "heightMap"), 0);
		break;
	case 3:
	{
		//draw pick
		glBindFramebuffer(GL_FRAMEBUFFER, this->waterPickFBO->fbo);
		this->waterSimShader->Use();
		if (waterFBOTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
			glUniform1i(glGetUniformLocation(this->waterSimShader->Program, "heightMap"), 0);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
			glUniform1i(glGetUniformLocation(this->waterSimShader->Program, "heightMap"), 0);
		}
		glUniform1f(glGetUniformLocation(this->waterSimShader->Program, "amplitude"), tw->amplitude->value());
		glm::mat4 model_matrix = glm::mat4(1);
		//model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 40.0f, 0.0f));
		glUniformMatrix4fv(
			glGetUniformLocation(this->waterSimShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);

		glBindVertexArray(this->waterSurface->vao);
		glDrawElements(GL_TRIANGLES, this->waterSurface->element_amount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		if (t >= 1)
		{
			t = 0;
			this->updateSurface();
			//addDrop(0.5, 0.5);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);
		this->waterHeightShader->Use();
		program = this->waterHeightShader->Program;
		if (waterFBOTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
			glUniform1i(glGetUniformLocation(program, "heightMap"), 0);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
			glUniform1i(glGetUniformLocation(program, "heightMap"), 0);
		}
		break;
	}
	default:
		this->waterSinShader->Use();
		program = this->waterSinShader->Program;
		break;
	}

	glUniform1f(glGetUniformLocation(program, "amplitude"), tw->amplitude->value());


	glm::mat4 model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
	//model_matrix = glm::rotate(model_matrix, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//model_matrix = glm::translate(model_matrix, this->source_pos);
	//model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(
		glGetUniformLocation(program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniform3fv(
		glGetUniformLocation(program, "u_color"),
		1,
		&glm::vec3(0.2, 0.5, 1.0)[0]);
	this->waterTex->bind(1);
	glUniform1i(glGetUniformLocation(program, "u_texture"), 1);
	this->skyboxTex->bind(2); 
	glUniform1i(glGetUniformLocation(program, "skyboxTex"), 2);
	this->tilesTex->bind(3);
	glUniform1i(glGetUniformLocation(program, "tilesTex"), 3);

	glm::vec3 eyePos = arcball.getEyePos();
	glUniform3f(glGetUniformLocation(program, "u_eyePosition"), eyePos.x, eyePos.y, eyePos.z);
	Shader::SetDirLight(program);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
	glUniform1i(glGetUniformLocation(program, "reflectTex"), 4);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
	glUniform1i(glGetUniformLocation(program, "refractTex"), 5);


	//bind VAO
	glBindVertexArray(this->waterSurface->vao);

	glDrawElements(GL_TRIANGLES, this->waterSurface->element_amount, GL_UNSIGNED_INT, 0);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	Shader::Unuse();

	//use frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
	//clear
	glClearColor(0, 0, 0, 0);		// background should be black
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//use frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
	//clear
	glClearColor(0, 0, 0, 0);		// background should be black
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//*********************************************************************
	// 
	// draw tiles
	// 
	//*********************************************************************
	//* frame buffer 
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);
	glDisable(GL_CLIP_DISTANCE0);

	glEnable(GL_CULL_FACE);
	this->tilesShader->Use();
	glUniform4fv(glGetUniformLocation(this->tilesShader->Program, "plane"), 1, &glm::vec4(0, -1, 0,10000)[0]);

	model_matrix = glm::mat4(1);
	model_matrix = glm::scale(model_matrix, glm::vec3(50.0f, 50.0f, 50.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->tilesShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);


	glBindVertexArray(this->tiles->vao);
	this->tilesCubeTex->bind(0);
	glUniform1i(glGetUniformLocation(this->tilesShader->Program, "tex"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//unbind VAO
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_CLIP_DISTANCE0);


	//* reflection FBO
	setUBO(true);
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size); 

	glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
	glUniform4fv(glGetUniformLocation(this->tilesShader->Program, "plane"), 1, &glm::vec4(0, -1, 0, -0)[0]);
	//glUniform4fv(glGetUniformLocation(this->tilesShader->Program, "plane"), 1, &glm::vec4(0, -1, 0, 100000)[0]);

	glEnable(GL_CULL_FACE);
	this->tilesShader->Use();

	model_matrix = glm::mat4(1);
	model_matrix = glm::scale(model_matrix, glm::vec3(50.0f, 50.0f, 50.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->tilesShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);


	glBindVertexArray(this->tiles->vao);
	this->tilesCubeTex->bind(0);
	glUniform1i(glGetUniformLocation(this->tilesShader->Program, "tex"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//unbind VAO
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	
	//* refraction FBO
	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);
	glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
	glUniform4fv(glGetUniformLocation(this->tilesShader->Program, "plane"), 1, &glm::vec4(0, -1, 0, 0)[0]);

	glEnable(GL_CULL_FACE);
	this->tilesShader->Use();

	model_matrix = glm::mat4(1);
	model_matrix = glm::scale(model_matrix, glm::vec3(50.0f, 50.0f, 50.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->tilesShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);


	glBindVertexArray(this->tiles->vao);
	this->tilesCubeTex->bind(0);
	glUniform1i(glGetUniformLocation(this->tilesShader->Program, "tex"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//unbind VAO
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);

	//*********************************************************************
	// 
	// draw skybox
	// 
	//*********************************************************************
	//* frame buffer 
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);

	glDepthFunc(GL_LEQUAL);

	this->skyboxShader->Use();

	glBindVertexArray(this->skybox->vao);
	this->skyboxTex->bind(0);
	glUniform1i(glGetUniformLocation(this->skyboxShader->Program, "skybox"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//unbind VAO
	glBindVertexArray(0);

	//* reflection FBO 
	setUBO(true);
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);
	glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);

	glDepthFunc(GL_LEQUAL);

	this->skyboxShader->Use();

	glBindVertexArray(this->skybox->vao);
	this->skyboxTex->bind(0);
	glUniform1i(glGetUniformLocation(this->skyboxShader->Program, "skybox"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//unbind VAO
	glBindVertexArray(0);

	//* refraction FBO 
	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);
	glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);

	glDepthFunc(GL_LEQUAL);

	this->skyboxShader->Use();

	glBindVertexArray(this->skybox->vao);
	this->skyboxTex->bind(0);
	glUniform1i(glGetUniformLocation(this->skyboxShader->Program, "skybox"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//unbind VAO
	glBindVertexArray(0);

	//*********************************************************************
	// 
	// draw box
	// 
	//*********************************************************************

	//* frame buffer 
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);
	glDisable(GL_CLIP_DISTANCE0);

	this->boxShader->Use();
	glUniform4fv(glGetUniformLocation(this->boxShader->Program, "plane"), 1, &glm::vec4(0, -1, 0, 10000)[0]);

	model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 40.0f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->boxShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);


	glBindVertexArray(this->skybox->vao);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//unbind VAO
	glBindVertexArray(0);
	glEnable(GL_CLIP_DISTANCE0);


	//* reflection FBO
	setUBO(true);
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);

	glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
	glUniform4fv(glGetUniformLocation(this->boxShader->Program, "plane"), 1, &glm::vec4(0, 1, 0, -0)[0]);
	//glUniform4fv(glGetUniformLocation(this->tilesShader->Program, "plane"), 1, &glm::vec4(0, -1, 0, 100000)[0]);

	this->boxShader->Use();

	model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 40.0f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->boxShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);


	glBindVertexArray(this->skybox->vao);
	this->tilesCubeTex->bind(0);
	glUniform1i(glGetUniformLocation(this->boxShader->Program, "tex"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//unbind VAO
	glBindVertexArray(0);

	//* refraction FBO
	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);
	glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
	glUniform4fv(glGetUniformLocation(this->boxShader->Program, "plane"), 1, &glm::vec4(0, -1, 0, 0)[0]);

	this->boxShader->Use();

	model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 40.0f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->boxShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);


	glBindVertexArray(this->skybox->vao);
	this->tilesCubeTex->bind(0);
	glUniform1i(glGetUniformLocation(this->boxShader->Program, "tex"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//unbind VAO
	glBindVertexArray(0);

	//*********************************************************************
	// 
	// draw frame buffer
	// 
	//*********************************************************************

	//bind to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (tw->pixelation->value())
	{
		this->pixelation->Use();
		glUniform1f(glGetUniformLocation(this->pixelation->Program, "rt_w"), w());
		glUniform1f(glGetUniformLocation(this->pixelation->Program, "rt_h"), h());
	}
	else
	{
		this->frameShader->Use();
	}
	glBindVertexArray(this->frame->vao);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->framebuffer->textures[0]);
	if (tw->fboTest->value())
	{
		if(waterFBOTex)
			glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
		else
			glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
	}
	if (tw->reflection->value())
	{
		glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
	}
	if (tw->refraction->value())
	{
		glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
	}


	// Draw the framebuffer rectangle
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);
	glDepthFunc(GL_LESS);

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

void TrainView::setUBO(bool inverstPitch)
{
	float wdt = this->pixel_w();
	float hgt = this->pixel_h();

	glm::mat4 view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	//HMatrix view_matrix; 
	//this->arcball.getMatrix(view_matrix);
	if (inverstPitch)
		view_matrix = viewMatrixInvertPitch(0);

	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	//projection_matrix = glm::perspective(glm::radians(this->arcball.getFoV()), (GLfloat)wdt / (GLfloat)hgt, 0.01f, 1000.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TrainView::pickSurface()
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

	my = viewport[3] - my - 1;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->waterPickFBO->fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glm::vec3 uv;
	glReadPixels(mx, my, 1, 1, GL_RGB, GL_FLOAT, &uv[0]);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	if (uv.z != 0.0)
	{
		std::cout << uv.x << ' ' << uv.y << ' ' <<uv.z<< std::endl;
		addDrop(uv.x, uv.y);
	}
}

void TrainView::updateSurface()
{
	//use fbo
	if(waterFBOTex)
		glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO0->fbo);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO1->fbo);


	this->waterUpdateShader->Use();
	glUniform1f(glGetUniformLocation(this->waterUpdateShader->Program, "velocity"), 230.0f / 180.0f);
	glUniform2f(glGetUniformLocation(this->waterUpdateShader->Program, "delta"), 8.0f / 512, 8.0f / 512);
	glUniform1f(glGetUniformLocation(this->waterUpdateShader->Program, "amplitude"), tw->amplitude->value() / 10.0f);


	if (waterFBOTex)
	{
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterFBO0->textures[0], 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
		glUniform1i(glGetUniformLocation(this->waterUpdateShader->Program, "lastTex"), 0);
	}
	else
	{
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterFBO1->textures[0], 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
		glUniform1i(glGetUniformLocation(this->waterUpdateShader->Program, "lastTex"), 0);
	}

	glBindVertexArray(this->waterSurface->vao);
	glDrawElements(GL_TRIANGLES, this->waterSurface->element_amount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	waterFBOTex = !waterFBOTex;
}


void TrainView::addDrop(float x, float y)
{
	//use fbo
	if (waterFBOTex)
		glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO0->fbo);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, this->waterFBO1->fbo);

	this->waterDropShader->Use();

	glUniform2f(glGetUniformLocation(this->waterDropShader->Program, "u_center"), x, y);
	glUniform1f(glGetUniformLocation(this->waterDropShader->Program, "u_radius"), tw->wavelength->value() / 1000);
	glUniform1f(glGetUniformLocation(this->waterDropShader->Program, "u_strength"), tw->amplitude->value() / 10);


	if (waterFBOTex)
	{
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterFBO0->textures[0], 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->waterFBO1->textures[0]);
		glUniform1i(glGetUniformLocation(this->waterDropShader->Program, "lastTex"), 0);
	}
	else
	{
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->waterFBO1->textures[0], 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->waterFBO0->textures[0]);
		glUniform1i(glGetUniformLocation(this->waterDropShader->Program, "lastTex"), 0);
	}

	glBindVertexArray(this->waterSurface->vao);
	glDrawElements(GL_TRIANGLES, this->waterSurface->element_amount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	waterFBOTex = !waterFBOTex;

	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->fbo);
}

glm::mat4 TrainView::viewMatrixInvertPitch(float waterHeight)
{
	float view[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, view);

	glm::vec3 orient = glm::vec3(view[2], view[6], view[10]);
	glm::vec3 pos = arcball.getEyePos();
	glm::vec3 rY = glm::vec3(view[1], view[5], view[9]);
	float dis = 2 * (pos.y - waterHeight);
	pos.y -= dis;

	orient.y = -orient.y;

	//glm::vec3 up = (orient * rY) / (glm::dot(orient, orient));

	glm::mat4 result = glm::lookAt(pos, orient, glm::vec3(0,-1,0));
	return result;
}
