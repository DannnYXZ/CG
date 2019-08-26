#ifndef CG_HEXAGONS_H
#define CG_HEXAGONS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <zconf.h>
#include <glm/detail/type_quat.hpp>
#include <bits/stdc++.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace glm;
using namespace std;

struct Tile {
    vec3 pos;
    quat orient;
};

/* animation state must depend on time

*/

class HexagonAnimation {
private:
    uint tileVBO, tileVAO, tileEBO;
    vec3 vertices[6];
    uint order[6] = {0, 5, 1, 4, 2, 3};
    float R = 1;
    double start_time, local_time;
    uint uView, uProj, uModel;
    mat4 &view;
    mat4 &proj;
    Shader *shader;
public:
    HexagonAnimation(mat4 &view, mat4 &proj) : view(view), proj(proj) {
        glGenBuffers(1, &tileVBO);
        glGenBuffers(1, &tileEBO);
        glGenVertexArrays(1, &tileVAO);
        glBindVertexArray(tileVAO);
        glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
        for (int i = 0; i < 6; i++) {
            float ang = radians(i * 60.f);
            vertices[i].x = cos(ang) * R;
            vertices[i].z = sin(ang) * R;
            vertices[i].y = 0;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(order), order, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        shader = new Shader("shaders/hex.vs", "shaders/hex.fs");
        uView = glGetUniformLocation(shader->ID, "view");
        uProj = glGetUniformLocation(shader->ID, "proj");
        uModel = glGetUniformLocation(shader->ID, "model");
    }

    float T = 3.0f; // seconds

    void drawTile(mat4 &model) {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, value_ptr(model));
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
    }

    void draw(vec3 &start_pos, mat4 &orient_n_pos, int depth) {
        // if called then start children transforms
        float time_depth = local_time / T;
        if (time_depth < depth)
            return;
        if (time_depth <= depth + 1) {
            drawTile(orient_n_pos);
            return;
        }
        mat4 trans = translate(mat4(1), start_pos);
        drawTile(trans);
        double intpart;
        float child_time = modf(time_depth, &intpart);
        vec3 rot_shift = vec3(0, 0, R * sqrt(3) / 2);
        quat rot = angleAxis(-child_time * pi<float>(), vec3(1, 0, 0));
        mat4 new_model = translate(mat4(1), -rot_shift) * toMat4(rot) * translate(mat4(1), rot_shift);
        rot_shift *= -2;
        quat y_q_rot = angleAxis(radians(120.f), vec3(0, 1, 0));
        mat4 y_m_rot = toMat4(y_q_rot);
        vec3 child_start_pos = start_pos + rot_shift;
        new_model = translate(mat4(1), start_pos) * new_model;
        draw(child_start_pos, new_model, depth + 1);
        new_model = y_m_rot * new_model;
        child_start_pos = start_pos + y_q_rot * rot_shift;
        draw(child_start_pos, new_model, depth + 1);
        new_model = y_m_rot * new_model;
        child_start_pos = start_pos + y_q_rot * y_q_rot * rot_shift;
        draw(child_start_pos, new_model, depth + 1);
    }

    // will be called every time
    void draw() {
        local_time = glfwGetTime() - start_time;
        shader->use();
        glUniformMatrix4fv(uView, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, value_ptr(proj));
        glBindVertexArray(tileVAO);
        mat4 model(1);
        vec3 origin(0, 0, 0);
        draw(origin, model, 0);
    }

    void reset() {
        double start_time = glfwGetTime();
    }
};

#endif //CG_HEXAGONS_H
