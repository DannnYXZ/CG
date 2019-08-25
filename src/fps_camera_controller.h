#ifndef CG_FPS_CAMERA_CONTROLLER_H
#define CG_FPS_CAMERA_CONTROLLER_H

#include <GLFW/glfw3.h>
#include "camera/fps_camera.h"

class FPSCameraController {
private:
    FPSCamera *camera;
public:
    float MOVE_SPEED = 10.0f;
    float ROT_SPEED = 1.5f;
    float MOUSE_SENSITIVITY = 0.005f;

    FPSCameraController(FPSCamera *camera) {
        this->camera = camera;
    }

    void processKey(int key, float deltatime) {
        if (key == GLFW_KEY_W)
            camera->translate(-camera->zaxis * MOVE_SPEED * deltatime);
        if (key == GLFW_KEY_A)
            camera->translate(-camera->xaxis * MOVE_SPEED * deltatime);
        if (key == GLFW_KEY_S)
            camera->translate(camera->zaxis * MOVE_SPEED * deltatime);
        if (key == GLFW_KEY_D)
            camera->translate(camera->xaxis * MOVE_SPEED * deltatime);
        if (key == GLFW_KEY_SPACE)
            camera->translate(camera->yaxis * MOVE_SPEED * deltatime);
        if (key == GLFW_KEY_LEFT_SHIFT)
            camera->translate(-camera->yaxis * MOVE_SPEED * deltatime);
    }

    void process_mouse(int dx, int dy) {
        camera->yaw(-dx * MOUSE_SENSITIVITY * ROT_SPEED);
        camera->pitch(dy * MOUSE_SENSITIVITY * ROT_SPEED);
    }
};

#endif //CG_FPS_CAMERA_CONTROLLER_H
