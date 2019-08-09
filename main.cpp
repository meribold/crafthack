#include <algorithm>      // std::clamp
#include <cassert>        // assert
#include <cstdint>        // std::unit32_t
#include <cstdlib>        // std::exit
#include <fstream>        // std::ifstream
#include <iostream>       // std::cerr
#include <memory>         // std::unique_ptr
#include <sstream>        // std::stringstream
#include <string>         // std::string
#include <unordered_map>  // std::unordered_map

// We need to include this before `glfw3.h`.
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <png.h>  // libpng(3)

#include <glm/ext/scalar_constants.hpp>  // glm::pi
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>  // glm::rotate(float, vec3)

// Load a PNG image.  See libpng(3) and `example.c` from the libpng source code.
struct pngImage {
    pngImage(const char path[]) {
        image.version = PNG_IMAGE_VERSION;
        image.opaque = nullptr;
        if (png_image_begin_read_from_file(&image, path) == 0) {
            std::exit(192);
        }
        image.format = PNG_FORMAT_RGB;
        assert(PNG_IMAGE_SIZE(image) == 3072);
        buffer = std::unique_ptr<std::uint8_t>(new std::uint8_t[PNG_IMAGE_SIZE(image)]);
        if (!buffer) {
            png_image_free(&image);
            std::exit(193);
        }
        if (!png_image_finish_read(&image, nullptr, buffer.get(), 0, nullptr)) {
            std::exit(194);
        }
    }

    std::uint32_t getWidth() { return image.width; }
    std::uint32_t getHeight() { return image.height; }

    std::unique_ptr<uint8_t> buffer;

   private:
    png_image image;
};

struct ShaderProgram {
    ShaderProgram(const std::string& vertexShaderPath,
                  const std::string& fragmentShaderPath);
    void use();
    GLuint handle;
};

ShaderProgram::ShaderProgram(const std::string& vertexShaderPath,
                             const std::string& fragmentShaderPath) {
    std::string vertexShaderCodeString, fragmentShaderCodeString;
    const char* vertexShaderCode;
    const char* fragmentShaderCode;
    {
        std::stringstream sStream;
        sStream << std::ifstream(vertexShaderPath).rdbuf();
        vertexShaderCodeString = sStream.str();
        sStream.str("");
        sStream << std::ifstream(fragmentShaderPath).rdbuf();
        fragmentShaderCodeString = sStream.str();
    }
    vertexShaderCode = vertexShaderCodeString.c_str();
    fragmentShaderCode = fragmentShaderCodeString.c_str();

    GLint success;

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint length;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<char> infoLog(new char[length]);
        glGetShaderInfoLog(vertexShader, length, nullptr, infoLog.get());
        std::cerr << "Failed to compile vertex shader: " << infoLog.get() << '\n';
        std::exit(64);
    }

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint length;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<char> infoLog(new char[length]);
        glGetShaderInfoLog(fragmentShader, length, nullptr, infoLog.get());
        std::cerr << "Failed to compile fragment shader: " << infoLog.get() << '\n';
        std::exit(65);
    }

    this->handle = glCreateProgram();
    glAttachShader(this->handle, vertexShader);
    glAttachShader(this->handle, fragmentShader);
    glLinkProgram(this->handle);
    glGetProgramiv(this->handle, GL_LINK_STATUS, &success);
    if (!success) {
        GLint length;
        glGetProgramiv(this->handle, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<char> infoLog(new char[length]);
        glGetProgramInfoLog(this->handle, length, nullptr, infoLog.get());
        std::cerr << "Failed to link shader program: " << infoLog.get() << '\n';
        std::exit(66);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::use() { glUseProgram(this->handle); }

GLuint createVertexArrayObject() {
    // clang-format off
    GLfloat vertices[] = {
        // positions           // texture coordinates
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,  // top right
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,  // top left
    };
    // clang-format on

    GLuint indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };

    // Create a vertex array object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    // Create a vertex buffer object (VBO).
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Copy our vertices into the OpenGL buffer.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    // Create an element buffer object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);

    // Copy our indices into the buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    {
        pngImage image("texture.png");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image.buffer.get());
    }
    if (glGetError() != GL_NO_ERROR) std::exit(128);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Tell OpenGL how to interpret the vertex data (the `vertices` array).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return vao;
}

