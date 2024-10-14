#include"Texture.h"

#include"shaderClass.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) {
    // Load texture
    type = texType;
    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true); // Flip the texture vertically on load
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
    // Determine texture format based on number of color channels

    // Generate and configure texture
    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    // Set texture parameters
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Upload texture data to GPU
    glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
    glGenerateMipmap(texType); // Generate mipmaps for the texture

    // Free texture data
    stbi_image_free(bytes);
    glBindTexture(texType, 0); // Unbind the texture

}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    shader.Activate();
    glUniform1i(texUni, unit); // Set the texture unit to 0
}

void Texture::Bind() {
    glBindTexture(type, ID);
}

void Texture::Unbind() {
    glBindTexture(type, 0);
}

void Texture::Delete() {
    glDeleteTextures(1, &ID);
}