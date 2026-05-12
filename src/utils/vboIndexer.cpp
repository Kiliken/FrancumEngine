#include "vboIndexer.h"

// Returns true iif v1 can be considered equal to v2
bool Utils::is_near(float v1, float v2)
{
	return fabs(v1 - v2) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool Utils::getSimilarVertexIndex(
	glm::vec3 &in_vertex,
	glm::vec2 &in_uv,
	glm::vec3 &in_normal,
	std::vector<glm::vec3> &out_vertices,
	std::vector<glm::vec2> &out_uvs,
	std::vector<glm::vec3> &out_normals,
	unsigned int &result)
{
	// Lame linear search
	for (unsigned int i = 0; i < out_vertices.size(); i++)
	{
		if (
			is_near(in_vertex.x, out_vertices[i].x) &&
			is_near(in_vertex.y, out_vertices[i].y) &&
			is_near(in_vertex.z, out_vertices[i].z) &&
			is_near(in_uv.x, out_uvs[i].x) &&
			is_near(in_uv.y, out_uvs[i].y) &&
			is_near(in_normal.x, out_normals[i].x) &&
			is_near(in_normal.y, out_normals[i].y) &&
			is_near(in_normal.z, out_normals[i].z))
		{
			result = i;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}

void Utils::indexVBO_TBN(
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
	std::vector<glm::vec3> &out_bitangents)
{
	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); i++)
	{

		// Try to find a similar vertex in out_XXXX
		unsigned int index;
		bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);

		if (found)
		{ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);

			// Average the tangents and the bitangents
			out_tangents[index] += in_tangents[i];
			out_bitangents[index] += in_bitangents[i];
		}
		else
		{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			out_tangents.push_back(in_tangents[i]);
			out_bitangents.push_back(in_bitangents[i]);
			out_indices.push_back((unsigned int)out_vertices.size() - 1);
		}
	}
}

void Utils::computeTangentBasis(
	// inputs
	std::vector<glm::vec3> &vertices,
	std::vector<glm::vec2> &uvs,
	std::vector<glm::vec3> &normals,
	// outputs
	std::vector<glm::vec3> &tangents,
	std::vector<glm::vec3> &bitangents)
{
	for (int i = 0; i < vertices.size(); i += 3)
	{

		// Shortcuts for vertices
		glm::vec3 &v0 = vertices[i + 0];
		glm::vec3 &v1 = vertices[i + 1];
		glm::vec3 &v2 = vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2 &uv0 = uvs[i + 0];
		glm::vec2 &uv1 = uvs[i + 1];
		glm::vec2 &uv2 = uvs[i + 2];

		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		// Set the same tangent for all three vertices of the triangle.
		// They will be merged later, in vboindexer.cpp
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		// Same thing for bitangents
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
}

void Utils::computeTangentsIndexed(
	const std::vector<glm::vec3> &positions,
	const std::vector<glm::vec2> &uvs,
	const std::vector<glm::vec3> &normals,
	const std::vector<unsigned int> &indices,
	std::vector<glm::vec3> &tangents,
	std::vector<glm::vec3> &bitangents)
{
	tangents.resize(positions.size(), glm::vec3(0.0f));
	bitangents.resize(positions.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		unsigned int i0 = indices[i + 0];
		unsigned int i1 = indices[i + 1];
		unsigned int i2 = indices[i + 2];

		const glm::vec3 &v0 = positions[i0];
		const glm::vec3 &v1 = positions[i1];
		const glm::vec3 &v2 = positions[i2];

		const glm::vec2 &uv0 = uvs[i0];
		const glm::vec2 &uv1 = uvs[i1];
		const glm::vec2 &uv2 = uvs[i2];

		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents[i0] += tangent;
		tangents[i1] += tangent;
		tangents[i2] += tangent;

		bitangents[i0] += bitangent;
		bitangents[i1] += bitangent;
		bitangents[i2] += bitangent;
	}

	// Normalize
	for (size_t i = 0; i < tangents.size(); i++)
	{
		tangents[i] = glm::normalize(tangents[i]);
		bitangents[i] = glm::normalize(bitangents[i]);
	}
}
