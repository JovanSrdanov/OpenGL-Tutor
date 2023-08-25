#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"


struct Input
{
	bool MoveLeft;
	bool MoveRight;
	bool MoveUp;
	bool MoveDown;
	bool LookLeft;
	bool LookRight;
	bool LookUp;
	bool LookDown;
	bool GoUp;
	bool GoDown;
};


struct EngineState
{
	Input* mInput;
	Camera* mCamera;
	unsigned mShadingMode;
	bool mDrawDebugLines;
	double mDT;
	int mode = 1;
	double lastMouseX = 0;
	double lastMouseY = 0;
	bool firstMouse = true;
};

static void ErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << std::endl;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
	Input* UserInput = State->mInput;
	bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
	switch (key)
	{
	case GLFW_KEY_1:
		if (IsDown)
		{
			State->mode = 1;
		}
		break;

	case GLFW_KEY_2:
		if (IsDown)
		{
			State->mode = 2;
		}
		break;

	case GLFW_KEY_3:
		if (IsDown)
		{
			State->mode = 3;
		}
		break;

	case GLFW_KEY_4:
		if (IsDown)
		{
			State->mode = 4;
		}
		break;


	case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
	case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
	case GLFW_KEY_W: UserInput->MoveUp = IsDown; break;
	case GLFW_KEY_S: UserInput->MoveDown = IsDown; break;

	case GLFW_KEY_SPACE: UserInput->GoUp = IsDown; break;
	case GLFW_KEY_C: UserInput->GoDown = IsDown; break;

	case GLFW_KEY_L:
	{
		if (IsDown)
		{
			State->mDrawDebugLines ^= true; break;
		}
	} break;

	case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
	}
}
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
	State->firstMouse = true;
}

static void HandleInput(EngineState* state)
{
	Input* UserInput = state->mInput;
	Camera* FPSCamera = state->mCamera;
	if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
	if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
	if (UserInput->MoveDown) FPSCamera->Move(0.0f, -1.0f, state->mDT);
	if (UserInput->MoveUp) FPSCamera->Move(0.0f, 1.0f, state->mDT);


	if (UserInput->GoUp) FPSCamera->UpDown(1, state->mDT);
	if (UserInput->GoDown) FPSCamera->UpDown(-1, state->mDT);
}
void printData(Camera cam) {
	std::cout << "###\n";
	std::cout << "Position";
	std::cout << cam.GetPosition().x << " " << cam.GetPosition().y << " " << cam.GetPosition().z << "\n";

	std::cout << "Pitch";
	std::cout << cam.mPitch << "\n";
	std::cout << "Yawn";
	std::cout << cam.mYaw << "\n";

	std::cout << "###\n";
};


// Add this function to handle mouse movement
static void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);

	if (State->firstMouse)
	{
		State->lastMouseX = xpos;
		State->lastMouseY = ypos;
		State->firstMouse = false;
	}

	double xoffset = xpos - State->lastMouseX;
	double yoffset = State->lastMouseY - ypos; // reversed since y-coordinates range from bottom to top

	State->lastMouseX = xpos;
	State->lastMouseY = ypos;

	Camera* FPSCamera = State->mCamera;
	float speed = 0.3;
	FPSCamera->Rotate(static_cast<float>(xoffset * speed), static_cast<float>(yoffset * speed), State->mDT);
}

void HandleKeyInput(GLFWwindow* window, EngineState* state)
{
	Input* UserInput = state->mInput;


	UserInput->MoveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	UserInput->MoveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	UserInput->MoveUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	UserInput->MoveDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	UserInput->GoUp = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	UserInput->GoDown = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;

	// Add other key checks here

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		state->mode = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		state->mode = 2;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		state->mode = 3;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		state->mode = 4;
	}

	// Add other mode keys here
}


