#include <cstdlib>   // std::exit
#include <fstream>   // std::ifstream
#include <iostream>  // std::cerr
#include <memory>    // std::unique_ptr
#include <sstream>   // std::stringstream
#include <string>    // std::string

// We need to include this before `glfw3.h`.
#include <GL/glew.h>

#include <GLFW/glfw3.h>

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
        // positions        // colors
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // top right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,  // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top left
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

    // Tell OpenGL how to interpret the vertex data (the `vertices` array).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

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

    ShaderProgram shaderProgram("vertex_shader.vert", "fragment_shader.frag");
    GLuint vao = createVertexArrayObject();

    shaderProgram.use();
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
