#include <buffer/blend.h>

namespace rendermesh::buffer {

    void MixableMaterial::set(const GLuint tex) {
        if (inUse)
            return;
        texture = tex;
        inUse = true;
    }

    Blend::Blend(const aiTextureType type, const GLuint texture) {
        blending(type, texture);
    }

    void Blend::bind(const std::shared_ptr<raster::Shaders>& shader, const aiTextureType type) const {
        GLuint unit{};
        GLuint tex{diffuse.texture};

        if (type == aiTextureType_SPECULAR) {
            unit += 1;
            tex = specular.texture;
        }
        else if (type == aiTextureType_EMISSIVE) {
            unit += 2;
            tex = emissive.texture;
        }

        constexpr std::array<std::string_view, 3> uniforms{"diffuse", "specular", "emissive"};
        glActiveTexture(GL_TEXTURE0 + unit);

        if (type == aiTextureType_DIFFUSE)

            glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(shader->getUniform(uniforms[unit]), unit);
    }

    void Blend::blending(const aiTextureType type, const GLuint texture) {
        if (type == aiTextureType_DIFFUSE)
            diffuse.set(texture);
        if (type == aiTextureType_SPECULAR)
            specular.set(texture);
        if (type == aiTextureType_EMISSIVE)
            emissive.set(texture);
    }
}