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
        out vec3 vertexColor;
        void main() {
            gl_Position = vec4(position, 1.0);
            vertexColor = position.xxx;
        })";

    const char* fragmentShaderCode = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 color;
        void main() {
            color = vec4(vertexColor * vertexColor, 1.0f);
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
        GLint length;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<char> infoLog(new char[length]);
        glGetShaderInfoLog(fragmentShader, length, nullptr, infoLog.get());
        std::cerr << "Failed to compile fragment shader: " << infoLog.get() << '\n';
        std::exit(65);
    }

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLint length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<char> infoLog(new char[length]);
        glGetProgramInfoLog(shaderProgram, length, nullptr, infoLog.get());
        std::cerr << "Failed to link shader program: " << infoLog.get() << '\n';
        std::exit(66);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createVertexArrayObject() {
    GLfloat vertices[] = {
        0.5f,  0.5f,  0.0f,  // top right
        0.5f,  -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f   // top left
    };

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
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
