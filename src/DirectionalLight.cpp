#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
    yaw = 45.0f;
    pitch = -30.0f;
    power = 1.1f;
    ambient = 0.2f;
}

DirectionalLight::~DirectionalLight()
{
    glDeleteBuffers(1, &UBOID);
}

void DirectionalLight::BindToShader()
{
    glGenBuffers(1, &UBOID);
    glBindBuffer(GL_UNIFORM_BUFFER, UBOID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, UBOID);
}

void DirectionalLight::Update()
{
    const float radYaw = glm::radians(yaw);
    const float radPitch = glm::radians(pitch);

    pos.x = cos(radPitch) * sin(radYaw);
    pos.y = sin(radPitch);
    pos.z = cos(radPitch) * cos(radYaw);

    pos = glm::normalize(pos);

    UBOdata.dir = pos;
    UBOdata.power = power;
    UBOdata.ambient = ambient;
}