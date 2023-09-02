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
	flat,
	gouraud,
	phong
};


struct engine_state
{
	input* m_input;
	Camera* m_camera;
	double m_dt;
	int mode = 7;
	double last_mouse_x = 0;
	double last_mouse_y = 0;
	bool first_mouse = true;
	bool enable_mouse_callback = true;
	shading_mode shading_mode = flat;
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
	const double y_offset = state->last_mouse_y - y_pos;
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
		state->shading_mode = flat;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		state->shading_mode = gouraud;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		state->shading_mode = phong;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}


void mode_averaged_normals(const Shader* current_shader, const std::vector<float>& averaged_normal_vertices, const unsigned averaged_normal_lines_vao, const std::vector<float>& cube_vertices, const glm::vec3 color)
{
	current_shader->SetUniform3f("uColor", color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(averaged_normal_lines_vao);
	glDrawArrays(GL_LINES, 0, averaged_normal_vertices.size() / 3);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void mode_render_vertices(Model model, const Shader* current_shader, const glm::vec3 color, const float point_size)
{
	glPointSize(point_size);
	current_shader->SetUniform3f("uColor", color);
	model.RenderVertices();
}

void mode_render_triangles(Model model, const Shader* current_shader, const glm::vec3 color)
{

	current_shader->SetUniform3f("uColor", color);
	model.RenderTriangles();
}

void mode_render_filled_triangles(Model model, const Shader* current_shader, const glm::vec3 color)
{
	current_shader->SetUniform3f("uColor", color);
	model.RenderFilledTriangles();
}

void mode_render_normals(Model model, const Shader* current_shader, glm::vec3 all_normals_color)
{
	current_shader->SetUniform3f("uColor", glm::vec3(all_normals_color));
	model.RenderNormals();
}

void mode_averaged_normals(Model model, const Shader* current_shader, const glm::vec3 averaged_normals_color)
{
	current_shader->SetUniform3f("uColor", averaged_normals_color);
	model.RenderAveragedNormals();
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
	//Model model("res/12190_Heart_v1_L3.obj");
	if (!model.Load())
	{
		std::cerr << "Failed to load model\n";
		glfwTerminate();
		return -1;
	}

	Shader color_only("shaders/basic.vert", "shaders/color.frag");
	Shader gouraud_shader_material("shaders/gouraud.vert", "shaders/gouraud.frag");
	Shader phong_shader_material("shaders/basic.vert", "shaders/phong_material.frag");
	Shader phong_shader_material_texture("shaders/basic.vert", "shaders/phong_material_texture.frag");

	glUseProgram(phong_shader_material.GetId());

	// Diffuse texture
	unsigned test_texture = Texture::LoadImageToTexture("res/test.png");

	// Start values of variables
	Shader* current_shader = &phong_shader_material;
	bool flash_light = false;
	bool show_gui = true;
	bool is_f_key_pressed = false;
	bool is_q_key_pressed = false;
	double start_time;
	float filled_color = 0.3f;
	float points_and_lines_color = 1.0f;
	auto all_normals_color = glm::vec3(0.7, 0.7, 0.0);
	auto averaged_normals_color = glm::vec3(0.5, 0.5, 0.0);
	glm::mat4 model_matrix(1.0f);

	glm::vec3 material_ka(1, 1, 1);
	glm::vec3 material_kd(1, 1, 1);
	glm::vec3 material_ks(1, 1, 1);
	float shininess = 1;

	float background_color = 0.0f;
	glClearColor(background_color, background_color, background_color, 1.0f);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("res/FreeSans-LrmZ.ttf", 14);
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

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

		glm::vec3 point_light_position_sun(-1115, 5, 0);
		current_shader->SetUniform3f("uSunLight.Position", point_light_position_sun);
		current_shader->SetUniform1f("uSunLight.Kc", 1);
		current_shader->SetUniform1f("uSunLight.Kq", 1);
		current_shader->SetUniform1f("uSunLight.Kl", 1);
		current_shader->SetUniform3f("uSunLight.Ka", glm::vec3(1.00, 0.97, 0.00));
		current_shader->SetUniform3f("uSunLight.Kd", glm::vec3(1.00, 0.97, 0.00));
		current_shader->SetUniform3f("uSunLight.Ks", glm::vec3(1.0));

		current_shader->SetUniform3f("uFlashLight.Position", glm::vec3(fps_camera.GetPosition()));
		current_shader->SetUniform3f("uFlashLight.Direction", glm::vec3(0));
		current_shader->SetUniform3f("uFlashLight.Ka", glm::vec3(0));
		current_shader->SetUniform1f("uFlashLight.Kc", 0.6f);
		current_shader->SetUniform1f("uFlashLight.Kl", 0.0002f);
		current_shader->SetUniform1f("uFlashLight.Kq", 0.0002f);
		current_shader->SetUniform1f("uFlashLight.InnerCutOff", glm::cos(glm::radians(1.0f)));
		current_shader->SetUniform1f("uFlashLight.OuterCutOff", glm::cos(glm::radians(30.0f)));

		current_shader->SetUniform3f("uMaterial.Ka", material_ka); // *** Check what is it for
		current_shader->SetUniform3f("uMaterial.Kd", material_kd);
		current_shader->SetUniform3f("uMaterial.Ks", material_ks);
		current_shader->SetUniform1f("uMaterial.Shininess", shininess * 128);

		current_shader->SetUniform3f("uDirLight.Direction", glm::vec3(0, -0.1, 0));
		current_shader->SetUniform3f("uDirLight.Ka", glm::vec3(0.50));
		current_shader->SetUniform3f("uDirLight.Kd", glm::vec3(0.50));
		current_shader->SetUniform3f("uDirLight.Ks", glm::vec3(1));




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

		switch (state.mode)
		{
		case 1:
			current_shader = &color_only;
			mode_render_vertices(model, current_shader, glm::vec3(points_and_lines_color), 2);
			break;
		case 2:
			current_shader = &color_only;
			mode_render_triangles(model, current_shader, glm::vec3(points_and_lines_color));
			break;
		case 3:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(filled_color));
			break;
		case 4:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(filled_color));
			mode_render_triangles(model, current_shader, glm::vec3(points_and_lines_color));
			break;
		case 5:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(filled_color));
			mode_render_triangles(model, current_shader, glm::vec3(points_and_lines_color));
			mode_render_normals(model, current_shader, all_normals_color);
			break;
		case 6:
			current_shader = &color_only;
			mode_render_filled_triangles(model, current_shader, glm::vec3(filled_color));
			mode_render_triangles(model, current_shader, glm::vec3(points_and_lines_color));
			mode_averaged_normals(model, current_shader, averaged_normals_color);

			break;
		case 7:
			switch (state.shading_mode)
			{
			case flat:
				current_shader = &gouraud_shader_material;
				glUseProgram(current_shader->GetId());
				model.RenderFlat();
				break;
			case gouraud:
				current_shader = &gouraud_shader_material;
				glUseProgram(current_shader->GetId());
				model.RenderSmooth();
				break;
			case phong:
				current_shader = &phong_shader_material;
				glUseProgram(current_shader->GetId());
				model.RenderSmooth();
				break;

			}
			break;
		case 8:
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, test_texture);
			current_shader = &phong_shader_material_texture;
			glUseProgram(current_shader->GetId());
			model.RenderSmooth();
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
			float margin_percentage = 0.025f;
			int margin_left = static_cast<int>(margin_percentage * window_width);
			int margin_top = static_cast<int>(margin_percentage * window_height);
			int margin_right = static_cast<int>((1 - margin_percentage) * window_width);
			ImGui::SetNextWindowPos(ImVec2(margin_left, margin_top), ImGuiCond_Always, ImVec2(0.0f, 0.0f));

			ImGui::Begin("Modes", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("Selected mode:");
			ImVec4 text_color(0.0f, 1.0f, 0.0f, 1.0f); // Green color
			const char* modes_text[] = {
		"Mode 01 - Vertices",
		"Mode 02 - Triangles",
		"Mode 03 - Filled",
		"Mode 04 - Filled with triangles",
		"Mode 05 - All normals",
		"Mode 06 - Averaged normals",
		"Mode 07 - Shading",
		"Mode 08 - Texture",
			};
			for (int i = 0; i < std::size(modes_text); ++i)
			{
				if (state.mode == i + 1)
				{
					ImGui::TextColored(text_color, modes_text[i]);
				}
				else
				{
					ImGui::Text(modes_text[i]);
				}
			}

			if (state.mode == 7)
			{
				ImGui::Separator();
				ImGui::Separator();
				text_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
				const char* shading_text[] = {
			"Flat",
			"Gouraud",
			"Phong",
				};
				ImGui::Text("Selected shading type:");
				for (int i = flat; i <= phong; ++i)
				{
					if (state.shading_mode == i)
					{
						ImGui::TextColored(text_color, shading_text[i]);
					}
					else
					{
						ImGui::Text(shading_text[i]);
					}
				}
				auto slider_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
				auto active_slider_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

				ImGui::Separator();
				ImGui::Separator();
				ImGui::Text("Material components");
				ImGui::Text("Ambient:");

				auto frame_bg_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Ambient Red", &material_ka.r, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);

				frame_bg_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Ambient Green", &material_ka.g, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);

				frame_bg_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Ambient Blue", &material_ka.b, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);
				ImGui::Separator();

				ImGui::Text("Diffuse:");

				frame_bg_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Diffuse Red", &material_kd.r, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);

				frame_bg_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Diffuse Green", &material_kd.g, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);

				frame_bg_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Diffuse Blue", &material_kd.b, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);
				ImGui::Separator();

				ImGui::Text("Specular:");

				frame_bg_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Specular Red", &material_ks.r, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);

				frame_bg_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Specular Green", &material_ks.g, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);

				frame_bg_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Specular Blue", &material_ks.b, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);
				ImGui::Separator();

				ImGui::Text("Shininess:");

				frame_bg_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_bg_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, slider_color);
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active_slider_color);
				ImGui::SliderFloat("Strength", &shininess, 0.0f, 1.0f);
				ImGui::PopStyleColor(5);
			}


			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(margin_right, margin_top), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
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
