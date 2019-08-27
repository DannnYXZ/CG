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

class HexagonAnimation {
public:
    static const int MAX_USED_TILES = 1000;
    uint tileVBO, tileVAO, tileEBO;
    uint uModel;
    vec3 vertices[6];
    uint order[6] = {0, 5, 1, 4, 2, 3};
    float R = 0.3;
    float T = 3.0f; // seconds
    int DIV = 6;
    int pieces_drawn = 0;
    double start_time, local_time;
    mat4 &view;
    mat4 &proj;
    char **used;

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

        used = new char *[MAX_USED_TILES];
        for (int i = 0; i < MAX_USED_TILES; i++) {
            used[i] = new char[MAX_USED_TILES];
            fill(used[i], used[i] + MAX_USED_TILES, 0);
        }
    }

    void drawTile(mat4 &model) {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, value_ptr(model));
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
        pieces_drawn++;
    }

    struct tile_info {
        int virt_i;
        int virt_j;
        int depth;
        mat4 precalc_model;
        vec3 start_pos;
    };

    void bfs_draw() {
        queue<tile_info> q;
        q.push(tile_info{0, 0, 0, mat4(1), vec3(0, 0, 0)});
        used[0][0] = 1;
        while (!q.empty()) {
            tile_info tile = q.front();
            q.pop();
            // drawing cur_tile
            float time_depth = local_time / T;
            if (time_depth < tile.depth)
                continue;
            if (time_depth <= tile.depth + 1) {
                drawTile(tile.precalc_model);
                continue;
            }
            mat4 trans = translate(mat4(1), tile.start_pos);
            drawTile(trans);
            // calc child start positions and models
            double intpart;
            float child_time = modf(time_depth, &intpart);
            vec3 rot_shift = vec3(0, 0, R * sqrt(3) / 2);
            quat rot = angleAxis(-child_time * pi<float>(), vec3(1, 0, 0));
            mat4 new_model = translate(mat4(1), -rot_shift) * toMat4(rot) * translate(mat4(1), rot_shift);
            rot_shift *= -2;
            quat y_q_rot = angleAxis(radians(360.f / DIV), vec3(0, 1, 0));
            mat4 y_m_rot = toMat4(y_q_rot);

            int di[] = {-2, -1, 1, 2, 1, -1};
            int dj[] = {0, -1, -1, 0, 1, 1};
            for (int i = 0; i < 6; i ++) {
                mat4 child_model = translate(mat4(1), tile.start_pos) * new_model;
                vec3 child_start_pos = tile.start_pos + rot_shift;
                int child_i = tile.virt_i + di[i];
                int child_j = tile.virt_j + dj[i];
                int wrap_i = mmod(child_i, MAX_USED_TILES);
                int wrap_j = mmod(child_j, MAX_USED_TILES);
                if (!used[wrap_i][wrap_j]) {
                    used[wrap_i][wrap_j] = true;
                    q.push(tile_info{child_i, child_j, tile.depth + 1, child_model, child_start_pos});
                }
                new_model = y_m_rot * new_model;
                rot_shift = y_q_rot * rot_shift;
            }
        }
    }

    // will be called every render
    void draw(Shader &shader) {
        local_time = glfwGetTime() - start_time;
        pieces_drawn = 0;
        shader.use();
        uModel = glGetUniformLocation(shader.ID, "model");
        glBindVertexArray(tileVAO);
        bfs_draw();
        for (int i = 0; i < MAX_USED_TILES; i++)
            fill(used[i], used[i] + MAX_USED_TILES, 0);
    }

    void reset() {
        start_time = glfwGetTime();
    }
};

#endif //CG_HEXAGONS_H
