#ifndef CG_FPS_CAMERA_H
#define CG_FPS_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using namespace glm;

class FPSCamera {
private:
    vec3 m_pos;
    float m_pitch = 0, m_yaw = 0;

    void recalc_basis() {
        float cosYaw = cos(m_yaw);
        float sinYaw = sin(m_yaw);
        float cosPitch = cos(m_pitch);
        float sinPitch = sin(m_pitch);
        xaxis = vec3(cosYaw, 0, -sinYaw);
        yaxis = vec3(sinPitch * sinYaw, cosPitch, sinPitch * cosYaw);
        zaxis = vec3(cosPitch * sinYaw, -sinPitch, cosPitch * cosYaw);
    }

public:
    vec3 xaxis{}, yaxis{}, zaxis{};

    explicit FPSCamera(vec3 pos) : m_pos(pos) {
        xaxis = vec3(1, 0, 0);
        yaxis = vec3(0, 1, 0);
        zaxis = vec3(0, 0, 1);
    }

    mat4 view() {
        mat4 inv_trans = {
                vec4(1, 0, 0, 0),
                vec4(0, 1, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(-m_pos.x, -m_pos.y, -m_pos.z, 1)
        };
        mat4 inv_rot = {
                vec4(xaxis.x, yaxis.x, zaxis.x, 0),
                vec4(xaxis.y, yaxis.y, zaxis.y, 0),
                vec4(xaxis.z, yaxis.z, zaxis.z, 0),
                vec4(0, 0, 0, 1)
        };
        return inv_rot * inv_trans;
    }

    void translate(vec3 v) {
        m_pos += v;
    }

    void yaw(float rad) {
        m_yaw += rad;
        recalc_basis();
    }

    void pitch(float rad) {
        m_pitch -= rad;
        m_pitch = clamp<float>(m_pitch, -half_pi<float>(), half_pi<float>());
        recalc_basis();
    }
};

#endif //CG_FPS_CAMERA_H
