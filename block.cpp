#include "block.hpp"

#include <cassert>  // assert
#include <memory>   // std::unique_ptr

#include <png.h>  // libpng(3)

// Load a PNG image.  See libpng(3) and `example.c` from the libpng source code.
struct pngImage {
    pngImage(const char path[]) {
        image.version = PNG_IMAGE_VERSION;
        image.opaque = nullptr;
        if (png_image_begin_read_from_file(&image, path) == 0) {
            std::exit(192);
        }
        image.format = PNG_FORMAT_RGB;
        assert(PNG_IMAGE_SIZE(image) == 768);
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

GLuint createVertexArrayObject() {
    // clang-format off
    GLfloat vertices[] = {
        // positions            // texture coordinates
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  // top right front
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  // bottom right front
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  // bottom left front
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  // top left front

         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  // top right back
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  // bottom right back
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // bottom left back
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  // top left back

         0.5f, -0.5f,  0.5f,    1.0f, 1.0f,  // bottom right front
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  // bottom right back
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // bottom left back
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f,  // bottom left front

         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  // top right front
         0.5f,  0.5f, -0.5f,    1.0f, 0.0f,  // top right back
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,  // top left back
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  // top left front

        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  // top left front
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  // bottom left front
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // bottom left back
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  // top left back

         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  // top right front
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  // bottom right front
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // bottom right back
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  // top right back
    };

    // Each line defines one triangle.  Two lines define one face of the cube.
    GLuint indices[] = {
         0,  1,  3,  // back
         1,  2,  3,
         4,  5,  7,  // front
         5,  6,  7,
         8,  9, 11,  // bottom
         9, 10, 11,
        12, 13, 15,  // top
        13, 14, 15,
        16, 17, 19,  // left
        17, 18, 19,
        20, 21, 23,  // right
        21, 22, 23,
    };
    // clang-format on

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
        pngImage image("dirt.png");
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
