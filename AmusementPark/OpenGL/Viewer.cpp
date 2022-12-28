#include "Viewer.h"

Viewer::Viewer(int width, int height, const char* name):width(width),height(height)
{
	//初始化GLFW
	glfwInit();
	//告訴GLFW OpenGL的Version (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//告訴GLFW 使用 Modern OpenGL 的 function
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	glfwSetWindowUserPointer(window, this);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, frambufferCallback);
	glfwSetKeyCallback(window, keyInputCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	camera = new Camera(this, glm::vec3(0.0, 0.0, 3.0));

	gladLoadGL();
}

Viewer::~Viewer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	//結束GLFW
	glfwTerminate();
}


void Viewer::Init()
{
	{
		GLfloat vertices[] =
		{
			// coord			/	color			/	texture
			-0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 1.0f,	0.0f, 0.0f,
			-0.5f, 0.0f, -0.5f,		1.0f, 1.0f, 1.0f,	5.0f, 0.0f,
			0.5f, 0.0f, -0.5f,		1.0f, 1.0f, 1.0f,	0.0f, 0.0f,
			0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 1.0f,	5.0f, 0.0f,
			0.0f, 0.8f, 0.0f,		1.0f, 1.0f, 1.0f,	2.5f, 5.0f
		};

		GLuint indices[] =
		{
			0,1,2,
			0,2,3,
			0,1,4,
			1,2,4,
			2,3,4,
			3,0,4
		};


		shader = new Shader("./Asset/Shader/default.vert", "./Asset/Shader/default.frag");
		vao = new VAO;
		vao->Bind();

		VBO vbo(vertices, sizeof(vertices));
		EBO ebo(indices, sizeof(indices));

		vao->LinkAttrib(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		vao->LinkAttrib(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		vao->LinkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		vao->Unbind();
		vbo.Unbind();
		vbo.Delete();
		ebo.Unbind();

		texture = new Texture2D("wall.jpg");
	}

	{
		GLfloat vertices[] =
		{
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		cube = new VAO;
		cube->Bind();

		VBO vbo(vertices, sizeof(vertices));

		cube->LinkAttrib(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		cube->LinkAttrib(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		cube->LinkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		cube->Unbind();
		vbo.Unbind();
		vbo.Delete();
	}

	{
		GLfloat vertices[] = {
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

		skybox = new VAO;
		skybox->Bind();

		VBO vbo(vertices, sizeof(vertices));

		skybox->LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

		vao->Unbind();
		vbo.Unbind();
		vbo.Delete();

		std::vector<std::string> paths{
			"./Asset/Images/skybox/right.jpg",
			"./Asset/Images/skybox/left.jpg",
			"./Asset/Images/skybox/top.jpg",
			"./Asset/Images/skybox/bottom.jpg",
			"./Asset/Images/skybox/front.jpg",
			"./Asset/Images/skybox/back.jpg"
		};
		skyboxTex = new Texture3D(paths);

		skyboxShader = new Shader("./Asset/Shader/skybox.vert", "./Asset/Shader/skybox.frag");
	}

	{
		dirLight = new DirLight(
			glm::vec4(0.7f,-0.7f, 0.9f, 0.0f),
			glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		);
	}

}

void Viewer::Draw()
{
	glEnable(GL_DEPTH_TEST);


	//清除顏色並用指定顏色取代
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	//執行上面清除顏色的function
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui
	{
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::SliderFloat("shininess", &cubeShininess, 1.0f, 100.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("\nCamera Position: x:%.1f y:%.1f z:%.1f", camera->position.x, camera->position.y, camera->position.z);
		ImGui::End();
	}

	static float rotation = 0.0f, prevTime = glfwGetTime();
	double curTime = glfwGetTime();
	if (curTime - prevTime >= 1.0 / 60)
	{
		rotation += 0.5f;
		prevTime = curTime;
		if (rotation >= 360.0f)rotation -= 360.0f;
	}

	camera->Update();
	camera->CommomMatrix();

	//vao
	{
		shader->Use(); 

		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);

		texture->Bind(0);
		shader->setInt("tex0", 0);
		vao->Bind();
		//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}

	//cube
	{
		shader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setFloat3("u_color", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->setDirLight(dirLight, camera->viewMatrix);

		cube->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


	//skybox
	{
		glDepthFunc(GL_LEQUAL);
		skyboxShader->Use();
		skyboxShader->setMat4("u_view", camera->viewMatrix);
		skyboxShader->setMat4("u_projection", camera->projectionMatrix);

		skyboxTex->Bind(0);
		skyboxShader->setInt("skybox", 0);

		skybox->Bind();
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
	}




	// ImGui Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Viewer::Update()
{
	while (!glfwWindowShouldClose(window))
	{
		Draw();


		//交換front 和 back Buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Viewer::frambufferCallbackHandler(int width, int height)
{
	glViewport(0, 0, width, height);
	Camera::Resize(width, height);
}

void Viewer::keyInputCallbackHandler(int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		camera->ToggleMouse();
	}
}

void Viewer::mouseCallbackHandler(double xpos, double ypos)
{
	this->camera->MouseInputs(xpos, ypos);
}

//===================================================================================================

void frambufferCallback(GLFWwindow* window, int width, int height)
{
	//如果視窗最小化要停止Render
	while (width == 0 && height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	Viewer* ptr = (Viewer*)glfwGetWindowUserPointer(window);
	ptr->frambufferCallbackHandler(width, height);
}

void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	Viewer* ptr = (Viewer*)glfwGetWindowUserPointer(window);
	ptr->keyInputCallbackHandler(key, scancode, action, mode);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Viewer* ptr = (Viewer*)glfwGetWindowUserPointer(window);
	ptr->mouseCallbackHandler(xpos, ypos);
}