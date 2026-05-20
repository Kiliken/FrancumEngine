#include "loadDDS.h"

GLuint Utils::loadDDS(const char *imagepath)
{

    unsigned char header[124];

    FILE *fp;

    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL)
        return 0;

    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0)
    {
        fclose(fp);
        return 0;
    }

    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int height = *(unsigned int *)&(header[8]);
    unsigned int width = *(unsigned int *)&(header[12]);
    unsigned int linearSize = *(unsigned int *)&(header[16]);
    unsigned int mipMapCount = *(unsigned int *)&(header[24]);
    unsigned int fourCC = *(unsigned int *)&(header[80]);

    unsigned char *buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char *)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);

    signed int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        free(buffer);
        return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                               0, size, buffer + offset);

        offset += size;
        width /= 2;
        height /= 2;

        if(width < 1) width = 1;
		if(height < 1) height = 1;
        
    }
    free(buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}

glm::vec3 Utils::hexToVec3(const char* hex) {
    if (!hex) return glm::vec3(1.0f, 0.0f, 1.0f);

    // Skip prefixes
    if (hex[0] == '#') {
        hex++;
    } else if ((hex[0] == '0') && (hex[1] == 'x' || hex[1] == 'X')) {
        hex += 2;
    }

    // Check digits
    if (std::strlen(hex) != 6)
        return glm::vec3(1.0f, 0.0f, 1.0f);

    // Convert hex string to integer
    unsigned int value = std::strtoul(hex, nullptr, 16);

    float r = ((value >> 16) & 0xFF) / 255.0f;
    float g = ((value >> 8) & 0xFF) / 255.0f;
    float b = (value & 0xFF) / 255.0f;

    return glm::vec3(r, g, b);
}
