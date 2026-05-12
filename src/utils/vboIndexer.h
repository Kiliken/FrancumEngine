#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <string.h>

namespace Utils
{
	void indexVBO_TBN(
		std::vector<glm::vec3> &in_vertices,
		std::vector<glm::vec2> &in_uvs,
		std::vector<glm::vec3> &in_normals,
		std::vector<glm::vec3> &in_tangents,
		std::vector<glm::vec3> &in_bitangents,

		std::vector<unsigned int> &out_indices,
		std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec2> &out_uvs,
		std::vector<glm::vec3> &out_normals,
		std::vector<glm::vec3> &out_tangents,
		std::vector<glm::vec3> &out_bitangents);

	// Returns true iif v1 can be considered equal to v2
	bool is_near(float v1, float v2);

	// Searches through all already-exported vertices
	// for a similar one.
	// Similar = same position + same UVs + same normal
	bool getSimilarVertexIndex(
		glm::vec3 &in_vertex,
		glm::vec2 &in_uv,
		glm::vec3 &in_normal,
		std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec2> &out_uvs,
		std::vector<glm::vec3> &out_normals,
		unsigned int &result);

	struct PackedVertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		bool operator<(const PackedVertex that) const
		{
			return memcmp((void *)this, (void *)&that, sizeof(PackedVertex)) > 0;
		};
	};

	void computeTangentBasis(
		// inputs
		std::vector<glm::vec3> &vertices,
		std::vector<glm::vec2> &uvs,
		std::vector<glm::vec3> &normals,
		// outputs
		std::vector<glm::vec3> &tangents,
		std::vector<glm::vec3> &bitangents);

	void computeTangentsIndexed(
		const std::vector<glm::vec3> &positions,
		const std::vector<glm::vec2> &uvs,
		const std::vector<glm::vec3> &normals,
		const std::vector<unsigned int> &indices,
		std::vector<glm::vec3> &tangents,
		std::vector<glm::vec3> &bitangents);
}
