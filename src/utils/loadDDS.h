#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <vector>

#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844
#define FOURCC_DX10 0x30315844

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3

namespace Utils{
    GLuint loadDDS(const char *imagepath);
    glm::vec3 hexToVec3(const char* hex);
}