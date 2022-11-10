#pragma once
#include <numbers>
#include <utility>

class VectorHelpFunctions
{
public:
    static constexpr float RADIANS_TO_DEGREES = 180.0f / std::numbers::pi_v<float>;
    static constexpr float DEGREES_TO_RADIANS = std::numbers::pi_v<float> / 180.0f;
    static float AngleDeg(const std::pair<float, float>& vector);
    static float AngleDeg(const std::pair<float, float>& vector, const std::pair<float, float>& reference);
    static void SetAngleDeg(std::pair<float, float>& vector, float degrees);
    static void SetAngleRad(std::pair<float, float>& vector, float radians);
    static void RotateRad(std::pair<float, float>& vector, float radians);
};
