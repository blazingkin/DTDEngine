#include "SystemRender.h"

using namespace std;
using namespace glm;
extern float map_size;
shared_ptr<Program> active_program = nullptr;
vec3 currentCameraPosition;
GLuint mainFBO;
GLuint mainFBOTex;
// Our shader program
std::shared_ptr<Program> prog, noShadowProg;
std::shared_ptr<Program> skyboxProg;

vector<vec4> planes;
void makeViewFrust(glm::mat4 P, glm::mat4 V){
    vec4 Left, Right, Bottom, Top, Near, Far;
    
    // vec4 planes[6];
    /* composite matrix */
    mat4 comp = P*V;
    vec3 n; //use to pull out normal
    float l; //length of normal for plane normalization

    Left.x = comp[0][3] + comp[0][0]; 
    Left.y = comp[1][3] + comp[1][0]; 
    Left.z = comp[2][3] + comp[2][0]; 
    Left.w = comp[3][3] + comp[3][0];

    n = vec3(Left.x, Left.y, Left.z);
    l = length(n);
    Left /= l;
    planes.push_back(Left);

    Right.x = comp[0][3] - comp[0][0];
    Right.y = comp[1][3] - comp[1][0];
    Right.z = comp[2][3] - comp[2][0]; 
    Right.w = comp[3][3] - comp[3][0];
    
    n = vec3(Right.x, Right.y, Right.z);
    l = length(n);
    Right /= l;
    planes.push_back(Right);

    Bottom.x = comp[0][3] + comp[0][1]; 
    Bottom.y = comp[1][3] + comp[1][1]; 
    Bottom.z = comp[2][3] + comp[2][1]; 
    Bottom.w = comp[3][3] + comp[3][1];
    
    n = vec3(Bottom.x, Bottom.y, Bottom.z);
    l = length(n);
    Bottom /= l;
    planes.push_back(Bottom);

    Top.x = comp[0][3] - comp[0][1]; 
    Top.y = comp[1][3] - comp[1][1]; 
    Top.z = comp[2][3] - comp[2][1]; 
    Top.w = comp[3][3] - comp[3][1];
    
    n = vec3(Top.x, Top.y, Top.z);
    l = length(n);
    Top /= l;
    planes.push_back(Top);

    Near.x = comp[0][2]; 
    Near.y = comp[1][2]; 
    Near.z = comp[2][2]; 
    Near.w = comp[3][2];
    
    n = vec3(Near.x, Near.y, Near.z);
    l = length(n);
    Near /= l;
    planes.push_back(Near);

    Far.x = comp[0][3] - comp[0][2]; 
    Far.y = comp[1][3] - comp[1][2]; 
    Far.z = comp[2][3] - comp[2][2]; 
    Far.w = comp[3][3] - comp[3][2];
    
    n = vec3(Far.x, Far.y, Far.z);
    l = length(n);
    Far /= l;   
    planes.push_back(Far);
}

float DistToPlane(float A, float B, float C, float D, vec3 point) {
  return (A * point.x + B * point.y + C * point.z + D)/sqrt(pow(A,2) +pow(B,2) + pow(C,2));
}

float calculateRadius(c_model_t *model_comp, c_location_t *location_comp) {
    float upperX = model_comp->upperBound.x * location_comp->size.x;
    float upperY = model_comp->upperBound.y * location_comp->size.y;
    float upperZ = model_comp->upperBound.z * location_comp->size.z;
    float lowerX = model_comp->lowerBound.x * location_comp->size.x;
    float lowerY = model_comp->lowerBound.y * location_comp->size.y;
    float lowerZ = model_comp->lowerBound.z * location_comp->size.z;
    float upper = (upperX * upperX) + (upperY * upperY) + (upperZ * upperZ);
    float lower = (lowerX * lowerX) + (lowerY * lowerY) + (lowerZ * lowerZ);
    return upper > lower ? sqrt(upper) : sqrt(lower);
}

bool ViewFrustCull(vec3 center, float radius) {
    float dist;
    for (int i=0; i < 6; i++) {
      dist = DistToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, center);
      //test against each plane
      if(dist < (-1)*radius)
        return true;
    }
    return false;
}

void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
}
 
void printVec3(vec3 v) {
    cout << v.x << ", " << v.y << ", " << v.z << endl;
}
void renderChildren(shared_ptr<Program> program, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
 shared_ptr<MatrixStack> Model, BScene *scene, vector<shared_ptr<RenderableWithPosition>> object);

