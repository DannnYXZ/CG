#ifndef CG_LOOK_AT_CAMERA_H
#define CG_LOOK_AT_CAMERA_H

#include <glm/glm.hpp>

using namespace glm;

class LookAtCamera {
public:
    LookAtCamera(vec3 pos, vec3 target, vec3 up) : pos(pos), target(target), worldUp(up) {}

    mat4 view() {
        vec3 zaxis = normalize(pos - target);
        vec3 xaxis = normalize(cross(worldUp, zaxis));
        vec3 yaxis = cross(zaxis, xaxis);

        mat4 inv_rot = {
                vec4(xaxis.x, yaxis.x, zaxis.x, 0),
                vec4(xaxis.y, yaxis.y, zaxis.y, 0),
                vec4(xaxis.z, yaxis.z, zaxis.z, 0),
                vec4(0, 0, 0, 1)
        };

        mat4 inv_trans = {
                vec4(1, 0, 0, 0),
                vec4(0, 1, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(-pos.x, -pos.y, -pos.z, 1)
        };
        return inv_rot * inv_trans;
    };

    void translate(vec3 v) {
        this->pos += v;
    }

    void change_target(vec3 target) {
        this->target = target;
    }

private:
    vec3 pos;
    vec3 target;
    vec3 worldUp;
};

#endif //CG_LOOK_AT_CAMERA_H
