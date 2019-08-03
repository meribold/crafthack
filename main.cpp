#include <cstdlib>   // std::exit
#include <iostream>  // std::cerr
#include <memory>    // std::unique_ptr

// We need to include this before `glfw3.h`.
#include <GL/glew.h>

#include <GLFW/glfw3.h>

GLuint createShaderProgram() {
    const char* vertexShaderCode = R"(
        #version 330 core
        in vec3 position;
        void main() {
            gl_Position = vec4(position, 1.0);
        })";

    const char* fragmentShaderCode = R"(
        #version 330 core
        out vec4 color;
        void main() {
            color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        })";

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
        std::exit(65);
    }

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        std::exit(66);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createVertexArrayObject() {
    GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

    // Create a vertex array object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    // Create a vertex buffer object (VBO).
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Copy our vertices into a buffer for OpenGL.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    // Tell OpenGL how to interpret the vertex data (the `vertices` array).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    return vao;
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

    GLuint shaderProgram = createShaderProgram();
    GLuint vao = createVertexArrayObject();

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    // The render loop
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
