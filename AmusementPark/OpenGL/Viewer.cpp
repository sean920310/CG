#include "Viewer.h"

Viewer::Viewer(int width, int height, const char* name):width(width),height(height)
{
	srand(time(NULL));
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


		colorShader = new Shader("./Asset/Shader/default.vert", "./Asset/Shader/default.frag");
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
			glm::vec4(0.5f, -0.9f, 0.7f, 0.0f),
			//glm::vec4(7.0f,-7.0f, 9.0f, 0.0f),
			//glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
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
		//for (int i = 0; i < 2; i++)
		//{
		//	for (int j = 0; j < 2; j++)
		//	{
		//		Terrain* terrain = new Terrain(i-1, j-1, "./Asset/Images/grass.png","./Asset/Images/heightmap.png");
		//		terrains.push_back(terrain);
		//	}
		//}
		Terrain* terrain = new Terrain(0, 0, "./Asset/Images/grass.png", "./Asset/Images/heightmap.png");
		terrains.push_back(terrain);
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
	
		float near_plane = 1.0f, far_plane = 50.0f;
		glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-dirLight->direction * 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::cross(glm::vec3(1.0f, 0.0f, 0.0f),glm::normalize(glm::vec3(-dirLight->direction))));
		
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

	//Track
	{
		track = new Track("./Asset/Model/track.txt");
	}

	//Train
	{
		train = new Train(track);
		trainCount = train->GetCarCount();
	}

	//pool
	{
		pool = new Model("./Asset/Model/pool.obj");
		modelShader = new Shader("./Asset/Shader/textureShader.vert", "./Asset/Shader/textureShader.frag");
	}

	//water
	{
		water = new WaterSurface(this);
		heightMapShader = new Shader("./Asset/Shader/waterHeightMap.vert", "./Asset/Shader/waterHeightMap.frag");
	}

	//tree
	{
		treeModel = new Model("./Asset/Model/Low poly tree 2.obj");
	}

	//rain 
	{
		rainParticles = new ParticleMaster();
	}
}

void Viewer::DrawEntity(DrawType type)
{
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->Update();
	camera->CommomMatrix();

	//set spot light to cam
	{
		spotLight->position = glm::vec4(camera->position, 1.0f);
		spotLight->direction = glm::vec4(camera->orientation, 1.0f);
	}
	
	Shader* shader = depthMapShader;
	//cube
	{
		if (type != DrawType::Shadow)
			shader = colorShader;

		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
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
	}

	//terrain
	{
		if (type != DrawType::Shadow)
			shader = terrainShader;

		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));

		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", 1.0f);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		for (const auto& terrain : terrains)
		{
			terrain->Draw(shader);
		}
	}

	//track
	{
		if (type != DrawType::Shadow)
			shader = colorShader;

		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1));
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		track->Draw(shader);
	}

	//train
	{
		if (type != DrawType::Shadow)
			shader = colorShader;

		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));


		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		train->Draw(shader);
	}

	//pool
	{
		if (type != DrawType::Shadow)
			shader = modelShader;


		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(glm::vec3(5.0f, -1.7f, -5.0f));
		model = glm::scale(model, glm::vec3(0.2));
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);

		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		pool->Draw(*modelShader);
	}

	//water
	if (type == DrawType::Default) 
	{
		heightMapShader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(glm::vec3(5.0f, waterHeight, -5.0f));
		model = glm::scale(model, glm::vec3(0.4));
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		heightMapShader->setMat4("u_model", model);

		heightMapShader->setMat4("u_view", camera->viewMatrix);
		heightMapShader->setMat4("u_projection", camera->projectionMatrix);
		heightMapShader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		heightMapShader->setFloat3("u_eyePosition", camera->position);
		heightMapShader->setDirLight(dirLight);
		heightMapShader->setSpotLight(spotLight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		heightMapShader->setInt("shadowMap", 1);

		water->Draw(heightMapShader);
	}

	//tree
	{
		if (type != DrawType::Shadow)
			shader = modelShader;


		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(glm::vec3(5.0f,-2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3));
		//model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapFBO->textures[0]);
		shader->setInt("shadowMap", 1);

		treeModel->Draw(*modelShader);


		model = glm::mat4(1.0f);
		model = glm::translate(glm::vec3(6.0f, -2.0f, -9.0f));
		model = glm::scale(model, glm::vec3(0.25));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		treeModel->Draw(*modelShader);


		model = glm::mat4(1.0f);
		model = glm::translate(glm::vec3(10.0f, -1.0f, -5.0f));
		model = glm::scale(model, glm::vec3(0.35));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("u_model", model);
		treeModel->Draw(*modelShader);
	}

	//rain
	if(rain)
	{
		dirLight->diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

		if (type != DrawType::Shadow)
			shader = colorShader;

		shader->Use();
		if (type == DrawType::Reflect)
			shader->setFloat4("u_plane", glm::vec4(0, 1, 0, -waterHeight));
		else if (type == DrawType::Refract)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, waterHeight));
		else if (type == DrawType::Default)
			shader->setFloat4("u_plane", glm::vec4(0, -1, 0, 10000));


		shader->setMat4("u_view", camera->viewMatrix);
		shader->setMat4("u_projection", camera->projectionMatrix);
		shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		shader->setFloat("shininess", cubeShininess);
		shader->setFloat3("u_eyePosition", camera->position);
		shader->setDirLight(dirLight);
		shader->setSpotLight(spotLight);

		rainParticles->Draw(shader);
	}
	else
	{
		dirLight->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	//skybox
	if (type != DrawType::Shadow)
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
}

