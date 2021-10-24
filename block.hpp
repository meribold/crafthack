#ifndef BLOCK_HPP_RSJBSFD4
#define BLOCK_HPP_RSJBSFD4

#include <cstdint>  // std::uint8_t

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>   // glm::value_ptr
#include <glm/gtx/transform.hpp>  // glm::translate(float, vec3)

enum class Block : std::uint8_t {
    nothing,
    dirt,
};

GLuint createVertexArrayObject();

inline void drawBlock() { glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); }

template <std::size_t iSize, std::size_t jSize, std::size_t kSize>
inline void drawBlocks(GLuint shaderProgramHandle,
                       const Block (&blocks)[iSize][jSize][kSize]) {
    static GLuint vao = createVertexArrayObject();
    glBindVertexArray(vao);

    GLint modelMatrixLocation = glGetUniformLocation(shaderProgramHandle, "modelMatrix");

    // TODO: don't loop over all blocks every frame.
    for (std::size_t i = 0; i < iSize; ++i) {
        for (std::size_t j = 0; j < jSize; ++j) {
            for (std::size_t k = 0; k < kSize; ++k) {
                if (blocks[i][j][k] == Block::nothing) {
                    continue;
                }
                glm::mat4 modelMatrix = glm::translate(glm::vec3(
                    static_cast<float>(i), static_cast<float>(j), static_cast<float>(k)));
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
                                   glm::value_ptr(modelMatrix));
                drawBlock();
            }
        }
    }
}

#endif  // BLOCK_HPP_RSJBSFD4
