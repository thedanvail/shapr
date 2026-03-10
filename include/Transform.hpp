#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Transform
{
public:
    Transform& Translate(glm::vec3 aTranslation);
    Transform& Rotate(float aAngle, glm::vec3 aAxis);
    Transform& Scale(glm::vec3 aScale);
    Transform& Scale(float aScale);

    [[nodiscard]] const glm::mat4& Matrix() const;

private:
    glm::mat4 m_matrix{1.0f};
};
