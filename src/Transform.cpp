#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

Transform& Transform::Translate(glm::vec3 aTranslation)
{
    m_matrix = glm::translate(m_matrix, aTranslation);
    return *this;
}

Transform& Transform::Rotate(float aAngle, glm::vec3 aAxis)
{
    m_matrix = glm::rotate(m_matrix, aAngle, aAxis);
    return *this;
}

Transform& Transform::Scale(glm::vec3 aScale)
{
    m_matrix = glm::scale(m_matrix, aScale);
    return *this;
}

Transform& Transform::Scale(float aScale) { return Scale(glm::vec3{aScale}); }

const glm::mat4& Transform::Matrix() const { return m_matrix; }
