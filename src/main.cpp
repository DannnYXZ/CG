#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <stb_image/stb_image.h>
#include "shader.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

static void error_callback(int error, const char *description);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void load2DTexture(uint &id, const std::string &path, bool alpha = false);

float vertices[] = {
        // positions          // colors           // texture coords
        0.9, 0.9, 0.0, 1.0, 0.0, 0.0, 2.0, 2.0,   // top right
        0.9, -0.9, 0.0, 0.0, 1.0, 0.0, 2.0, 0.0,   // bottom right
        -0.9, -0.9, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,   // bottom left
        -0.9, 0.9, 0.0, 1.0, 1.0, 0.0, 0.0, 2.0    // top left
};

float cube[] = {
        // front
        0.5, 0.5, 0.5, 0, 0,
        0.5, -0.5, 0.5, 1, 0,
        -0.5, 0.5, 0.5, 0, 1,
        -0.5, -0.5, 0.5, 1, 1,
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

uint indices[] = {
        0, 1, 2,   // first triangle
        0, 2, 3    // second triangle
};

float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f
};

int main() {
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader rainbowShader("shaders/rainbowShader.vs", "shaders/rainbowShader.fs");
    Shader posColorShader("shaders/posColor.vs", "shaders/posColor.fs");
    Shader textureShader("shaders/texture.vs", "shaders/texture.fs");
    Shader cubeShader("shaders/3D.vs", "shaders/3D.fs");

    uint planeVAO, planeVBO, EBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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

    uint woodTexture, eyeTexture;
    load2DTexture(woodTexture, "assets/container.jpg");
    load2DTexture(eyeTexture, "assets/triangle.png", true);
    textureShader.use();
    textureShader.setInt("texSampler0", 0);
    textureShader.setInt("texSampler1", 1);
    cubeShader.use();
    cubeShader.setInt("texSampler0", 0);
    cubeShader.setInt("texSampler1", 1);


    uint planeModelUniform = glGetUniformLocation(textureShader.ID, "model");
    uint planeViewUniform = glGetUniformLocation(textureShader.ID, "view");
    uint planeProjUniform = glGetUniformLocation(textureShader.ID, "projection");

    uint cubeModelUniform = glGetUniformLocation(cubeShader.ID, "model");
    uint cubeViewUniform = glGetUniformLocation(cubeShader.ID, "view");
    uint cubeProjUniform = glGetUniformLocation(cubeShader.ID, "projection");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //Camera camera(vec3(0, 0, 3),);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = glfwGetTime();
        mat4 model(1.0f);
        mat4 view(1.0f);
        view = translate(view, vec3(0, 0, -3));
        mat4 projection(1);
        projection = perspective(radians(45.f), SCR_WIDTH * 1.f / SCR_HEIGHT, 0.1f, 100.f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, eyeTexture);


        textureShader.use();
        //model = rotate(model, radians(-55.f), vec3(1, 0, 0));
        glUniformMatrix4fv(planeModelUniform, 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(planeViewUniform, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(planeProjUniform, 1, GL_FALSE, value_ptr(projection));

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        cubeShader.use();
        glUniformMatrix4fv(cubeModelUniform, 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(cubeViewUniform, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(cubeProjUniform, 1, GL_FALSE, value_ptr(projection));

        vec3 cubePositions[] = {
                vec3(0.0f, 0.0f, 0.0f),
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

        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++) {
            float angle = 20.0f * i;
            mat4 _model(1.f);
            _model = translate(_model, cubePositions[i]);
            _model = rotate(_model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(cubeModelUniform, 1, GL_FALSE, value_ptr(_model));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
        }

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
    glViewport(0, 0, width, height);
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}