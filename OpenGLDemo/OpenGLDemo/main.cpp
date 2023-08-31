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


struct input
{
	bool move_left;
	bool move_right;
	bool move_up;
	bool move_down;
	bool look_left;
	bool look_right;
	bool look_up;
	bool look_down;
	bool go_up;
	bool go_down;
};

enum shading_mode
{
	FLAT,
	GOURAUD,
	PHONG
};


struct engine_state
{
	input* m_input;
	Camera* m_camera;
	double m_dt;
	int mode = 5;
	double last_mouse_x = 0;
	double last_mouse_y = 0;
	bool first_mouse = true;
	bool enable_mouse_callback = true;
	shading_mode shading_mode = FLAT;
};

void error_callback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << std::endl;
}

void frame_buffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
	auto* state = static_cast<engine_state*>(glfwGetWindowUserPointer(window));
	state->first_mouse = true;
}

void handle_input(const engine_state* state)
{
	const input* user_input = state->m_input;
	Camera* fps_camera = state->m_camera;
	if (user_input->move_left) fps_camera->Move(-1.0f, 0.0f, state->m_dt);
	if (user_input->move_right) fps_camera->Move(1.0f, 0.0f, state->m_dt);
	if (user_input->move_down) fps_camera->Move(0.0f, -1.0f, state->m_dt);
	if (user_input->move_up) fps_camera->Move(0.0f, 1.0f, state->m_dt);


	if (user_input->go_up) fps_camera->UpDown(1, state->m_dt);
	if (user_input->go_down) fps_camera->UpDown(-1, state->m_dt);
}
void print_data(Camera cam) {
	std::cout << "###\n";
	std::cout << "Position";
	std::cout << cam.GetPosition().x << " " << cam.GetPosition().y << " " << cam.GetPosition().z << "\n";
	std::cout << "Pitch";
	std::cout << cam.mPitch << "\n";
	std::cout << "Yawn";
	std::cout << cam.mYaw << "\n";
	std::cout << "###\n";
};

bool containsElement(const glm::vec3& target, const std::vector<glm::vec3>& added) {
	for (const glm::vec3& element : added) {
		if (element == target) {
			return true; // Element found in the vector
		}
	}
	return false; // Element not found in the vector
}


// Add this function to handle mouse movement
void mouse_callback(GLFWwindow* window, const double x_pos, const double y_pos)
{
	auto* state = static_cast<engine_state*>(glfwGetWindowUserPointer(window));

	if (!state->enable_mouse_callback) {
		state->last_mouse_x = x_pos;
		state->last_mouse_y = y_pos;
		return;
	}

	if (state->first_mouse)
	{
		state->last_mouse_x = x_pos;
		state->last_mouse_y = y_pos;
		state->first_mouse = false;
	}

	const double x_offset = x_pos - state->last_mouse_x;
	const double y_offset = state->last_mouse_y - y_pos; // reversed since y-coordinates range from bottom to top

	state->last_mouse_x = x_pos;
	state->last_mouse_y = y_pos;

	Camera* fps_camera = state->m_camera;
	constexpr float speed = 0.3f;
	fps_camera->Rotate(static_cast<float>(x_offset * speed), static_cast<float>(y_offset * speed), state->m_dt);
}

void handle_key_input(GLFWwindow* window, engine_state* state)
{
	input* user_input = state->m_input;


	user_input->move_left = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	user_input->move_right = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	user_input->move_up = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	user_input->move_down = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	user_input->go_up = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	user_input->go_down = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;

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
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		state->mode = 5;
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		state->mode = 6;
	}
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
	{
		state->mode = 7;
	}
	else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
	{
		state->mode = 8;
	}
	else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		state->shading_mode = FLAT;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		state->shading_mode = GOURAUD;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		state->shading_mode = PHONG;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}


	// Add other mode keys here
}