void Viewer::DrawImGui()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui
	{
		ImGui::Begin("Control");                          // Create a window called "Hello, world!" and append into it.
		if (ImGui::TreeNode("Train"))
		{
			ImGui::Checkbox("Train Run", &trainRun);
			
			ImGui::SliderFloat("Train Speed", &trainSpeed, 0.0f, 10.0f);
			ImGui::Checkbox("Train Physic", &trainPhysic);
			ImGui::Text("Train Count: ");
			ImGui::SameLine();
			if (ImGui::ArrowButton("reduce", ImGuiDir_Left))
			{
				trainCount--;
				train->SetCarCount(trainCount);
			}
			ImGui::SameLine();
			ImGui::Text("%d",trainCount);
			ImGui::SameLine();
			if(ImGui::ArrowButton("add", ImGuiDir_Right))
			{
				trainCount++;
				train->SetCarCount(trainCount);
			}

			
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Particle"))
		{
			ImGui::Checkbox("Rain Partile", &rain);
			ImGui::SliderInt("Rain Dense", &rainDence, 1.0f, 100.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Information"))
		{
			ImGui::SliderFloat("Shininess", &cubeShininess, 1.0f, 100.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("\nCamera Position: x:%.1f y:%.1f z:%.1f", camera->position.x, camera->position.y, camera->position.z);
			ImGui::Text("Camera Orient: x:%.1f y:%.1f z:%.1f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
			ImGui::TreePop();
		}
		ImGui::End();
	}

	if(false)
	{
		ImGui::Begin("shadow map Debugger");

		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddImage(
			(void*)depthMapFBO->textures[0], ImVec2(pos),
			ImVec2(pos.x + ImGui::GetWindowWidth(), pos.y + ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
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
		UpdateObject();

		//shadow
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO->fbo);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		DrawEntity(DrawType::Shadow);

		//reflect
		glBindFramebuffer(GL_FRAMEBUFFER, water->reflectionFBO->fbo);
		glViewport(0, 0, width, height);
		float distance = 2 * (camera->position.y - waterHeight);
		camera->position.y -= distance;
		camera->invertPitch();
		DrawEntity(DrawType::Reflect);

		//refract
		glBindFramebuffer(GL_FRAMEBUFFER, water->refractionFBO->fbo);
		glViewport(0, 0, width, height); 
		camera->position.y += distance;
		camera->invertPitch(); 
		DrawEntity(DrawType::Refract);

		//noraml
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		DrawEntity(DrawType::Default);
		DrawImGui();


		//交換front 和 back Buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Viewer::UpdateObject()
{
	static float rotation = 0.0f, prevTime = glfwGetTime();
	double curTime = glfwGetTime();
	if (curTime - prevTime >= 1.0 / 60)
	{
		static float vel = trainSpeed;
		if (trainRun)
		{
			if (trainPhysic)
			{
				vel = 0.999 *vel + 0.01 * train->AddPhysics();
				if (vel < trainSpeed * 0.1) vel += trainSpeed * 0.1;
			}
			else
			{
				vel = trainSpeed;
			}
			//v = v0 + at
			//std::cout << vel << std::endl;
			train->AddTrainU(vel);
		}

		if (water)
			water->Update();
		if (rain)
		{
			for (int i = 0; i < rainDence; i++)
			{
				float x = rand() % 50000 / 1000.0f - 25.f + camera->position.x;
				float z = rand() % 50000 / 1000.0f - 25.f + camera->position.z;
	
				Particle* particle = new Particle(
					glm::vec3(x, 50.0f, z),
					glm::vec3(0.0f, -1.0f, 0.0f),
					1.5f,
					glm::vec3(0.01f, 0.5f, 0.01f)
				);
				rainParticles->AddParticle(particle);
			}
		}
		rainParticles->Update(1.0f / 60);

		prevTime = curTime;
	}
}

void Viewer::frambufferCallbackHandler(int width, int height)
{
	glViewport(0, 0, width, height);
	this->width = width;
	this->height = height;
	Camera::Resize(width, height);
	if (this->water)
		water->ResizeFBO();
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