int main()
{
	GLFWwindow* window = 0;
	if (!glfwInit())
	{
		std::cerr << "Failed to init glfw" << std::endl;
		return -1;
	}

	const std::string WindowTitle = "OpenGLDemo";
	int WindowWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	int WindowHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
	if (!window)
	{
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide the cursor

	glEnable(GL_MULTISAMPLE);

	GLenum GlewError = glewInit();
	if (GlewError != GLEW_OK)
	{
		std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
		glfwTerminate();
		return -1;
	}

	EngineState State = { 0 };
	Camera FPSCamera;
	Input UserInput = { 0 };
	State.mCamera = &FPSCamera;
	State.mInput = &UserInput;
	glfwSetWindowUserPointer(window, &State);

	glfwSetErrorCallback(ErrorCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
	// Inside the main function, add this line to set the mouse callback
	glfwSetCursorPosCallback(window, MouseCallback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	std::vector<float> CubeVertices =
	{
		// X     Y     Z     NX    NY    NZ    U     V    
		// FRONT SIDE
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
		 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // R U
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
		// LEFT SIDE
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		// RIGHT SIDE
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		// BOTTOM SIDE
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // R U
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
		// TOP SIDE
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // R U
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
		// BACK SIDE
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // R U
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
	};


	//Model woman("res/Woman/091_W_Aya_100K.obj");
	//if (!woman.Load())
	//{
	//	std::cerr << "Failed to load model\n";
	//	glfwTerminate();
	//	return -1;
	//}

	unsigned CubeVAO;
	glGenVertexArrays(1, &CubeVAO);
	glBindVertexArray(CubeVAO);
	unsigned CubeVBO;
	glGenBuffers(1, &CubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, CubeVertices.size() * sizeof(float), CubeVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader PhongShaderMaterialTexture("shaders/basic.vert", "shaders/phong_material_texture.frag");
	glUseProgram(PhongShaderMaterialTexture.GetId());

	// Default for point light (Sun)
	PhongShaderMaterialTexture.SetUniform3f("uSunLight.Ka", glm::vec3(1.00, 0.97, 0.00));
	PhongShaderMaterialTexture.SetUniform3f("uSunLight.Kd", glm::vec3(1.00, 0.97, 0.00));
	PhongShaderMaterialTexture.SetUniform3f("uSunLight.Ks", glm::vec3(0));

	// Light from the FlashLight 
	PhongShaderMaterialTexture.SetUniform3f("uFlashLight.Ka", glm::vec3(0));
	PhongShaderMaterialTexture.SetUniform3f("uFlashLight.Kd", glm::vec3(1));
	PhongShaderMaterialTexture.SetUniform3f("uFlashLight.Ks", glm::vec3(1));
	PhongShaderMaterialTexture.SetUniform1f("uFlashLight.Kc", 0.7f);
	PhongShaderMaterialTexture.SetUniform1f("uFlashLight.Kl", 0.0002f);
	PhongShaderMaterialTexture.SetUniform1f("uFlashLight.Kq", 0.0002f);
	PhongShaderMaterialTexture.SetUniform1f("uFlashLight.InnerCutOff", glm::cos(glm::radians(1.0f)));
	PhongShaderMaterialTexture.SetUniform1f("uFlashLight.OuterCutOff", glm::cos(glm::radians(30.0f)));

	// Materials
	PhongShaderMaterialTexture.SetUniform1i("uMaterial.Kd", 0);
	PhongShaderMaterialTexture.SetUniform1i("uMaterial.Ks", 1);
	PhongShaderMaterialTexture.SetUniform1f("uMaterial.Shininess", 64);

	// Diffuse texture
	unsigned RockDiffuseTexture = Texture::LoadImageToTexture("res/rock.jpg");
	unsigned BlackDiffuseTexture = Texture::LoadImageToTexture("res/black.jpg");
	unsigned WhiteDiffuseTexture = Texture::LoadImageToTexture("res/white.jpg");

	// Start values of variables
	Shader* CurrentShader = &PhongShaderMaterialTexture;
	bool flash_light = false;
	double start_time;
	glm::mat4 model_matrix(1.0f);
	float grey = 0.9;
	glClearColor(grey, grey, grey, 1.0f);

	//////////////////////////////////////////////////////////////////////////////////////////
	// 
	// Calculate the normal lines' vertices
	std::vector<float> NormalLineVertices;
	for (size_t i = 0; i < CubeVertices.size(); i += 8) {
		float x = CubeVertices[i];
		float y = CubeVertices[i + 1];
		float z = CubeVertices[i + 2];
		float nx = CubeVertices[i + 3];
		float ny = CubeVertices[i + 4];
		float nz = CubeVertices[i + 5];

		// Calculate the endpoints of the normal line
		glm::vec3 startPoint(x, y, z);
		glm::vec3 endPoint = startPoint + glm::vec3(nx, ny, nz) * 0.5f;

		// Add endpoints to the NormalLineVertices vector
		NormalLineVertices.push_back(startPoint.x);
		NormalLineVertices.push_back(startPoint.y);
		NormalLineVertices.push_back(startPoint.z);
		NormalLineVertices.push_back(endPoint.x);
		NormalLineVertices.push_back(endPoint.y);
		NormalLineVertices.push_back(endPoint.z);
	}
	unsigned NormalLinesVAO;
	glGenVertexArrays(1, &NormalLinesVAO);
	glBindVertexArray(NormalLinesVAO);

	unsigned NormalLinesVBO;
	glGenBuffers(1, &NormalLinesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalLinesVBO);
	glBufferData(GL_ARRAY_BUFFER, NormalLineVertices.size() * sizeof(float), NormalLineVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	// Unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////
	// For text
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();
	bool showHelloWindow = false;



	while (!glfwWindowShouldClose(window)) {

		start_time = glfwGetTime();
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		// Call the function to handle key input
		HandleKeyInput(window,&State);


		HandleInput(&State);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(CurrentShader->GetId());
		CurrentShader->SetProjection(glm::perspective(70.0f, static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight), 0.1f, 100.0f));
		CurrentShader->SetView(glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp()));
		CurrentShader->SetUniform3f("uViewPos", FPSCamera.GetPosition());




		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			flash_light = true;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			flash_light = false;
		}
		//if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		//{
		//	printData(FPSCamera);
		//}
		if (flash_light)
		{
			glm::vec3 pos = FPSCamera.GetTarget() - FPSCamera.GetPosition();
			CurrentShader->SetUniform3f("uFlashLight.Position", glm::vec3(FPSCamera.GetPosition()));
			CurrentShader->SetUniform3f("uFlashLight.Direction", glm::vec3(pos.x, pos.y, pos.z));
			CurrentShader->SetUniform3f("uFlashLight.Kd", glm::vec3(1));
			CurrentShader->SetUniform3f("uFlashLight.Ks", glm::vec3(1));
		}
		else
		{
			CurrentShader->SetUniform3f("uFlashLight.Kd", glm::vec3(0));
			CurrentShader->SetUniform3f("uFlashLight.Ks", glm::vec3(0));
		}
		CurrentShader->SetUniform3f("uDirLight.Direction", glm::vec3(0, -0.1, 0));
		CurrentShader->SetUniform3f("uDirLight.Ka", glm::vec3(0.68, 0.70, 0.51));
		CurrentShader->SetUniform3f("uDirLight.Kd", glm::vec3(0.68, 0.70, 0.51));
		CurrentShader->SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));

		glm::vec3 point_light_position_sun(0, 25, 0);
		CurrentShader->SetUniform1f("uSunLight.Kc", 0.1 / abs(sin(start_time)));
		CurrentShader->SetUniform1f("uSunLight.Kq", 0.1 / abs(sin(start_time)));
		CurrentShader->SetUniform3f("uSunLight.Position", point_light_position_sun);

		glm::mat4 model_matrix(1.0f);
		CurrentShader->SetModel(model_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Main cube 
		model_matrix = glm::mat4(1.0f);
		model_matrix = glm::scale(model_matrix, glm::vec3(1));
		CurrentShader->SetModel(model_matrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RockDiffuseTexture);
		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

		// Bind the VAO and draw the vertices as points
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
		glBindVertexArray(0);




		//////////////////////////////////////////////////////////////////////////////////////////
		model_matrix = glm::mat4(1.0f);
		CurrentShader->SetModel(model_matrix);
		glBindVertexArray(NormalLinesVAO);
		glDrawArrays(GL_LINES, 0, NormalLineVertices.size() / 3);
		glBindVertexArray(0);


		glBindVertexArray(0);
		glUseProgram(0);

		ImGui_ImplGlfwGL3_NewFrame();

		// Create an ImGui window for controlling the actions
		ImGui::Begin("Controls");

		if (ImGui::Button("Toggle Hello (U)"))
		{
			// Toggle the showHelloWindow flag
			showHelloWindow = !showHelloWindow;
		}

		ImGui::End();

		// Handle the "U" key press to show/hide the "hello" window
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		{
			showHelloWindow = true;
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		{
			showHelloWindow = false;
		}

		// Render the "hello" window if the flag is set
		if (showHelloWindow)
		{
			ImGui::SetNextWindowPos(ImVec2(WindowWidth / 2.0f, WindowHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(100, 50));
			ImGui::Begin("Hello", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			ImGui::Text("Hello");
			ImGui::End();
		}

		// Render ImGui
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());



		glfwSwapBuffers(window);
		State.mDT = glfwGetTime() - start_time;
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
