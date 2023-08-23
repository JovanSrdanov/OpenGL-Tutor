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
	case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
	case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
	case GLFW_KEY_W: UserInput->MoveUp = IsDown; break;
	case GLFW_KEY_S: UserInput->MoveDown = IsDown; break;

	case GLFW_KEY_RIGHT: UserInput->LookLeft = IsDown; break;
	case GLFW_KEY_LEFT: UserInput->LookRight = IsDown; break;
	case GLFW_KEY_UP: UserInput->LookUp = IsDown; break;
	case GLFW_KEY_DOWN: UserInput->LookDown = IsDown; break;

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
}

static void HandleInput(EngineState* state)
{
	Input* UserInput = state->mInput;
	Camera* FPSCamera = state->mCamera;
	if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
	if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
	if (UserInput->MoveDown) FPSCamera->Move(0.0f, -1.0f, state->mDT);
	if (UserInput->MoveUp) FPSCamera->Move(0.0f, 1.0f, state->mDT);

	if (UserInput->LookLeft) FPSCamera->Rotate(1.0f, 0.0f, state->mDT);
	if (UserInput->LookRight) FPSCamera->Rotate(-1.0f, 0.0f, state->mDT);
	if (UserInput->LookDown) FPSCamera->Rotate(0.0f, -1.0f, state->mDT);
	if (UserInput->LookUp) FPSCamera->Rotate(0.0f, 1.0f, state->mDT);

	if (UserInput->GoUp) FPSCamera->UpDown(1, state->mDT);
	if (UserInput->GoDown) FPSCamera->UpDown(-1, state->mDT);
}



void printData(Camera cam) {
	std::cout << "###\n";
	std::cout << "Position";
	std::cout << cam.GetPosition().x<< " " << cam.GetPosition().y << " " << cam.GetPosition().z << "\n" ;

	std::cout << "Pitch";
	std::cout << cam.mPitch << "\n" ;
	std::cout << "Yawn";
	std::cout << cam.mYaw<< "\n";

	std::cout << "###\n";
};

int main()
{
	GLFWwindow* Window = 0;
	if (!glfwInit())
	{
		std::cerr << "Failed to init glfw" << std::endl;
		return -1;
	}

	const std::string WindowTitle = "Karibi Kontrolna Tacka 02";
	int WindowWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	int WindowHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), glfwGetPrimaryMonitor(), 0);
	if (!Window)
	{
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window);

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
	glfwSetWindowUserPointer(Window, &State);

	glfwSetErrorCallback(ErrorCallback);
	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
	glfwSetKeyCallback(Window, KeyCallback);

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

	// Start values of variables
	Shader* CurrentShader = &PhongShaderMaterialTexture;
	bool flash_light = false;
	double start_time;
	glm::mat4 model_matrix(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	while (!glfwWindowShouldClose(Window)) {
		start_time = glfwGetTime();
		glfwPollEvents();
		HandleInput(&State);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(CurrentShader->GetId());
		CurrentShader->SetProjection(glm::perspective(70.0f, static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight), 0.1f, 100.0f));
		CurrentShader->SetView(glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp()));
		CurrentShader->SetUniform3f("uViewPos", FPSCamera.GetPosition());

		if (glfwGetKey(Window, GLFW_KEY_F) == GLFW_PRESS)
		{
			flash_light = true;
		}
		if (glfwGetKey(Window, GLFW_KEY_G) == GLFW_PRESS)
		{
			flash_light = false;
		}

		if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			printData(FPSCamera);
		}

		if (flash_light)
		{
			glm::vec3 pos = FPSCamera.GetTarget() - FPSCamera.GetPosition();
			CurrentShader->SetUniform3f("uFlashLight.Position", glm::vec3(FPSCamera.GetPosition()));
			CurrentShader->SetUniform3f("uFlashLight.Direction", glm::vec3(pos.x, pos.y, pos.z));
			CurrentShader->SetUniform3f("uFlashLight.Kd", glm::vec3(1));
			CurrentShader->SetUniform3f("uFlashLight.Ks", glm::vec3(1));
		}
		if (!flash_light)
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
		
		// Cube 
		model_matrix = glm::mat4(1.0f);
		model_matrix = glm::scale(model_matrix, glm::vec3(2));
		CurrentShader->SetModel(model_matrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RockDiffuseTexture);
		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(Window);
		State.mDT = glfwGetTime() - start_time;
	}

	glfwTerminate();
	return 0;
}
