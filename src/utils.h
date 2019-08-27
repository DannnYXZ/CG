#ifndef CG_UTILS_H
#define CG_UTILS_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

using namespace glm;

void print(glm::mat4 mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            std::cout << mat[i][j] << ' ';
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print(glm::vec3 vec) {
    for (int i = 0; i < 3; i++)
        std::cout << vec[i] << ' ';
    std::cout << std::endl;
}

void print(glm::quat q) {
    for (int i = 0; i < 4; i++)
        std::cout << q[i] << ' ';
    std::cout << std::endl;
}

quat fromtwovectors(vec3 u, vec3 v) {
    vec3 w = cross(u, v);
    quat q = quat(1.f + dot(u, v), w.x, w.y, w.z);
    return normalize(q);
}

inline int mmod(int a, int b) {
    return (b + (a % b)) % b;
}

#endif //CG_UTILS_H
