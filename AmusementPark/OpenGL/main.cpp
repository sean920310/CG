#include "GameHeader.h"
#include "gameManager.h"


int main()
{
	GameManager app;
	app.Run();

	return 0;
}

//int main()
//{
//	//初始化GLFW
//	glfwInit();
//	//告訴GLFW OpenGL的Version (3.3)
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	//告訴GLFW 使用 Modern OpenGL 的 function
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//
//	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//
//	gladLoadGL();
//	glViewport(0, 0, width, height);
//
//	GLfloat vertices[] =
//	{
//		// coord			/	color			/	texture
//		-0.5f, 0.0f, 0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
//		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	5.0f, 0.0f,
//		0.5f, 0.0f, -0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
//		0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 0.0f,	5.0f, 0.0f,
//		0.0f, 0.8f, 0.0f,		1.0f, 1.0f, 1.0f,	2.5f, 5.0f
//	};
//
//	GLuint indices[] =
//	{
//		0,1,2,
//		0,2,3,
//		0,1,4,
//		1,2,4,
//		2,3,4,
//		3,0,4
//	};
//
//
//	Shader shader("default.vert", "default.frag");
//	VAO VAO;
//	VAO.Bind();
//
//	VBO VBO(vertices, sizeof(vertices));
//	EBO EBO(indices, sizeof(indices));
//
//	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
//	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//
//	VAO.Unbind();
//	VBO.Unbind();
//	VBO.Delete();
//	EBO.Unbind();
//
//	Texture2D texture("wall.jpg");
//
//	Camera camera(width, height, glm::vec3(0.0, 0.0, 3.0));
//
//	float rotation = 0.0f;
//	double prevTime = glfwGetTime();
//
//	glEnable(GL_DEPTH_TEST);
//
//	ImGui::CreateContext();
//	ImGui::StyleColorsDark();
//	ImGui_ImplGlfw_InitForOpenGL(window, true);
//	ImGui_ImplOpenGL3_Init("#version 150");
//
//	bool show_demo_window = true;
//	bool show_another_window = false;
//	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
//	while (!glfwWindowShouldClose(window))
//	{
//		//清除顏色並用指定顏色取代
//		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
//		//執行上面清除顏色的function
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//
//		{
//			static float f = 0.0f;
//			static int counter = 0;
//
//			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
//
//			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//			ImGui::Checkbox("Another Window", &show_another_window);
//
//			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//				counter++;
//			ImGui::SameLine();
//			ImGui::Text("counter = %d", counter);
//
//			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//			ImGui::End();
//		}
//
//		//使用已經編譯好的shader
//		shader.Use();
//
//		double curTime = glfwGetTime();
//		if (curTime - prevTime >= 1.0 / 60)
//		{
//			rotation += 0.5f;
//			prevTime = curTime;
//		}
//
//		camera.Inputs(window);
//		camera.commomMatrix();
//
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
//
//		shader.setMat4("u_model", model);
//		shader.setMat4("u_view", camera.viewMatrix);
//		shader.setMat4("u_projection", camera.projectionMatrix);
//
//
//		texture.Bind(0);
//		shader.setInt("tex0", 0);
//		//由於資料都存在VAO內 所以每次需要畫出物件時 就只要把對應的VAO綁定
//		VAO.Bind();
//		//glDrawArrays(GL_TRIANGLES, 0, 3);
//		glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(vertices[0]), GL_UNSIGNED_INT, 0);
//
//
//		// Rendering
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//
//		//交換front 和 back Buffer
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	VAO.Delete();
//	VBO.Delete();
//	EBO.Delete();
//	shader.Delete();
//	texture.Delete();
//
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwDestroyWindow(window);
//	//結束GLFW
//	glfwTerminate();
//	return 0;
//}