#include "FreeCameraInput.h"

void freeCameraHandleKey(GLFWwindow *window, int key, int scancode, int action, int mods, BScene *scene) {
    if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        scene->camera->eye.y -= 0.2;
        scene->camera->lookAt.y -= 0.2;
    } 
    if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        scene->camera->eye.y +=0.2;
        scene->camera->lookAt.y += 0.2;
    }
    if (key == GLFW_KEY_P) {
        scene->singleton()->getComponent<c_video_settings_t>()->fovX += 0.01f;
    }
    if (key == GLFW_KEY_O) {
        scene->singleton()->getComponent<c_video_settings_t>()->fovX -= 0.01f;
    }
  /*  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
        if (action == GLFW_PRESS){
            playerMotion.y = 1;
        } else {
            playerMotion.y = 0;
        }
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_RELEASE) ) {
        if (action == GLFW_PRESS) {
            playerMotion.y = -1;
        } else {
            playerMotion.y = 0;
        }
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
        if (action == GLFW_PRESS) {
            playerMotion.x = 1;
        } else {
            playerMotion.x = 0;
        }
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
        if (action == GLFW_PRESS) {
            playerMotion.x = -1;					
        } else {
            playerMotion.x = 0;
        }	
    }*/
}

double mouseDownX, mouseDownY;
bool mouseIsDown = false;
float hcameraSpeed = 5.5;
float vcameraSpeed = 3.0f;
float vTheta = -M_PI / 2;
float vPhi = 0;

void updateGameCameraLook(BScene *scene, double timeDelta, GLFWwindow *window){
    if (mouseIsDown) {
        int w, h;
        double xpos, ypos;
        glfwGetFramebufferSize(window, &w, &h);
        if (w == 0 || h == 0) {
            return;
        }
        glfwGetCursorPos(window, &xpos, &ypos);
        float dX = hcameraSpeed * (mouseDownX - xpos) * timeDelta / (float) w;
        float dY = vcameraSpeed * (mouseDownY - ypos) * timeDelta / (float) h;
        vTheta -= dX;
        vPhi += dY;
        glm::vec3 lookDisplacement = glm::vec3(cos(vTheta) * cos(vPhi), sin(vPhi), cos(vPhi) * cos((M_PI / 2) - vTheta));
        scene->camera->lookAt = scene->camera->eye + lookDisplacement;
    }
}

void freeCameraHandleClick(GLFWwindow *window, int button, int action, int mods, BScene *scene) {
    if (GLFW_PRESS == action && GLFW_MOUSE_BUTTON_LEFT == button) {
        glfwGetCursorPos(window, &mouseDownX, &mouseDownY);
        mouseIsDown = true;
    } else if (GLFW_RELEASE == action && GLFW_MOUSE_BUTTON_LEFT == button) {
        mouseIsDown = false;
    }

}