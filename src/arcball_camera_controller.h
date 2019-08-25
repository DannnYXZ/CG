#ifndef CG_ARCBALL_CAMERA_CONTROLLER_H
#define CG_ARCBALL_CAMERA_CONTROLLER_H

#include "camera/arcball_camera.h"
#include "utils.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

using namespace std;

class ArcballCamController {
    float EPS = 1e-6;
    float MOUSE_SENS = 3.f;
private:
    ArcballCamera *camera;
    quat saved_rot;
    int &scr_width, &scr_height;
    vec2 last_mv{0, 0};
    bool arcball_on = false;

    vec3 get_arcball_vec(float x, float y) {
        vec3 P = vec3(2 * x / scr_width - 1, 2 * y / scr_height - 1, 0);
        P.y = -P.y;
        float z_square = dot(P, P);
        if (z_square <= 1)
            P.z = sqrt(1 - z_square);
        else
            P = normalize(P);
        return P;
    }

public:
    ArcballCamController(ArcballCamera *camera, int &scr_width, int &scr_height) : scr_height(scr_height),
                                                                                   scr_width(scr_width) {
        this->camera = camera;
        this->saved_rot = camera->orient;
    }

    void mouseMove(double xpos, double ypos) {
        if (!arcball_on)
            return;
        vec3 p0 = get_arcball_vec(last_mv.x, last_mv.y);
        vec3 p1 = get_arcball_vec(xpos, ypos);
        if (abs(1.f - dot(p0, p1)) <= EPS)
            return;
        vec3 axis = cross(p0, p1);
        float ang = acos(clamp<float>(dot(p0, p1), -1., 1.)) * MOUSE_SENS;
        axis = saved_rot * axis; // respect camera orientation
        quat rot = angleAxis(ang, normalize(axis));
        camera->orient = conjugate(rot) * saved_rot;
    }

    void mouseButton(int button, int action, int mods, double x, double y) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            arcball_on = true;
            saved_rot = quat(camera->orient);
            last_mv = vec2(x, y);
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            arcball_on = false;
        }
    }

    void mouseScroll(double xoffset, double yoffset) {
        vec3 shift = normalize(camera->dist) * (float) yoffset;
        vec3 new_pos = camera->dist - shift;
        if (dot(new_pos, new_pos) >= 0.1)
            camera->dist = new_pos;
    }

    void processKey(int key) {

    }
};

#endif //CG_ARCBALL_CAMERA_CONTROLLER_H
