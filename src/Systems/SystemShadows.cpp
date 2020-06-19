#include "SystemShadows.h"
GLuint depthMapFBO;
const GLuint S_WIDTH = 2048 * 4, S_HEIGHT = 2048 * 4;
GLuint depthMap;
std::shared_ptr<Program> depthProg;

using namespace glm;

/* set up the FBO for storing the light's depth */
void initShadow() {
    //generate the FBO for the shadow depth
    glGenFramebuffers(1, &depthMapFBO);

    //generate the texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT, 
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const std::vector<std::string> depthShaders({"depth_vert.glsl", "depth_frag.glsl"});
    depthProg = std::make_shared<Program>();
    depthProg->setVerbose(true);
    depthProg->init(depthShaders);
    depthProg->addAttribute("vertPos");
    depthProg->addUniform("LP");
    depthProg->addUniform("LV");
    depthProg->addUniform("LM");
}


void RenderChildren(std::vector<std::shared_ptr<RenderableWithPosition>> children, std::shared_ptr<MatrixStack> M){
    for (auto it = children.begin(); it != children.end(); it++) {
        M->pushMatrix();
        auto child = it->get();
        M->translate(child->relativeLocation.position);
        if (child->relativeLocation.rotationAmount.x != 0) { M->rotate(child->relativeLocation.rotationAmount.x, vec3(1,0,0)); }
        if (child->relativeLocation.rotationAmount.y != 0) { M->rotate(child->relativeLocation.rotationAmount.y, vec3(0,1,0)); }
        if (child->relativeLocation.rotationAmount.z != 0) { M->rotate(child->relativeLocation.rotationAmount.z, vec3(0,0,1)); }
        if (!child->renderable->children.empty()) {
            RenderChildren(child->renderable->children, M);
        }
        M->scale(child->relativeLocation.size);
        glUniformMatrix4fv(depthProg->getUniform("LM"), 1, GL_FALSE, value_ptr(M->topMatrix()));

        for (auto s : *child->model->model) {
            s.draw(depthProg);
        }
        M->popMatrix();
    }
}

void RenderDepthMap(std::vector<std::shared_ptr<BLZEntity>> occludedBy, BScene *scene, float map_size) {
    // Render to the depth map FBO
    
    glViewport(0, 0, S_WIDTH, S_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  	glClear(GL_DEPTH_BUFFER_BIT);
  	glCullFace(GL_FRONT);
    depthProg->bind();
    auto perspectiveMatrix = ortho((float)-map_size * 1.7f,(float) map_size * 1.7f,(float) -map_size * 1.7f, (float)map_size * 1.7f, 0.1f, 1.7f * (float) map_size);
    glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, value_ptr(perspectiveMatrix));

    auto viewMatrix = lookAt(scene->lightLocation, vec3(0,0,0), vec3(0, 1, 0));
    glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, value_ptr(viewMatrix));
    auto M = std::make_shared<MatrixStack>();
    M->loadIdentity();
    while (!occludedBy.empty()) {
        auto entity = occludedBy.back();
        occludedBy.pop_back();
        auto modelComponent = entity->getComponent<c_model_t>();
        auto locationComponent = *entity->getComponent<c_location_t>();
        auto renderComponent = *entity->getComponent<c_render_t>();
        M->pushMatrix();
            // Render ourselves
            M->translate(locationComponent.position);
            if (locationComponent.rotationAmount.x != 0) { M->rotate(locationComponent.rotationAmount.x, vec3(1,0,0)); }
            if (locationComponent.rotationAmount.y != 0) { M->rotate(locationComponent.rotationAmount.y, vec3(0,1,0)); }
            if (locationComponent.rotationAmount.z != 0) { M->rotate(locationComponent.rotationAmount.z, vec3(0,0,1)); }
            RenderChildren(renderComponent.children, M);
            M->scale(locationComponent.size);
            glUniformMatrix4fv(depthProg->getUniform("LM"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            
            for (auto s : *modelComponent->model) {
                s.draw(depthProg);
            }

        M->popMatrix();
    }
    depthProg->unbind();



    //set culling back to normal
    glCullFace(GL_BACK);


    //this sets the output back to the screen
  	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
