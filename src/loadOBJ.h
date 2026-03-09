#pragma once

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>
#include "Model.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

bool loadGlTf(const char *path, std::vector<Model> &models)
{
    cgltf_options options = {};
    cgltf_data *data = NULL;
    cgltf_result result;

    // Parse file
    result = cgltf_parse_file(&options, path, &data);
    if (result != cgltf_result_success)
        return false;

    // Load Data
    if (result != cgltf_result_success)
    {
        cgltf_free(data);
        return false;
    }

    // Process meshes
    for (size_t m = 0; m < data->meshes_count; m++)
    {
        cgltf_mesh *mesh = &data->meshes[m];

        for (size_t p = 0; p < mesh->primitives_count; p++)
        {
            cgltf_primitive *prim = &mesh->primitives[p];

            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> uvs;
            std::vector<unsigned int> indices;

            for (size_t a = 0; a < prim->attributes_count; a++)
            {
                cgltf_attribute *attr = &prim->attributes[a];
                cgltf_accessor *accessor = attr->data;

                if (attr->type == cgltf_attribute_type_position)
                {
                    positions.resize(accessor->count);
                    for (size_t i = 0; i < accessor->count; i++)
                    {
                        float v[3];
                        cgltf_accessor_read_float(accessor, i, v, 3);
                        positions[i] = glm::vec3(v[0], v[1], v[2]);
                    }
                }
                else if (attr->type == cgltf_attribute_type_normal)
                {
                    normals.resize(accessor->count);
                    for (size_t i = 0; i < accessor->count; i++)
                    {
                        float v[3];
                        cgltf_accessor_read_float(accessor, i, v, 3);
                        normals[i] = glm::vec3(v[0], v[1], v[2]);
                    }
                }
                else if (attr->type == cgltf_attribute_type_texcoord)
                {
                    uvs.resize(accessor->count);
                    for (size_t i = 0; i < accessor->count; i++)
                    {
                        float v[2];
                        cgltf_accessor_read_float(accessor, i, v, 2);
                        uvs[i] = glm::vec2(v[0], v[1]);
                    }
                }
            }

            // --- Read indices ---
            if (prim->indices)
            {
                cgltf_accessor *accessor = prim->indices;
                indices.resize(accessor->count);

                for (size_t i = 0; i < accessor->count; i++)
                {
                    uint32_t idx = 0;
                    cgltf_accessor_read_uint(accessor, i, &idx, 1);
                    indices[i] = idx;
                }
            }

            cgltf_material *mat = prim->material;

            cgltf_texture *diffuseTex = nullptr;
            cgltf_texture *normalTex = nullptr;
            cgltf_texture *specularTex = nullptr;

            // Diffuse
            if (mat && mat->pbr_metallic_roughness.base_color_texture.texture)
                diffuseTex = mat->pbr_metallic_roughness.base_color_texture.texture;

            // Normal
            if (mat && mat->normal_texture.texture)
                normalTex = mat->normal_texture.texture;

            // Specular
            if (mat && mat->pbr_metallic_roughness.metallic_roughness_texture.texture)
                specularTex = mat->pbr_metallic_roughness.metallic_roughness_texture.texture;

            models.emplace_back(positions, uvs, normals);
        }
    }

    cgltf_free(data);
    return true;
}

bool loadOBJ(
    const char *path,
    std::vector<Model> &models)
{

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    bool firstIteration = true;

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1)
    {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else if (strcmp(lineHeader, "o") == 0)
        {
            if (!firstIteration)
            {
                std::vector<glm::vec3> out_vertices;
                std::vector<glm::vec2> out_uvs;
                std::vector<glm::vec3> out_normals;

                for (unsigned int i = 0; i < vertexIndices.size(); i++)
                {
                    unsigned int vi = vertexIndices[i];
                    unsigned int ti = uvIndices[i];
                    unsigned int ni = normalIndices[i];

                    out_vertices.push_back(temp_vertices[vi - 1]);
                    out_uvs.push_back(temp_uvs[ti - 1]);
                    out_normals.push_back(temp_normals[ni - 1]);
                }

                models.emplace_back(out_vertices, out_uvs, out_normals);

                // New Object
                vertexIndices.clear();
                uvIndices.clear();
                normalIndices.clear();
            }

            firstIteration = false;
        }
    }

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }

    models.emplace_back(out_vertices, out_uvs, out_normals);
    fclose(file);

    return true;
}