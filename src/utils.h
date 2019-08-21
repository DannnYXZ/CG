#ifndef CG_UTILS_H
#define CG_UTILS_H

#include <iostream>
#include <glm/glm.hpp>

void print(glm::mat4 mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            std::cout << mat[i][j] << ' ';
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

#endif //CG_UTILS_H
