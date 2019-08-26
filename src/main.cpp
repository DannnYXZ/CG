#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "shader.h"
#include "camera/look_at_camera.h"
#include "camera/fps_camera.h"
#include "fps_camera_controller.h"
#include "camera/arcball_camera.h"
#include "arcball_camera_controller.h"
#include "utils.h"
#include "hexagons.h"

using namespace glm;

int SCR_WIDTH = 800;
int SCR_HEIGHT = 800;

static void error_callback(int error, const char *description);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void load2DTexture(uint &id, const std::string &path, bool alpha = false);

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

static void process_input(GLFWwindow *window);

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

LookAtCamera look_at_camera(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
FPSCamera fps_camera(vec3(0, 0, 3));
FPSCameraController fps_controller(&fps_camera);
ArcballCamera arcball_camera(identity<quat>(), vec3(0, 0, 0), vec3(0, 0, 5));
ArcballCameraController arcball_controller(&arcball_camera, SCR_WIDTH, SCR_HEIGHT);
float timestamp;
float loop_deltatime;
double last_mouse_x, last_mouse_y;


mat4 view;
mat4 projection;

struct {
    GLenum PolygonMode = GL_FILL;
    bool drawPoints = true;
} settings;

int main() {
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    //glfwMaximizeWindow(window);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader rainbowShader("shaders/rainbowShader.vs", "shaders/rainbowShader.fs");
    Shader posColorShader("shaders/posColor.vs", "shaders/posColor.fs");
    Shader planeShader("shaders/texture.vs", "shaders/texture.fs");
    uint planeModelUniform = glGetUniformLocation(planeShader.ID, "model");
    uint planeViewUniform = glGetUniformLocation(planeShader.ID, "view");
    uint planeProjUniform = glGetUniformLocation(planeShader.ID, "projection");
    Shader cubeShader("shaders/3D.vs", "shaders/3D.fs");
    uint cubeModelUniform = glGetUniformLocation(cubeShader.ID, "model");
    uint cubeViewUniform = glGetUniformLocation(cubeShader.ID, "view");
    uint cubeProjUniform = glGetUniformLocation(cubeShader.ID, "projection");
    Shader pointsShader("shaders/point.vs", "shaders/point.fs");
    uint pointsModelUniform = glGetUniformLocation(pointsShader.ID, "model");
    uint pointsViewUniform = glGetUniformLocation(pointsShader.ID, "view");
    uint pointsProjUniform = glGetUniformLocation(pointsShader.ID, "projection");

    // plane
    float plane[] = {
            // top right
            0.9, 0.9, 0.0, 1.0, 0.0, 0.0, 2.0, 2.0,
            // bottom right
            0.9, -0.9, 0.0, 0.0, 1.0, 0.0, 2.0, 0.0,
            // bottom left
            -0.9, -0.9, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
            // top left
            -0.9, 0.9, 0.0, 1.0, 1.0, 0.0, 0.0, 2.0
    };

    uint order[] = {
            0, 1, 3,
            1, 2, 3
    };

    uint planeVAO, planeVBO, planeEBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(order), order, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // cube data
    float cube[] = {
            // front
            0.5, 0.5, 0.5, 1, 1,
            0.5, -0.5, 0.5, 1, 0,
            -0.5, 0.5, 0.5, 0, 1,
            -0.5, -0.5, 0.5, 0, 0,
            // back
            0.5, 0.5, -0.5, 0, 0,
            0.5, -0.5, -0.5, 1, 0,
            -0.5, 0.5, -0.5, 0, 1,
            -0.5, -0.5, -0.5, 1, 1,
            // left
            0.5, 0.5, -0.5, 0, 0,
            0.5, 0.5, 0.5, 1, 0,
            0.5, -0.5, -0.5, 0, 1,
            0.5, -0.5, 0.5, 1, 1,
            // right
            -0.5, 0.5, -0.5, 0, 0,
            -0.5, 0.5, 0.5, 1, 0,
            -0.5, -0.5, -0.5, 0, 1,
            -0.5, -0.5, 0.5, 1, 1,
            // up
            0.5, 0.5, -0.5, 0, 0,
            0.5, 0.5, 0.5, 1, 0,
            -0.5, 0.5, -0.5, 0, 1,
            -0.5, 0.5, 0.5, 1, 1,
            // down
            0.5, -0.5, -0.5, 0, 0,
            0.5, -0.5, 0.5, 1, 0,
            -0.5, -0.5, -0.5, 0, 1,
            -0.5, -0.5, 0.5, 1, 1,
    };
    uint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void *) 12);
    glEnableVertexAttribArray(1);

    // textures
    uint woodTexture, eyeTexture;
    load2DTexture(woodTexture, "assets/container.jpg");
    load2DTexture(eyeTexture, "assets/triangle.png", true);
    planeShader.use();
    planeShader.setInt("texSampler0", 0);
    planeShader.setInt("texSampler1", 1);
    cubeShader.use();
    cubeShader.setInt("texSampler0", 0);
    cubeShader.setInt("texSampler1", 1);

    // animations
    HexagonAnimation hexAnim(view, projection);
    hexAnim.reset();

    glLineWidth(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    while (!glfwWindowShouldClose(window)) {
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, settings.PolygonMode);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, eyeTexture);

        float time = glfwGetTime();
        loop_deltatime = time - timestamp;
        timestamp = time;

        // calculating MVP

        view = fps_camera.view();
        projection = perspective(radians(45.f), SCR_WIDTH * 1.f / SCR_HEIGHT, 0.1f, 100.f);
        //mat4 projection = perspective(radians(45.f), SCR_WIDTH * 1.f / SCR_HEIGHT, 0.1f, 100.f);
        float R = 10;
        vec3 cam_pos(cos(time) * R, 0, sin(time) * R);
        look_at_camera.translate(vec3(cam_pos.x, cam_pos.y, cam_pos.z) * loop_deltatime);
        //mat4 view = look_at_camera.view();
        //mat4 view = arcball_camera.view();

        // draw origin planes
        planeShader.use();
        mat4 model(1.0f);
        glUniformMatrix4fv(planeModelUniform, 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(planeViewUniform, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(planeProjUniform, 1, GL_FALSE, value_ptr(projection));
        glBindVertexArray(planeVAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        model = rotate(model, radians(90.f), vec3(1, 0, 0));
        glUniformMatrix4fv(planeModelUniform, 1, GL_FALSE, value_ptr(model));
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        model = rotate(model, radians(90.f), vec3(0, 1, 0));
        glUniformMatrix4fv(planeModelUniform, 1, GL_FALSE, value_ptr(model));
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // scattered cubes
        vec3 cubePositions[] = {
                vec3(2.0f, 5.0f, -15.0f),
                vec3(-1.5f, -2.2f, -2.5f),
                vec3(-3.8f, -2.0f, -12.3f),
                vec3(2.4f, -0.4f, -3.5f),
                vec3(-1.7f, 3.0f, -7.5f),
                vec3(1.3f, -2.0f, -2.5f),
                vec3(1.5f, 2.0f, -2.5f),
                vec3(1.5f, 0.2f, -1.5f),
                vec3(-1.3f, 1.0f, -1.5f)
        };
        pointsShader.use();
        glUniformMatrix4fv(pointsViewUniform, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(pointsProjUniform, 1, GL_FALSE, value_ptr(projection));
        cubeShader.use();
        glUniformMatrix4fv(cubeViewUniform, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(cubeProjUniform, 1, GL_FALSE, value_ptr(projection));
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) {
            float angle = 20.0f * i;
            mat4 _model(1.f);
            _model = translate(_model, cubePositions[i]);
            _model = rotate(_model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
            cubeShader.use();
            glUniformMatrix4fv(cubeModelUniform, 1, GL_FALSE, value_ptr(_model));
            //glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
            if (settings.drawPoints) {
                pointsShader.use();
                glUniformMatrix4fv(pointsModelUniform, 1, GL_FALSE, value_ptr(_model));
                //glDrawArrays(GL_POINTS, 0, 24);
            }
        }

        // rotating cube
        cubeShader.use();
        model = mat4(1);
        static quat q = angleAxis(time, normalize(vec3(1, 0, 0)));
        quat p = angleAxis(0.1f, normalize(vec3(1, 1, 0)));
        q *= p;
        model = toMat4(q) * model;
        glUniformMatrix4fv(cubeModelUniform, 1, GL_FALSE, value_ptr(model));
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);

        // hexagons
        hexAnim.draw();
        process_input(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

static void load2DTexture(uint &id, const std::string &path, bool alpha) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    if (alpha)
        stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        alpha
        ? glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data)
        : glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else
        std::cout << "Failed to load texture: " << path << std::endl;
    stbi_image_free(data);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    // mouse coordinates now are not the same
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if (settings.PolygonMode == GL_LINE)
            settings.PolygonMode = GL_FILL;
        else
            settings.PolygonMode = GL_LINE;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        settings.drawPoints = !settings.drawPoints;
}

static void process_input(GLFWwindow *window) {
    int trackable_keys[] = {
            GLFW_KEY_W,
            GLFW_KEY_A,
            GLFW_KEY_S,
            GLFW_KEY_D,
            GLFW_KEY_Q,
            GLFW_KEY_E,
            GLFW_KEY_Z,
            GLFW_KEY_C,
            GLFW_KEY_SPACE,
            GLFW_KEY_LEFT_SHIFT
    };

    for (int trackable_key : trackable_keys) {
        if (glfwGetKey(window, trackable_key) == GLFW_PRESS)
            fps_controller.processKey(trackable_key, loop_deltatime);
    }
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    static bool first_mouse = true;
    if (first_mouse) {
        first_mouse = false;
        last_mouse_x = xpos;
        last_mouse_y = ypos;
    }
    float dx = xpos - last_mouse_x;
    float dy = ypos - last_mouse_y;
    last_mouse_x = xpos;
    last_mouse_y = ypos;
    fps_controller.process_mouse(dx, dy);
    arcball_controller.mouseMove(xpos, ypos);
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    arcball_controller.mouseButton(button, action, mods, mx, my);
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    arcball_controller.mouseScroll(xoffset, yoffset);
}