static glm::mat4 viewRotation(1.0f);
static glm::mat4 viewTranslation =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

static GLint viewMatrixLocation;
static GLint projectionMatrixLocation;

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f,
        100.0f);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
}

void cursorPosCallback(GLFWwindow*, double newX, double newY) {
    static constexpr double mouseSensitivity = 0.001f;
    static constexpr double maxPitch = 0.5 * glm::pi<double>();
    static constexpr double minPitch = -maxPitch;

    static double lastX = 320.0, lastY = 240.0;
    static double yaw = 0.0, pitch = 0.0;
    double deltaX = newX - lastX;
    double deltaY = newY - lastY;

    yaw += mouseSensitivity * deltaX;
    pitch = std::clamp(pitch + mouseSensitivity * deltaY, minPitch, maxPitch);

    viewRotation = glm::rotate(static_cast<float>(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
                   glm::rotate(static_cast<float>(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

    lastX = newX;
    lastY = newY;
}

static std::unordered_map<int, float> pressTimes;
static std::unordered_map<int, float> releaseTimes;

void keyCallback(GLFWwindow*, int key, int, int action, int) {
    float now = static_cast<float>(glfwGetTime());
    switch (action) {
        case GLFW_PRESS:
            pressTimes[key] = now;
            break;
        case GLFW_RELEASE:
            releaseTimes[key] = now;
            break;
    }
}

int main() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "CraftHack", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        return 3;
    }

    ShaderProgram shaderProgram("vertex_shader.vert", "fragment_shader.frag");
    GLuint vao = createVertexArrayObject();

    GLint modelMatrixLocation = glGetUniformLocation(shaderProgram.handle, "modelMatrix");
    viewMatrixLocation = glGetUniformLocation(shaderProgram.handle, "viewMatrix");
    projectionMatrixLocation =
        glGetUniformLocation(shaderProgram.handle, "projectionMatrix");

    shaderProgram.use();
    glBindVertexArray(vao);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    framebufferSizeCallback(window, 640, 480);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);
    cursorPosCallback(window, 320, 240);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.8f, 0.0f));
    modelMatrix =
        glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // The render loop
    float before = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());

        for (auto it = pressTimes.begin(); it != pressTimes.end();) {
            int keyCode = it->first;
            float pressTime = it->second;
            float pressDuration;
            if (glfwGetKey(window, keyCode) == GLFW_RELEASE) {
                // The key was intermediately released.
                auto searchIt = releaseTimes.find(keyCode);
                // The assumption here is that the callback always gets notified about key
                // release events before the return value of `glfwGetKey` changes.
                assert(searchIt != releaseTimes.end());
                float releaseTime = searchIt->second;
                pressDuration = releaseTime - before;
                releaseTimes.erase(searchIt);
                it = pressTimes.erase(it);
            } else {
                pressDuration = std::min(now - before, now - pressTime);
                ++it;
            }

            glm::vec3 viewForward =
                glm::transpose(glm::mat3(viewRotation)) * glm::vec3(0.0f, 0.0f, -1.0);
            glm::vec3 viewRight =
                glm::transpose(glm::mat3(viewRotation)) * glm::vec3(1.0f, 0.0f, 0.0);
            switch (keyCode) {
                case GLFW_KEY_W:
                    viewTranslation =
                        glm::translate(viewTranslation, -pressDuration * viewForward);
                    break;
                case GLFW_KEY_A:
                    viewTranslation =
                        glm::translate(viewTranslation, pressDuration * viewRight);
                    break;
                case GLFW_KEY_S:
                    viewTranslation =
                        glm::translate(viewTranslation, pressDuration * viewForward);
                    break;
                case GLFW_KEY_D:
                    viewTranslation =
                        glm::translate(viewTranslation, -pressDuration * viewRight);
                    break;
            }
        }

        glm::mat4 viewMatrix = viewRotation * viewTranslation;
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        before = now;
    }

    glfwTerminate();
}