void renderGeneric(shared_ptr<Program> defaultProg, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
 shared_ptr<MatrixStack> Model, BScene *scene,c_render_t renderComponent, c_location_t positionComponent, c_model_t modelComponent) {
    Model->pushMatrix();
            Model->translate(positionComponent.position);
            if (positionComponent.rotationAmount.x != 0) { Model->rotate(positionComponent.rotationAmount.x, vec3(1,0,0)); }
            if (positionComponent.rotationAmount.y != 0) { Model->rotate(positionComponent.rotationAmount.y, vec3(0,1,0)); }
            if (positionComponent.rotationAmount.z != 0) { Model->rotate(positionComponent.rotationAmount.z, vec3(0,0,1)); }
            auto program = renderComponent.program;
            if (nullptr == program) {
                program = defaultProg;
            }
            renderChildren(program, Projection, View, Model, scene, renderComponent.children);
            if (active_program != program) {
                if (active_program != nullptr) {
                    active_program->unbind();
                }
                program->bind();
                active_program = program;
                if (program->hasUniform("lightPos")) {
                    glUniform3f(program->getUniform("lightPos"), scene->lightLocation.x, scene->lightLocation.y, scene->lightLocation.z);
                }
                if (program->hasUniform("lightColor")) {
                    glUniform3f(program->getUniform("lightColor"), scene->lightColor.r, scene->lightColor.g, scene->lightColor.b);
                }
                if (program->hasUniform("viewVector")) {
                    glUniform3f(program->getUniform("viewVector"), currentCameraPosition.x, currentCameraPosition.y, currentCameraPosition.z);
                }
                if (program->hasUniform("P")) {
                    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
                }
                if (program->hasUniform("V")) {
                    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
                }
                if (program->hasUniform("LS")) {
                    auto LP = ortho((float)-map_size * 1.7f,(float) map_size * 1.7f,(float) -map_size * 1.7f, (float)map_size * 1.7f, 0.1f, 1.7f * (float) map_size);
                    auto LV = lookAt(scene->lightLocation, vec3(0,0,0), vec3(0, 1, 0));
                    glUniformMatrix4fv(program->getUniform("LS"), 1, GL_FALSE, value_ptr(LP * LV));
                }
                if (program->hasUniform("shadowDepth")) {
                    glActiveTexture(GL_TEXTURE1); //also this may be different since we have more than one texture
                    glBindTexture(GL_TEXTURE_2D, depthMap);
                    glUniform1i(program->getUniform("shadowDepth"), 1);
                }
                
            }
            if (program->hasUniform("bmLight")) {
                glUniform3f(program->getUniform("bmLight"), -85.0, 80.0, -15.0);
            }
        
            Model->scale(positionComponent.size);
            // set material
            if (renderComponent.material) {
                program->setMaterial(renderComponent.material);
            } else {
                program->setMaterial(materials["flat_grey"]);
            }
            if (renderComponent.tex != nullptr) {
                program->textureEnabled(true);
                program->setTexture(renderComponent.tex);
            } else {
                program->textureEnabled(false);
            }
            if (renderComponent.normalMap != nullptr) {
                program->setNormalMap(renderComponent.normalMap);
            } else {
                program->setNormalMap(textures["nmap_none.png"]);
            }
            if (renderComponent.bump != nullptr) {
                program->setBumpMap(renderComponent.bump);
            } 
            for (auto it = renderComponent.shaderUniforms.begin(); it != renderComponent.shaderUniforms.end(); it++) {
                glUniform4f(program->getUniform(it->first), it->second.x, it->second.y, it->second.z, it->second.w);
            }
            setModel(program, Model);
            for (auto shape : *modelComponent.model) {
                shape.draw(program);
            }
        Model->popMatrix();
}

void renderChildren(shared_ptr<Program> program,shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
 shared_ptr<MatrixStack> Model, BScene *scene, vector<shared_ptr<RenderableWithPosition>> object) {
    for (shared_ptr<RenderableWithPosition> obj : object) {
        renderGeneric(program, Projection, View, Model, scene, *obj->renderable, obj->relativeLocation, *obj->model);
    }
}

