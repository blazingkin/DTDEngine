/*
 * Program 2 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <stdio.h>
#include <dirent.h>
#include <ctime>
#include "Camera.h"
#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "BScene.h"
#include "perlin.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "AssetHelper.h"
#include "Utils/ColorConversion.h"
#include "Particles.h"
#include <thread>
#include "Audio.h"
#include "Scenes/GameScene.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Systems/SystemRender.h"
#include "Systems/SystemPhysics.h"
#include "Systems/SystemShadows.h"
#include "Systems/SystemApplyPostProcessing.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#define _USE_MATH_DEFINES
#include <math.h>


#include <rapidjson/document.h>
#include "Config.h"
// #include "Audio.h"

//#include <iostream>
//#include "stb_gen.h"

Document config;

using namespace std;
using namespace glm;


void loadConfig(const string resourceDirectory) {
	cout << "Loading Configs" << endl;
	struct dirent *entry = nullptr;
	DIR *dir = nullptr;
	dir = opendir((resourceDirectory + "/config").c_str());
	if(dir == nullptr){
		std::cerr << "Resource directory did not exist!" << std::endl;
		std::exit(1);
	}
	string content;
	while ((entry = readdir(dir))) {
		if (std::string(entry->d_name).find("global_config.json") != string::npos) {
			ifstream ifs(resourceDirectory + "/config/" + entry->d_name);
			content.assign(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
			cout << "Loaded " << entry->d_name << endl;
		}
	}

	config.Parse(content.c_str());	
}


#ifdef _DEBUG_OPENGL
		void GLAPIENTRY
	MessageCallback( GLenum source,
					GLenum type,
					GLuint id,
					GLenum severity,
					GLsizei length,
					const GLchar* message,
					const void* userParam )
	{
	fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
				type, severity, message );
	}
#endif
float map_size = 100;
class Application : public EventCallbacks
{

public:

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		/*auto io = ImGui::GetIO();
		if (io.WantCaptureKeyboard) {
			return;
		}*/
		if (key == GLFW_KEY_ESCAPE && action == GLFW_REPEAT) {
        	glfwSetWindowShouldClose(window, GL_TRUE);
    	}
		if (ActiveScene() != nullptr && ActiveScene()->handleKey != nullptr) {
			ActiveScene()->handleKey(window, key, scancode, action, mods, ActiveScene());
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {

	}

	
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		/*auto io = ImGui::GetIO();
		if (io.WantCaptureMouse) {
			return;
		}*/
		if (ActiveScene() != nullptr && ActiveScene()->handleClick != nullptr) {
			ActiveScene()->handleClick(window, button, action, mods, ActiveScene());
		}
	}


	void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (ActiveScene() != nullptr && ActiveScene()->onCursorUpdate != nullptr) {
			ActiveScene()->onCursorUpdate(window, xpos, ypos, ActiveScene());
		}
	}


	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		if (width <= 0 || height <= 0) {
			return;
		}
		postprocessingOnResize(width, height);
		glViewport(0, 0, width, height);
	}

	void initUI(const std::string& resourceDirectory) {
		struct dirent *entry = nullptr;
		DIR *dir = nullptr;
		dir = opendir((resourceDirectory + "/ui/textures").c_str());
		if (dir != nullptr) {
			while ((entry = readdir(dir))) {
				if (std::string(entry->d_name).find(".jpg") != string::npos ||
					std::string(entry->d_name).find(".png") != string::npos) {
					auto tex = new Texture();
					tex->setFilename(resourceDirectory + "/ui/textures/" + entry->d_name);
					tex->init();
					tex->setUnit(texUnit++);
					tex->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE); 
					uiTextures[entry->d_name] = make_shared<Texture>(*tex);
				}
			}
		}
	}

	void initScene() {
		scenes["main"] = *new BScene(vec2(map_size/2, map_size/2));
		InitGameScene(&scenes["main"], 0);
		SetActiveScene("main");
	}
	
	void init(const std::string& resourceDirectory)
	{
		
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		#ifdef _DEBUG_OPENGL
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(MessageCallback, 0);
		#endif

		/* Compile all shaders */
		struct dirent *entry = nullptr;
		DIR *dir = nullptr;
		dir = opendir((resourceDirectory + "/shaders/vertex").c_str());
		if (dir != nullptr) {
			while ((entry = readdir(dir))) {
				if (std::string(entry->d_name).find(".glsl") != string::npos) {
					compile_vertex_shader(resourceDirectory + "/shaders/vertex", entry->d_name);
				}
			}
		}

		dir = opendir((resourceDirectory + "/shaders/fragment").c_str());
		if (dir != nullptr) {
			while ((entry = readdir(dir))) {
				if (std::string(entry->d_name).find(".glsl") != string::npos) {
					compile_fragment_shader(resourceDirectory + "/shaders/fragment", entry->d_name);
				}
			}
		}


	}

	void initTextures(std::string resourceDirectory) {
		struct dirent *entry = nullptr;
		DIR *dir = opendir((resourceDirectory + "/skyboxes").c_str());
		if (dir != nullptr) {
			while ((entry = readdir(dir))) {
				if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
					auto sb = make_shared<Skybox>();
					sb->init(resourceDirectory + "/skyboxes/" + entry->d_name + "/");
					skyboxes[entry->d_name] = sb;
				}
			}
		}


		dir = opendir((resourceDirectory + "/textures").c_str());
		if (dir != nullptr) {
			while ((entry = readdir(dir))) {
				if (std::string(entry->d_name).find(".jpg") != string::npos ||
					std::string(entry->d_name).find(".png") != string::npos) {
					auto tex = new Texture();
					tex->setFilename(resourceDirectory + "/textures/" + entry->d_name);
					tex->init();
					tex->setUnit(texUnit++); 
					tex->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE); 
					textures[entry->d_name] = make_shared<Texture>(*tex);
				}
			}
		}
	}

	void initGeom(const std::string& resourceDirectory)
	{
		importModelsFrom((resourceDirectory + "/models"), "");
	}


	void render() {
		// Get current frame buffer size.
		int width, height;
		auto scene = ActiveScene();
		RenderDepthMap(scene->getEntitiesWithComponents({COMPONENT_LOCATION, COMPONENT_MODEL, COMPONENT_RENDERABLE}), scene, map_size);
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		if (width == 0 || height == 0) {
			return;
		}
		auto tex = RenderScene(scene, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		applyPostprocessing(scene, tex);
		if (scene->updateGUI != nullptr) {
			scene->updateGUI(scene, width, height);	
		}
		//set viewport for minimap	
	}
};

