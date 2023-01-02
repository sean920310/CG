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

	camera = new Camera(this, glm::vec3(0.0, 1.0, 3.0));

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
	//triangle
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

	//cube
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

	//skybox
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

		skybox->Unbind();
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

	//Lights
	{
		dirLight = new DirLight(
			glm::vec4(0.7f, -0.7f, 0.9f, 0.0f),
			//glm::vec4(7.0f,-7.0f, 9.0f, 0.0f),
			glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		);
	
		spotLight = new SpotLight(
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
			glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			1.0f, 0.01f, 0.0f,
			1, 10
		);
	}

	//terrain
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Terrain* terrain = new Terrain(i-1, j-1, "./Asset/Images/grass.png");
				terrains.push_back(terrain);
			}
		}
		
		terrainShader = new Shader("./Asset/Shader/terrain.vert", "./Asset/Shader/terrain.frag");
	}

	//depth map fbo
	{
		depthMapFBO = new FBO;

		//gen framebuffer
		glGenFramebuffers(1, &depthMapFBO->fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO->fbo);
		//gen texture
		glGenTextures(1, depthMapFBO->textures);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapFBO->textures[0], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		//bind default fbo
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		depthMapShader = new Shader("./Asset/Shader/depthMap.vert", "./Asset/Shader/depthMap.frag");
	
		float near_plane = 1.0f, far_plane = 100.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-dirLight->direction * 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		
		lightSpaceMatrix = lightProjection * lightView;
	}

	{
		GLfloat vertices[] = {
			// positions	/ texCoord
			0.0f, 0.0f,		1.0f, 0.0f,
			-1.0f, 0.0f,	0.0f, 0.0f,
			-1.0f, 1.0f,	0.0f, 1.0f,

			0.0f, 1.0f,		1.0f, 1.0f,
			0.0f, 0.0f, 	1.0f, 0.0f,
			-1.0f, 1.0f,	0.0f, 1.0f
		};

		debugVAO = new VAO;
		debugVAO->Bind();

		VBO vbo(vertices, sizeof(vertices));
		
		debugVAO->LinkAttrib(vbo, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
		debugVAO->LinkAttrib(vbo, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		debugVAO->Unbind();
		vbo.Unbind();
		vbo.Delete();

		debugShader = new Shader("./Asset/Shader/debug.vert", "./Asset/Shader/debug.frag");
	}
}

void Viewer::DrawEntity()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	//set spot light to cam
	{
		spotLight->position = glm::vec4(camera->position, 1.0f);
		spotLight->direction = glm::vec4(camera->orientation, 1.0f);
	}

	//vao
	{
		shader->Use(); 

		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		texture->Bind(0);
		shader->setInt("tex0", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		vao->Bind();
		//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}

	//cube
	{
		shader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setFloat3("u_color", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		cube->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 1.0f));
		shader->setMat4("u_model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}

	//terrain
	{
		terrainShader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		terrainShader->setMat4("u_model", model);
		terrainShader->setMat4("u_view", camera->viewMatrix);
		terrainShader->setMat4("u_projection", camera->projectionMatrix);
		terrainShader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		terrainShader->setFloat("shininess", 1.0f);
		terrainShader->setFloat3("u_eyePosition", camera->position);
		terrainShader->setDirLight(dirLight);
		terrainShader->setSpotLight(spotLight);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		terrainShader->setInt("shadowMap", 1);

		for (const auto& terrain : terrains)
		{
			terrain->Draw(terrainShader);
		}
	}


	//skybox
	{
		glDepthFunc(GL_LEQUAL);
		skyboxShader->Use();
		skyboxShader->setMat4("u_view", camera->viewMatrix);
		skyboxShader->setMat4("u_projection", camera->projectionMatrix);

		skyboxTex->Bind(0);
		skyboxShader->setInt("skybox", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		skyboxShader->setInt("shadowMap", 1);
		
		skybox->Bind();
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
	}

	//debug
	{
		debugShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		debugShader->setInt("tex", 0);


		debugVAO->Bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

}

void Viewer::DrawShadowMap()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);


	//cube
	{
		depthMapShader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		depthMapShader->setMat4("u_model", model);
		depthMapShader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		cube->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);


		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 1.0f));
		depthMapShader->setMat4("u_model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//terrain
	{
		depthMapShader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		depthMapShader->setMat4("u_model", model);
		depthMapShader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);


		for (const auto& terrain : terrains)
		{
			terrain->Draw(depthMapShader);
		}
	}
}

void Viewer::DrawImGui()
{

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
		ImGui::Text("Camera Orient: x:%.1f y:%.1f z:%.1f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
		ImGui::End();
	}

	// ImGui Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Viewer::Update()
{
	while (!glfwWindowShouldClose(window))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO->fbo);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); 
		DrawShadowMap();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		DrawEntity();

		DrawImGui();


		//交換front 和 back Buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Viewer::frambufferCallbackHandler(int width, int height)
{
	glViewport(0, 0, width, height);
	this->width = width;
	this->height = height;
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