void renderEntities(shared_ptr<Program> program,shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
                    shared_ptr<MatrixStack> Model, BScene *scene, vector<shared_ptr<BLZEntity>> object, bool cull) {

    // 1. Loop through every entity
        // 2a. Cull
        // 2b. "Sort" using a hashmap based on what model type it's using
    // 3. Instance render specific models
    // 4. Render everything else
    
    std::map<pair<model_t, shared_ptr<Program>>, std::vector<shared_ptr<BLZEntity>>> modelMap; 

    for (shared_ptr<BLZEntity> obj : object) {
        auto positionComponent = obj->getComponent<c_location_t>();
        auto modelComponent = obj->getComponent<c_model_t>();
        auto renderComponent = obj->getComponent<c_render_t>();
        auto prog = renderComponent->program;
        if (prog == nullptr) {
            prog = program;
        }
        
        if(cull && ViewFrustCull(positionComponent->position, modelComponent->radius)) {
            continue;
        }
        modelMap[std::pair<model_t, shared_ptr<Program>>(modelComponent->model, prog)].push_back(obj);
    }

    //GL_MAX_VERTEX_UNIFORM_COMPONENTS gives max number of floats
    map<pair<model_t, shared_ptr<Program>>, instance_render_data_t> instanceRenderData;
    map<pair<model_t, shared_ptr<Program>>, thread> threads;
    // Set up instance render data
    for (auto it = modelMap.begin(); it != modelMap.end(); it++) {
        auto key = it->first;
        auto program = key.second;
        if (program->supportsInstanceRendering) {
            // Setup data for instance rendering
            threads[key] = thread(setupInstanceRenderData, &modelMap[key], &instanceRenderData[key]);
        }
    }
    
    // Render instance renderables
    // Render others
    for (auto it = modelMap.begin(); it != modelMap.end(); it++) {
        auto key = it->first;
        auto program = key.second;
        if (program->supportsInstanceRendering) {
            if (threads[key].joinable()) {
                threads[key].join();
            }            
            auto data = instanceRenderData[key];
            auto entity = it->second[0];
            RenderInstances(&data, program, entity, Projection, View, scene);
        } else {
            auto entities = it->second;
            for (auto entityIterator = entities.begin(); entityIterator != entities.end(); entityIterator++) {
                auto entity = entityIterator->get();
                auto renderComponent = entity->getComponent<c_render_t>();
                auto positionComponent = entity->getComponent<c_location_t>();
                auto modelComponent = entity->getComponent<c_model_t>();
                renderGeneric(program, Projection, View, Model, scene, *renderComponent, *positionComponent, *modelComponent);
            }
        }
    }
    if (active_program != nullptr) {
        active_program->unbind();
        active_program = nullptr;
    }
}