void mode_vertices(const std::vector<float>& cube_vertices, const unsigned cube_vao, const Shader* current_shader)
{

	current_shader->SetUniform3f("uColor", glm::vec3(0.9f));
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_POINTS, 0, cube_vertices.size() / 8);
	glBindVertexArray(0);
	glPointSize(1.0f);

}

void mode_polygon_lines(const std::vector<float>& cube_vertices, const unsigned cube_vao, const Shader* current_shader)
{

	current_shader->SetUniform3f("uColor", glm::vec3(0.91f));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size() / 8);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void mode_polygon_filled(const std::vector<float>& cube_vertices, const unsigned cube_vao, const Shader* current_shader)
{

	current_shader->SetUniform3f("uColor", glm::vec3(0.4f));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size() / 8);
	glBindVertexArray(0);
}

void mode_polygon_lines_and_filled(const std::vector<float>& cube_vertices, const unsigned cube_vao, const Shader* current_shader)
{
	mode_polygon_lines(cube_vertices, cube_vao, current_shader);
	mode_polygon_filled(cube_vertices, cube_vao, current_shader);
}

void mode_normals(const std::vector<float>& cube_vertices, const unsigned cube_vao, const Shader* current_shader, const std::vector<float>& normal_line_vertices, const unsigned normal_lines_vao)
{

	current_shader->SetUniform3f("uColor", glm::vec3(0.0, 0.5, 0.00));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(normal_lines_vao);
	glDrawArrays(GL_LINES, 0, normal_line_vertices.size() / 3);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void mode_averaged_normals(const Shader* current_shader, const std::vector<float>& averaged_normal_vertices, const unsigned averaged_normal_lines_vao, const std::vector<float>& cube_vertices, const unsigned cube_vao)
{

	mode_polygon_lines_and_filled(cube_vertices, cube_vao, current_shader);

	current_shader->SetUniform3f("uColor", glm::vec3(0.28, 1, 0.00));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(averaged_normal_lines_vao);
	glDrawArrays(GL_LINES, 0, averaged_normal_vertices.size() / 3);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void mode_render_vertices(Model model, Shader* current_shader, glm::vec3 color)
{

	current_shader->SetUniform3f("uColor", color);
	model.RenderVertices();

}

void mode_render_triangles(Model model, Shader* current_shader, glm::vec3 color)
{

	current_shader->SetUniform3f("uColor", color);
	model.RenderTriangles();

}

void mode_render_filled_triangles(Model model, Shader* current_shader, glm::vec3 color)
{
	current_shader->SetUniform3f("uColor", color);
	model.RenderFilledTriangles();

}

int main()
{
	GLFWwindow* window = nullptr;
	if (!glfwInit())
	{
		std::cerr << "Failed to init glfw" << std::endl;
		return -1;
	}
	const std::string window_title = "OpenGLDemo";
	int window_width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	int window_height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide the cursor
	glEnable(GL_MULTISAMPLE);
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cerr << "Failed to init glew: " << glewGetErrorString(error) << std::endl;
		glfwTerminate();
		return -1;
	}
	engine_state state;
	Camera fps_camera;
	input user_input;
	state.m_camera = &fps_camera;
	state.m_input = &user_input;
	glfwSetWindowUserPointer(window, &state);
	glfwSetErrorCallback(error_callback);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Model model("res/Woman/091_W_Aya_100K.obj");
	Model model("res/moto_simple_1.obj");
	if (!model.Load())
	{
		std::cerr << "Failed to load model\n";
		glfwTerminate();
		return -1;
	}

	std::vector<float> cube_vertices = model.GetVertices();
	std::vector<float> cube_vertices1 =
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


	unsigned cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	unsigned cube_vbo;
	glGenBuffers(1, &cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(float), cube_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader phong_shader_material_texture("shaders/basic.vert", "shaders/phong_material_texture.frag");
	Shader color_only("shaders/basic.vert", "shaders/color.frag");
	glUseProgram(phong_shader_material_texture.GetId());

	phong_shader_material_texture.SetUniform3f("uColor", glm::vec3(1, 1, 1));

	// Default for point light (Sun)
	phong_shader_material_texture.SetUniform3f("uSunLight.Ka", glm::vec3(1.00, 0.97, 0.00));
	phong_shader_material_texture.SetUniform3f("uSunLight.Kd", glm::vec3(1.00, 0.97, 0.00));
	phong_shader_material_texture.SetUniform3f("uSunLight.Ks", glm::vec3(0));

	// Light from the FlashLight 
	phong_shader_material_texture.SetUniform3f("uFlashLight.Ka", glm::vec3(0));
	phong_shader_material_texture.SetUniform3f("uFlashLight.Kd", glm::vec3(1));
	phong_shader_material_texture.SetUniform3f("uFlashLight.Ks", glm::vec3(1));
	phong_shader_material_texture.SetUniform1f("uFlashLight.Kc", 0.7f);
	phong_shader_material_texture.SetUniform1f("uFlashLight.Kl", 0.0002f);
	phong_shader_material_texture.SetUniform1f("uFlashLight.Kq", 0.0002f);
	phong_shader_material_texture.SetUniform1f("uFlashLight.InnerCutOff", glm::cos(glm::radians(1.0f)));
	phong_shader_material_texture.SetUniform1f("uFlashLight.OuterCutOff", glm::cos(glm::radians(30.0f)));

	// Materials
	phong_shader_material_texture.SetUniform1f("uMaterial.Ka", 1); // *** Pre nije bilo tu vidi cemu sluzi
	phong_shader_material_texture.SetUniform1i("uMaterial.Kd", 0);
	phong_shader_material_texture.SetUniform1i("uMaterial.Ks", 1);
	phong_shader_material_texture.SetUniform1f("uMaterial.Shininess", 128);

	// Diffuse texture
	unsigned rock_diffuse_texture = Texture::LoadImageToTexture("res/test.png");

	// Start values of variables
	Shader* current_shader = &phong_shader_material_texture;
	bool flash_light = false;
	bool show_gui = true;
	bool is_f_key_pressed = false;
	bool is_q_key_pressed = false;
	double start_time;
	glm::mat4 model_matrix(1.0f);
	float background_color = 0.01f;
	glClearColor(background_color, background_color, background_color, 1.0f);

	//////////////////////////////////////////////////////////////////////////////////////////
	// 
	// Calculate the normal lines' vertices
	std::vector<float> normal_line_vertices;
	for (size_t i = 0; i < cube_vertices.size(); i += 8) {
		float x = cube_vertices[i];
		float y = cube_vertices[i + 1];
		float z = cube_vertices[i + 2];
		float nx = cube_vertices[i + 3];
		float ny = cube_vertices[i + 4];
		float nz = cube_vertices[i + 5];

		// Calculate the endpoints of the normal line
		glm::vec3 start_point(x, y, z);
		glm::vec3 direction(nx, ny, nz);

		// Normalize the direction vector
		direction = normalize(direction);

		glm::vec3 scaled_direction = 0.5f * direction;
		glm::vec3 end_point = start_point + scaled_direction;

		normal_line_vertices.push_back(start_point.x);
		normal_line_vertices.push_back(start_point.y);
		normal_line_vertices.push_back(start_point.z);
		normal_line_vertices.push_back(end_point.x);
		normal_line_vertices.push_back(end_point.y);
		normal_line_vertices.push_back(end_point.z);
	}
	unsigned normal_lines_vao;
	glGenVertexArrays(1, &normal_lines_vao);
	glBindVertexArray(normal_lines_vao);

	unsigned normal_lines_vbo;
	glGenBuffers(1, &normal_lines_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normal_lines_vbo);
	glBufferData(GL_ARRAY_BUFFER, normal_line_vertices.size() * sizeof(float), normal_line_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// Unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::vector<float> averaged_normal_vertices;

	// Calculate averaged normals



	std::ifstream inputFile("averaged_normal_vertices.txt");
	if (inputFile.is_open()) {
		float value;
		while (inputFile >> value) {
			averaged_normal_vertices.push_back(value);
		}
		inputFile.close();
	}
	else {
		// Calculate averaged normals and populate the vector
		std::vector<glm::vec3> added_to_vertices;;
		for (size_t i = 0; i < cube_vertices.size(); i += 8) {
			glm::vec3 averaged_normal(0.0f);
			std::vector<glm::vec3> added_to_current_normal_calculation;

			if (i % 8000 == 0)
			{
				std::cout << i << "\n";
				std::cout << cube_vertices.size() << "\n";
			}

			float start_x = cube_vertices[i];
			float start_y = cube_vertices[i + 1];
			float start_z = cube_vertices[i + 2];

			if (!containsElement(glm::vec3(start_x, start_y, start_z), added_to_vertices))
			{
				added_to_vertices.push_back(glm::vec3(start_x, start_y, start_z));
			}
			else
			{
				continue;
			}

			// Calculate the averaged normal by summing up normals with the same starting point
			for (size_t j = i; j < cube_vertices.size(); j += 8) {
				float current_x = cube_vertices[j];
				float current_y = cube_vertices[j + 1];
				float current_z = cube_vertices[j + 2];
				float current_nx = cube_vertices[j + 3];
				float current_ny = cube_vertices[j + 4];
				float current_nz = cube_vertices[j + 5];


				if (start_x == current_x && start_y == current_y && start_z == current_z) {
					glm::vec3 current_normal(current_nx, current_ny, current_nz);
					if (!containsElement(current_normal, added_to_current_normal_calculation)) {
						averaged_normal += current_normal;
						added_to_current_normal_calculation.push_back(current_normal);
					}
				}
			}

			if (averaged_normal != glm::vec3(0.0f)) {
				averaged_normal = static_cast<float>(1.00 / added_to_current_normal_calculation.size()) * averaged_normal;
				averaged_normal = glm::normalize(averaged_normal);

				glm::vec3 scaled_direction = 5.0f * averaged_normal;
				glm::vec3 end_point = glm::vec3(start_x, start_y, start_z) + scaled_direction;

				averaged_normal_vertices.push_back(start_x);
				averaged_normal_vertices.push_back(start_y);
				averaged_normal_vertices.push_back(start_z);
				averaged_normal_vertices.push_back(end_point.x);
				averaged_normal_vertices.push_back(end_point.y);
				averaged_normal_vertices.push_back(end_point.z);


			}
		
		}

		// Save the calculated data to the file
		std::ofstream outputFile("averaged_normal_vertices.txt");
		if (outputFile.is_open()) {
			for (const float& value : averaged_normal_vertices) {
				outputFile << value << "\n";
			}
			outputFile.close();
		}
		else {
			std::cerr << "Unable to save data to file.\n";
		}
	}

	unsigned averaged_normal_lines_vao;
	glGenVertexArrays(1, &averaged_normal_lines_vao);
	glBindVertexArray(averaged_normal_lines_vao);

	unsigned averaged_normal_lines_vbo;
	glGenBuffers(1, &averaged_normal_lines_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, averaged_normal_lines_vbo);
	glBufferData(GL_ARRAY_BUFFER, averaged_normal_vertices.size() * sizeof(float), averaged_normal_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// Unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	//////////////////////////////////////////////////////////////////////////////////////////
	// For text

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("res/FreeSans-LrmZ.ttf", 14);
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();


	glm::vec3 material_ka(0, 0, 0);
	glm::vec3 material_kd(0, 0, 0);
	glm::vec3 material_ks(0, 0, 0);
	float Shininess = 0;

	/// joj
	auto tacke = model.GetVertices();
	unsigned tacke_vao;
	glGenVertexArrays(1, &tacke_vao);
	glBindVertexArray(tacke_vao);

	unsigned tacke_vbo;
	glGenBuffers(1, &tacke_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tacke_vbo);
	glBufferData(GL_ARRAY_BUFFER, tacke.size() * sizeof(float), tacke.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// Unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	while (!glfwWindowShouldClose(window)) {
		start_time = glfwGetTime();
		glfwPollEvents();
		handle_key_input(window, &state);
		handle_input(&state);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(current_shader->GetId());
		current_shader->SetProjection(glm::perspective(70.0f, static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 10000.0f));
		current_shader->SetView(glm::lookAt(fps_camera.GetPosition(), fps_camera.GetTarget(), fps_camera.GetUp()));
		current_shader->SetUniform3f("uViewPos", fps_camera.GetPosition());
		current_shader->SetModel(model_matrix);

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			if (!is_f_key_pressed)
			{
				is_f_key_pressed = true;
				flash_light = !flash_light;
			}
		}
		else
		{
			is_f_key_pressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			if (!is_q_key_pressed)
			{
				is_q_key_pressed = true;
				show_gui = !show_gui;
			}
		}
		else
		{
			is_q_key_pressed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			state.enable_mouse_callback = false;

		}
		else
		{
			state.enable_mouse_callback = true;
		}




		if (flash_light)
		{
			glm::vec3 pos = fps_camera.GetTarget() - fps_camera.GetPosition();
			current_shader->SetUniform3f("uFlashLight.Position", glm::vec3(fps_camera.GetPosition()));
			current_shader->SetUniform3f("uFlashLight.Direction", glm::vec3(pos.x, pos.y, pos.z));
			current_shader->SetUniform3f("uFlashLight.Kd", glm::vec3(1));
			current_shader->SetUniform3f("uFlashLight.Ks", glm::vec3(1));
		}
		else
		{
			current_shader->SetUniform3f("uFlashLight.Kd", glm::vec3(0));
			current_shader->SetUniform3f("uFlashLight.Ks", glm::vec3(0));
		}

		current_shader->SetUniform3f("uDirLight.Direction", glm::vec3(0, -0.1, 0));
		current_shader->SetUniform3f("uDirLight.Ka", glm::vec3(0.68, 0.70, 0.51));
		current_shader->SetUniform3f("uDirLight.Kd", glm::vec3(0.68, 0.70, 0.51));
		current_shader->SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));

		glm::vec3 point_light_position_sun(15, 5, 0);
		current_shader->SetUniform1f("uSunLight.Kc", 0.1 / abs(sin(start_time)));
		current_shader->SetUniform1f("uSunLight.Kq", 0.1 / abs(sin(start_time)));
		current_shader->SetUniform3f("uSunLight.Position", point_light_position_sun);
		current_shader->SetUniform1f("uIsDrawingLines", 0);

		switch (state.mode)
		{
		case 1:
			current_shader = &color_only;
			mode_render_vertices(model, current_shader, glm::vec3(1.0f));
			break;
		case 2:
			current_shader = &color_only;
			mode_render_triangles(model, current_shader, glm::vec3(1.0f));
			break;
		case 3:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(0.5f));
			break;
		case 4:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(0.5f));
			mode_render_triangles(model, current_shader, glm::vec3(1.0f));
			break;
		case 5:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(0.5f));
			mode_render_triangles(model, current_shader, glm::vec3(1.0f));
			mode_normals(cube_vertices, cube_vao, current_shader, normal_line_vertices, normal_lines_vao);
			break;
		case 6:
			current_shader = &color_only;
			mode_averaged_normals(current_shader, averaged_normal_vertices, averaged_normal_lines_vao, cube_vertices, cube_vao);
			break;
		case 7:
		
		
			break;
		case 8:
			current_shader = &phong_shader_material_texture;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, rock_diffuse_texture);
			model.RenderWithTexture();
		


			break;
		default:
			break;
		}





		glBindVertexArray(0);
		glUseProgram(0);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////


		if (show_gui)
		{

			ImGui_ImplGlfwGL3_NewFrame();
			float margin_precentage = 0.025f;
			int marginLeft = static_cast<int>(margin_precentage * window_width);
			int marginTop = static_cast<int>(margin_precentage * window_height);
			int marginBottom = static_cast<int>((0.8 - margin_precentage) * window_height);
			int marginRight = static_cast<int>((1 - margin_precentage) * window_width);
			ImGui::SetNextWindowPos(ImVec2(marginLeft, marginTop), ImGuiCond_Always, ImVec2(0.0f, 0.0f));

			ImGui::Begin("Modes", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("Selected mode:");
			ImVec4 text_color(0.0f, 1.0f, 0.0f, 1.0f); // Green color
			const char* modesText[] = {
		"Mode 01 - Vertices",
		"Mode 02 - Triangles",
		"Mode 03 - Filled",
		"Mode 04 - Filled with triangles",
		"Mode 05 - All normals",
		"Mode 06 - Averaged normals",
		"Mode 07 - Shading",
		"Mode 08 - Texture",

			};

			for (int i = 0; i < std::size(modesText); ++i)
			{
				if (state.mode == i + 1)
				{
					ImGui::TextColored(text_color, modesText[i]);
				}
				else
				{
					ImGui::Text(modesText[i]);
				}
			}
			ImGui::Separator();
			ImGui::Separator();

			text_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			const char* shadingText[] = {
		"Flat",
		"Gouraud",
		"Phong",
			};
			ImGui::Text("Selected shading type:");
			for (int i = FLAT; i <= PHONG; ++i)
			{
				if (state.shading_mode == i)
				{
					ImGui::TextColored(text_color, shadingText[i]);
				}
				else
				{
					ImGui::Text(shadingText[i]);
				}
			}
			float test = 0;
			ImVec4 sliderColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			ImVec4 activeSliderColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

			ImGui::Separator();
			ImGui::Separator();
			ImGui::Text("Material components");
			ImGui::Text("Ambient:");

			ImVec4 frameBgColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Ambient Red", &material_ka.r, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);

			frameBgColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Ambient Green", &material_ka.g, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);

			frameBgColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Ambient Blue", &material_ka.b, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);
			ImGui::Separator();

			ImGui::Text("Diffuse:");

			frameBgColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Diffuse Red", &material_kd.r, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);

			frameBgColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Diffuse Green", &material_kd.g, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);

			frameBgColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Diffuse Blue", &material_kd.b, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);
			ImGui::Separator();

			ImGui::Text("Specular:");

			frameBgColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Specular Red", &material_ks.r, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);

			frameBgColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Specular Green", &material_ks.g, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);

			frameBgColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Specular Blue", &material_ks.b, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);
			ImGui::Separator();

			ImGui::Text("Shininess:");
			frameBgColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, activeSliderColor);
			ImGui::SliderFloat("Strength", &Shininess, 0.0f, 1.0f);
			ImGui::PopStyleColor(5);


			ImGui::End();


			ImGui::SetNextWindowPos(ImVec2(marginRight, marginTop), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
			ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("GUI toggle - Q");
			ImGui::Text("Movement - W A S D");
			ImGui::Text("Up / Down - Space / C");
			ImGui::Text("Camera - Mouse");
			ImGui::Text("Flashlight toggle - F");
			ImGui::Text("Disable mouse - E (hold)");
			ImGui::Separator();
			ImGui::Text("Switching modes - 1 2 3 4 5 6 7 8");
			ImGui::Separator();
			ImGui::Text("Switching shading type:");
			ImGui::Text("Flat - I");
			ImGui::Text("Gouraud - O");
			ImGui::Text("Phong - P");
			ImGui::End();


			ImGui::SetNextWindowPos(ImVec2(marginRight, marginBottom), ImGuiCond_Always, ImVec2(1.0f, 1.0f));

			ImGui::Begin("Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("TBAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

			ImGui::End();

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
		state.m_dt = glfwGetTime() - start_time;
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
