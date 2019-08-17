//
// Created by noroot on 8/17/19.
//

#ifndef CG_CAMERA_H
#define CG_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Camera {
public:
    Camera(vec3 position, vec3 direction, vec3 up) {
        this->position = position;
        this->direction = direction;
        this->up = up;
    }

    mat4 getMatrix() {
        return mat4(1);
    };

private:
    vec3 up;
    vec3 position;
    vec3 direction;

};

#endif //CG_CAMERA_H
