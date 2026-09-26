#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
    yaw = 45.0f;
    pitch = -30.0f;
}

DirectionalLight::~DirectionalLight()
{
    glDeleteBuffers(1, &glID);
}

void DirectionalLight::BindToShader(const GLuint &shader)
{
    glID = glGetUniformLocation(shader, "LightDirection_worldspace");
}

void DirectionalLight::Update()
{
    const float radYaw = glm::radians(yaw);
    const float radPitch = glm::radians(pitch);

    pos.x = cos(radPitch) * sin(radYaw);
    pos.y = sin(radPitch);
    pos.z = cos(radPitch) * cos(radYaw);

    pos = glm::normalize(pos);
}