#ifndef CG_ARCBALL_CAMERA_H
#define CG_ARCBALL_CAMERA_H

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

using namespace glm;

class ArcballCamera {
public:
    ArcballCamera(const quat &orient, vec3 target, vec3 dist) : orient(orient),
                                                                dist(dist),
                                                                target(target) {}

    quat orient{0, 0, 0, 1};
    vec3 dist;
    vec3 target;

    mat4 view() {
        mat4 displacement = translate(mat4(1), dist);
        mat4 trans = translate(mat4(1), target);
        mat4 rot = toMat4(orient);
        mat4 model = trans * rot * displacement;
        return inverse(model);
    }
};

#endif //CG_ARCBALL_CAMERA_H
