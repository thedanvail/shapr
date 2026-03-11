#include "Transform.hpp"

#include "Exception.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec4.hpp>

namespace
{
    glm::vec3 ApplyToPoint(const Transform& aTransform, glm::vec3 aPoint)
    {
        return glm::vec3{aTransform.Matrix() * glm::vec4{aPoint, 1.0f}};
    }
} // namespace

TEST_CASE("Transform - Scale affects point coordinates")
{
    Transform transform;
    transform.Scale(glm::vec3{2.0f, 3.0f, 4.0f});

    const glm::vec3 point = ApplyToPoint(transform, glm::vec3{1.0f, 1.0f, 1.0f});
    REQUIRE(point.x == Catch::Approx(2.0f));
    REQUIRE(point.y == Catch::Approx(3.0f));
    REQUIRE(point.z == Catch::Approx(4.0f));
}

TEST_CASE("Transform - Rotate around z axis")
{
    Transform transform;
    transform.Rotate(glm::radians(90.0f), glm::vec3{0.0f, 0.0f, 1.0f});

    const glm::vec3 point = ApplyToPoint(transform, glm::vec3{1.0f, 0.0f, 0.0f});
    REQUIRE(point.x == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(point.y == Catch::Approx(1.0f).margin(1e-5f));
    REQUIRE(point.z == Catch::Approx(0.0f).margin(1e-5f));
}

TEST_CASE("Transform - Operation order changes result")
{
    Transform translateThenScale;
    translateThenScale.Translate(glm::vec3{1.0f, 0.0f, 0.0f}).Scale(2.0f);

    Transform scaleThenTranslate;
    scaleThenTranslate.Scale(2.0f).Translate(glm::vec3{1.0f, 0.0f, 0.0f});

    const glm::vec3 p1 = ApplyToPoint(translateThenScale, glm::vec3{0.0f, 0.0f, 0.0f});
    const glm::vec3 p2 = ApplyToPoint(scaleThenTranslate, glm::vec3{0.0f, 0.0f, 0.0f});

    REQUIRE(glm::distance(p1, p2) > 1e-5f);
}

TEST_CASE("Transform - Zero rotation axis throws")
{
    Transform transform;
    REQUIRE_THROWS_AS(transform.Rotate(glm::radians(30.0f), glm::vec3{0.0f, 0.0f, 0.0f}), ZeroRotationAxisError);
}
