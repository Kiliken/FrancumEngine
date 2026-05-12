#pragma once

#include <glad/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <vector>

struct BinaryData
{
	std::vector<uint32_t> data; // Changed to uint32_t for alignment
	size_t sizeBytes;			// Keep track of the actual byte count
};

namespace Utils
{
	BinaryData LoadBinaryFile(const char *path);
	GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);
	GLuint LoadSPIRV(const char *vertex_file_path, const char *fragment_file_path);
}