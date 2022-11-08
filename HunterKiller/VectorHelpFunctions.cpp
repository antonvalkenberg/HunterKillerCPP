#include "VectorHelpFunctions.h"
#include <cmath>

float VectorHelpFunctions::AngleDeg(const std::pair<float, float>& vector) {
    float angle = std::atan2f(vector.second, vector.first) * RADIANS_TO_DEGREES;
    if (angle < 0) angle += 360;
    return angle;
}

float VectorHelpFunctions::AngleDeg(const std::pair<float, float>& vector, const std::pair<float, float>& reference) {
    const float crs = vector.first * reference.second - vector.second * reference.first;
    const float dot = vector.first * reference.first + vector.second * reference.second;
    return std::atan2f(crs, dot) * RADIANS_TO_DEGREES;
}

void VectorHelpFunctions::SetAngleDeg(std::pair<float, float>& vector, const float degrees) {
    SetAngleRad(vector, degrees * DEGREES_TO_RADIANS);
}

void VectorHelpFunctions::SetAngleRad(std::pair<float, float>& vector, const float radians) {
    vector.first = std::sqrt(vector.first * vector.first + vector.second * vector.second);
    vector.second = 0.0f;
    RotateRad(vector, radians);
}

void VectorHelpFunctions::RotateRad(std::pair<float, float>& vector, const float radians) {
    const float cos = std::cos(radians);
    const float sin = std::sin(radians);

    const float newX = vector.first * cos - vector.second * sin;
    const float newY = vector.first * sin + vector.second * cos;

    vector.first = newX;
    vector.second = newY;
}