int main(int argc, char *argv[])
{
	srand(time(NULL));

	// Where the resources are loaded from
	std::string resourceDir = "../resources";
	
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	loadConfig(resourceDir);
	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);

	// Clear the viewport while we are loading in assets
	glViewport(0, 0, 640, 480);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(windowManager->getHandle());
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->initUI(resourceDir);
	application->init(resourceDir);
	application->initTextures(resourceDir);
	initMaterials(resourceDir);
	application->initGeom(resourceDir);
	initAudio(resourceDir);
	application->initScene();
	initShadow(); // OpenGL calls must be on main thread
	setupParticleSystem(); // OpenGL calls must be on main thread
	initSystemRender();
	initializePostprocessing(); // OpenGL calls must be on the main thread



	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		auto scene = ActiveScene();
		// Update scene.
		if (scene != nullptr && scene->update != nullptr) {
			scene->update(scene, windowManager);
		}
		// Update location for 3d sound
		if (scene != nullptr && scene->camera != nullptr) {
			irrklang::vec3df position(scene->camera->eye.x, scene->camera->eye.y, scene->camera->eye.z);        // position of the listener
			irrklang::vec3df lookDirection(scene->camera->lookAt.x, scene->camera->lookAt.y, scene->camera->lookAt.z); // the direction the listener looks into
			irrklang::vec3df velPerSecond(0,0,0);    // only relevant for doppler effects
			irrklang::vec3df upVector(0,1,0);        // where 'up' is in your 3D scene
			soundEngine->setListenerPosition(position, lookDirection, velPerSecond, upVector);
		}

		// Render
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();

	// Quit program.
	windowManager->shutdown();
	return 0;
}
