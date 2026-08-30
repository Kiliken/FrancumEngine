#pragma once

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>
#include "Model.h"

#include <cgltf.h>


namespace Utils{
    const char* getTexturePath(const char *path, cgltf_texture* tex);
    bool loadGlTf(const char *path, std::vector<Model*> &models);
    bool loadOBJ(const char *path,std::vector<Model*> &models);
}