void initSystemRender() {


		glGenFramebuffers(1, &mainFBO);

		//generate the texture
		glGenTextures(1, &mainFBOTex);
		glBindTexture(GL_TEXTURE_2D, mainFBOTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//bind with framebuffer's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainFBOTex, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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


GLuint RenderScene(BScene *scene, int width, int height) {

    //Use the matrix stack for Lab 6
    float aspect = width/(float)height;
    // Create the matrix stacks - please leave these alone for now
    auto Projection = make_shared<MatrixStack>();
    auto View = make_shared<MatrixStack>();
    auto Model = make_shared<MatrixStack>();
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glViewport(0, 0, 1024, 1024);
    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Projection->perspective(45.0f, aspect, 0.01f, 10000.0f);
    View->loadIdentity();
    View->lookAt(scene->camera->eye, scene->camera->lookAt, scene->camera->up);
    if (scene->skybox != nullptr) {
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
				glActiveTexture(GL_TEXTURE0 + scene->skybox->unit);
				glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox->textureID); //draw the actual cube 
				glUniform1i(skyboxProg->getUniform("skybox"),scene->skybox->unit);
				vec4 skyColor = vec4(1.0, 1.0f, 1.0f, 1);
				glUniform4f(skyboxProg->getUniform("lightColor"), skyColor.r, skyColor.g, skyColor.b, 1);
				for (auto s : *(meshes["cube.obj"])) {
					s.draw(skyboxProg);
				}
				//set the depth test back to normal! 
				glDepthFunc(GL_LESS);
			skyboxProg->unbind();
		}
    renderParticles(scene, width, height);
    Projection->pushMatrix();
        Projection->perspective(45.0f, aspect, 0.01f, 750.0f);
    View->pushMatrix();
        View->loadIdentity();
        View->lookAt(scene->camera->eye, scene->camera->lookAt, scene->camera->up);
    currentCameraPosition = scene->camera->eye;
    active_program = nullptr;
    planes.clear();
    makeViewFrust(Projection->topMatrix(), View->topMatrix());

    Model->pushMatrix();
        Model->loadIdentity();
        renderEntities(prog, Projection, View, Model, scene, scene->getEntitiesWithComponents({COMPONENT_LOCATION, COMPONENT_MODEL, COMPONENT_RENDERABLE}), true);
    Model->popMatrix();
    Projection->popMatrix();
    View->popMatrix();

    return mainFBOTex;
}


void setupInstanceRenderData(vector<shared_ptr<BLZEntity>> *entities, instance_render_data_t *result) {
    for (auto it = entities->begin(); it != entities->end(); it++) {
        auto entity = it->get();
        auto renderableComponent = entity->getComponent<c_render_t>();
        auto positionComponent = *entity->getComponent<c_location_t>();
        mat4 m = scale(positionComponent.size);
        if (positionComponent.rotationAmount.x != 0) { m = rotate(positionComponent.rotationAmount.x, vec3(1, 0, 0)) * m; }
        if (positionComponent.rotationAmount.y != 0) { m = rotate(positionComponent.rotationAmount.y, vec3(0, 1, 0)) * m; }
        if (positionComponent.rotationAmount.z != 0) { m = rotate(positionComponent.rotationAmount.z, vec3(0, 0, 1)) * m; }
        m = translate(positionComponent.position) * m;
        result->M.push_back(m);
        for (auto customUniformIterator = renderableComponent->shaderUniforms.begin(); customUniformIterator != renderableComponent->shaderUniforms.end(); customUniformIterator++) {
            if (result->attributes[customUniformIterator->first] == nullptr) {
                result->attributes[customUniformIterator->first] = make_shared<vector<vec4>>();
            }
            result->attributes[customUniformIterator->first]->push_back(customUniformIterator->second);
        }
    }
}


void RenderInstances(instance_render_data_t *data, shared_ptr<Program> program, shared_ptr<BLZEntity> entity, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View, BScene *scene) {
    auto modelComponent = *entity->getComponent<c_model_t>();
    auto renderComponent = *entity->getComponent<c_render_t>();
    if (active_program != program) {
        if (active_program != nullptr) {
            active_program->unbind();
        }
        program->bind();
        active_program = program;
        if (program->hasUniform("lightPos")) {
            glUniform3f(program->getUniform("lightPos"), scene->lightLocation.x, scene->lightLocation.y, scene->lightLocation.z);
        }
        if (program->hasUniform("lightColor")) {
            glUniform3f(program->getUniform("lightColor"), scene->lightColor.r, scene->lightColor.g, scene->lightColor.b);
        }
        if (program->hasUniform("viewVector")) {
            glUniform3f(program->getUniform("viewVector"), currentCameraPosition.x, currentCameraPosition.y, currentCameraPosition.z);
        }
        if (program->hasUniform("P")) {
            glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
        }
        if (program->hasUniform("V")) {
            glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        }
        if (program->hasUniform("LS")) {
            auto LP = ortho((float)-map_size * 1.7f,(float) map_size * 1.7f,(float) -map_size * 1.7f, (float)map_size * 1.7f, 0.1f, 1.7f * (float) map_size);
            auto LV = lookAt(scene->lightLocation, vec3(0,0,0), vec3(0, 1, 0));
            glUniformMatrix4fv(program->getUniform("LS"), 1, GL_FALSE, value_ptr(LP * LV));
        }
        if (program->hasUniform("shadowDepth")) {
            glActiveTexture(GL_TEXTURE1); //also this may be different since we have more than one texture
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(program->getUniform("shadowDepth"), 1);
        }
    }
    // set material
    if (renderComponent.material) {
        program->setMaterial(renderComponent.material);
    } else {
        program->setMaterial(materials["flat_grey"]);
    }
    if (renderComponent.tex != nullptr) {
        program->textureEnabled(true);
        program->setTexture(renderComponent.tex);
    } else {
        program->textureEnabled(false);
    }
    if (renderComponent.normalMap != nullptr) {
        program->setNormalMap(renderComponent.normalMap);
    } else {
        program->setNormalMap(textures["nmap_none.png"]);
    }

    for (auto shape : *modelComponent.model) {
        shape.drawInstances(program, data->M.size(), data);
    }
}
