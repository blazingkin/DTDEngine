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
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#define _USE_MATH_DEFINES
#include <math.h>


#include <rapidjson/document.h>
#include "Config.h"
// #include "Audio.h"

//#include <iostream>
//#include "stb_gen.h"

#define USE_LOADED_MAPS = true;

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



	// Our shader program
	std::shared_ptr<Program> prog, noShadowProg;
	std::shared_ptr<Program> skyboxProg;


	std::shared_ptr<Program> loadingScreenProg;
 
	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> houseMesh;
	shared_ptr<Shape> boxMesh;
	shared_ptr<Shape> sphereMesh;
	Shape mutatingMesh;



	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_REPEAT) {
        	glfwSetWindowShouldClose(window, GL_TRUE);
    	}
		if (activeScene != nullptr && activeScene->handleKey != nullptr) {
			activeScene->handleKey(window, key, scancode, action, mods, activeScene);
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {

	}

	
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		if (activeScene != nullptr && activeScene->handleClick != nullptr) {
			activeScene->handleClick(window, button, action, mods, activeScene);
		}
	}


	void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (activeScene != nullptr && activeScene->onCursorUpdate != nullptr) {
			activeScene->onCursorUpdate(window, xpos, ypos, activeScene);
		}
	}


	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		if (width <= 0 || height <= 0) {
			return;
		}
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
		activeScene = &scenes["main"];
		InitGameScene(activeScene, 0);
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

		const vector<string> shaders({"tex_vert.glsl", "blinn_phong.glsl"});
		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->init(shaders);
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("LS");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("shine");
		prog->addUniform("viewVector");
		prog->addUniform("lightPos");
		prog->addUniform("lightColor");
		prog->addUniform("Texture0");
		prog->addUniform("shadowDepth");
		prog->addUniform("textureEnabled");
		prog->addUniform("normalMap");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		const vector<string> NoShadShaders({"tex_vert.glsl", "blinn_phong_no_shadows.glsl"});
		// Initialize the GLSL program.
		noShadowProg = make_shared<Program>();
		noShadowProg->setVerbose(true);
		noShadowProg->init(NoShadShaders);
		noShadowProg->addUniform("P");
		noShadowProg->addUniform("V");
		noShadowProg->addUniform("M");
		noShadowProg->addUniform("viewVector");
		noShadowProg->addUniform("lightPos");
		noShadowProg->addUniform("lightColor");
		noShadowProg->addUniform("Texture0");
		noShadowProg->addUniform("normalMap");
		noShadowProg->addAttribute("vertPos");
		noShadowProg->addAttribute("vertNor");
		noShadowProg->addAttribute("vertTex");

		const vector<string> skyboxShaders({"sky_vert.glsl", "sky_frag.glsl"});
		skyboxProg = make_shared<Program>();
		skyboxProg->setVerbose(true);
		skyboxProg->init(skyboxShaders);
		skyboxProg->addUniform("P");
		skyboxProg->addUniform("V");
		skyboxProg->addUniform("M");
		skyboxProg->addUniform("skybox");
		skyboxProg->addUniform("lightColor");
		skyboxProg->addAttribute("vertPos");
		skyboxProg->addAttribute("vertTex");
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



	void initMaterials(const std::string& resourceDirectory) {
		Material shinyBluePlastic;
			shinyBluePlastic.Ambient = vec3(0.02, 0.04, 0.2);
			shinyBluePlastic.Diffuse = vec3(0, 0.15, 0.9);
			shinyBluePlastic.Specular = vec3(0.14, 0.2, 0.6);
			shinyBluePlastic.shine = 120;
		Material shinyWhitePlastic;
			shinyWhitePlastic.Ambient = vec3(0, 0, 0);
			shinyWhitePlastic.Diffuse = vec3(0.65, 0.65, 0.65);
			shinyWhitePlastic.Specular = vec3(0.20, 0.20, 0.20);
			shinyWhitePlastic.shine = 0.25;
		Material flatGrey;
			flatGrey.Ambient = vec3(0.13, 0.13, 0.14);
			flatGrey.Diffuse = vec3(0.3, 0.3, 0.4);
			flatGrey.Specular = vec3(0.05, 0.05, 0.05);
			flatGrey.shine = 0.2;
		Material darkGrey;
			flatGrey.Ambient = vec3(0.05, 0.05, 0.05);
			flatGrey.Diffuse = vec3(0.05, 0.05, 0.1);
			flatGrey.Specular = vec3(0.025, 0.025, 0.025);
			flatGrey.shine = 0.2;
		Material shinyGrey;
			flatGrey.Ambient = vec3(0.13, 0.13, 0.14);
			flatGrey.Diffuse = vec3(0.3, 0.3, 0.4);
			flatGrey.Specular = vec3(0.1, 0.1, 0.1);
			flatGrey.shine = 0.3;
		Material brass;
			brass.Ambient = vec3(0.3294, 0.2235, 0.02745);
			brass.Diffuse = vec3(0.7804, 0.5686, 0.11373);
			brass.Specular = vec3(0.9922, 0.941176, 0.80784);
			brass.shine = 27.9;
		Material grass;
			grass.Ambient = vec3(0.0, 0.2235, 0.0);
			grass.Diffuse = vec3(0.1004, 0.686, 0.31373);
			grass.Specular = vec3(0.09922, 0.0941176, 0.080784);
			grass.shine = 1;
		Material tree;
			tree.Ambient = vec3(0.0, 0.2235, 0.0);
			tree.Diffuse = vec3(0.1004, 0.686, 0.31373);
			tree.Specular = vec3(0.03, 0.300, 0.080784);
			tree.shine = 1;
		Material fur;
			fur.Ambient = vec3(50.0/255.0, 30.0/255.0, 15.0/255.0);
			fur.Diffuse = vec3(51.0/255.0, 37.0/255.0, 18.0/255.0);
			fur.Specular = vec3(10.0/255.0, 6.0/255.0, 3.0/255.0);
			fur.shine = 0;
		Material shadow;
			shadow.Ambient = vec3(0);
			shadow.Diffuse = vec3(0);
			shadow.Specular = vec3(0);
			shadow.shine = 0;
		Material straw;
			straw.Ambient = vec3(228.0/500.0, 217.0 / 500.0, 111.0 / 500.0);
			straw.Diffuse = vec3(228.0/500.0, 217.0 / 500.0, 111.0 / 500.0);
			straw.Specular = vec3(0.0, 0.0, 0.0);
			straw.shine = 1;
		Material clothing;
			clothing.Ambient = vec3(0.35, 0.35, 0.35);
			clothing.Diffuse = vec3(0.6, 0.6, 0.6);
			clothing.Specular = vec3(0.05, 0.05, 0.05);
			clothing.shine = 1;
		Material tanSkin;
			tanSkin.Ambient = vec3(0.35, 0.35, 0.35);
			tanSkin.Diffuse = vec3(0.5, 0.5, 0.5);
			tanSkin.Specular = vec3(0.15, 0.15, 0.15);
			tanSkin.shine = 1;
		Material virus;
			virus.Ambient = RGBToVec3(45, 15, 45);
			virus.Diffuse = RGBToVec3(0x7C238C) - virus.Ambient;
			virus.Specular = vec3(0.13, 0.0, 0.05);
			virus.shine = 8;
		Material houseSiding;
			houseSiding.Ambient = RGBToVec3(60, 60, 40);
			houseSiding.Diffuse = RGBToVec3(180, 176, 126);
			houseSiding.Specular = RGBToVec3(10,10,10);
			houseSiding.shine = 0.6;
		Material houseRoof;
			houseRoof.Ambient = RGBToVec3(5, 40, 70);
			houseRoof.Diffuse = RGBToVec3(12, 141, 122);
			houseRoof.Specular = RGBToVec3(5,5,5);
			houseRoof.shine = 0.4;
		Material towerMaterial;
			towerMaterial.Ambient = RGBToVec3(60, 80, 60);
			towerMaterial.Diffuse = RGBToVec3(108, 150, 108);
			towerMaterial.Specular = RGBToVec3(20,20,20);
			towerMaterial.shine = 8;
		Material redMat;
			redMat.Ambient = vec3(0.2, 0.04, 0.02);
			redMat.Diffuse = vec3(0.9, 0.15, 0);
			redMat.Specular = vec3(0.6, 0.2, 0.12);
			redMat.shine = 23;
		Material floor;
			floor.Ambient = vec3(0.0, 0.2235, 0.0);
			floor.Diffuse = vec3(0.1004, 0.686, 0.31373);
			floor.Specular = vec3(0.009922, 0.00941176, 0.0080784);
			floor.shine = 0.01;
		Material whiteAmbient;
			whiteAmbient.Ambient = vec3(1.0, 1.0, 1.0);
			whiteAmbient.Diffuse = vec3(0.0, 0.0, 0.0);
			whiteAmbient.Specular = vec3(0.0, 0.0, 0.0);
			whiteAmbient.shine = 0.01;
		materials["shiny_blue_plastic"]  = make_shared<Material>(shinyBluePlastic);
		materials["shiny_white_plastic"]  = make_shared<Material>(shinyWhitePlastic);
		materials["flat_grey"] = make_shared<Material>(flatGrey);
		materials["flat_gray"] = make_shared<Material>(flatGrey);
		materials["dark_gray"] = make_shared<Material>(darkGrey);
		materials["dark_grey"] = make_shared<Material>(darkGrey);
		materials["shiny_gray"] = make_shared<Material>(shinyGrey);
		materials["shiny_gray"] = make_shared<Material>(shinyGrey);
		materials["brass"] = make_shared<Material>(brass);
		materials["grass"] = make_shared<Material>(grass);
		materials["tree"] = make_shared<Material>(tree);
		materials["fur"] = make_shared<Material>(fur);
		materials["shadow"] = make_shared<Material>(shadow);
		materials["straw"] = make_shared<Material>(straw);
		materials["clothing"] = make_shared<Material>(clothing);
		materials["tan_skin"] = make_shared<Material>(tanSkin);
		materials["virus"] = make_shared<Material>(virus);
		materials["house_siding"] = make_shared<Material>(houseSiding);
		materials["house_roof"] = make_shared<Material>(houseRoof);
		materials["tower"] = make_shared<Material>(towerMaterial);
		materials["redMat"] = make_shared<Material>(redMat);
		materials["floor"] = make_shared<Material>(floor);
		materials["white_ambient"] = make_shared<Material>(whiteAmbient);
	}


	void initGeom(const std::string& resourceDirectory)
	{
		importModelsFrom((resourceDirectory + "/models"), "");
	}



	

	void render() {
		// Get current frame buffer size.
        int width, height;
		RenderDepthMap(activeScene->getEntitiesWithComponents({COMPONENT_LOCATION, COMPONENT_MODEL, COMPONENT_RENDERABLE}), activeScene, map_size);
		
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		if (width == 0 || height == 0) {
			return;
		}
		glViewport(0, 0, width, height);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = width/(float)height;
		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();
        Projection->pushMatrix();
            Projection->perspective(45.0f, aspect, 0.01f, 10000.0f);
        View->pushMatrix();
            View->loadIdentity();
            View->lookAt(activeScene->camera->eye, activeScene->camera->lookAt, activeScene->camera->up);
		if (activeScene->skybox != nullptr) {
			skyboxProg->bind();
				auto ident = make_shared<MatrixStack>();
					ident->loadIdentity();
					ident->scale(1000);
				//set the projection matrix - can use the same one
				glUniformMatrix4fv(skyboxProg->getUniform("P"), 1, GL_FALSE,value_ptr(Projection->topMatrix()));
				//set the depth function to always draw the box!
				glDepthFunc(GL_LEQUAL); //set up view matrix to include your view transforms  
				//(your code likely will be different depending 
				glUniformMatrix4fv(skyboxProg->getUniform("V"), 1,GL_FALSE,value_ptr(View->topMatrix()) ); //set and send model transforms - likely want a bigger cube
				glUniformMatrix4fv(skyboxProg->getUniform("M"), 1,GL_FALSE,value_ptr(ident->topMatrix())); //bind the cube map texture 
				glActiveTexture(GL_TEXTURE0 + activeScene->skybox->unit);
				glBindTexture(GL_TEXTURE_CUBE_MAP, activeScene->skybox->textureID); //draw the actual cube 
				glUniform1i(skyboxProg->getUniform("skybox"),activeScene->skybox->unit);
				vec4 skyColor = vec4(1.0, 1.0f, 1.0f, 1);
				glUniform4f(skyboxProg->getUniform("lightColor"), skyColor.r, skyColor.g, skyColor.b, 1);
				for (auto s : *(meshes["cube.obj"])) {
					s.draw(skyboxProg);
				}
				//set the depth test back to normal! 
				glDepthFunc(GL_LESS);
			skyboxProg->unbind();
		}
		renderParticles(activeScene, width, height);
		RenderScene(prog, activeScene, width, height);
		glViewport(0, 0, width, height);
		if (activeScene->updateGUI != nullptr) {
			activeScene->updateGUI(activeScene, width, height);	
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
	application->initMaterials(resourceDir);
	application->initGeom(resourceDir);
	initAudio(resourceDir);
	application->initScene();
	initShadow(); // OpenGL calls must be on main thread
	setupParticleSystem(); // OpenGL calls must be on main thread

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Update scene.
		if (activeScene != nullptr && activeScene->update != nullptr) {
			activeScene->update(activeScene, windowManager);
		}
		// Update location for 3d sound
		if (activeScene != nullptr && activeScene->camera != nullptr) {
			irrklang::vec3df position(activeScene->camera->eye.x, activeScene->camera->eye.y, activeScene->camera->eye.z);        // position of the listener
			irrklang::vec3df lookDirection(activeScene->camera->lookAt.x, activeScene->camera->lookAt.y, activeScene->camera->lookAt.z); // the direction the listener looks into
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
