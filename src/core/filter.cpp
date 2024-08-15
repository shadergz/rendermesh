#include <string>
#include <vector>

#include <types.h>
#include <core/filter.h>
namespace rendermesh::core {
    std::pair<bool, GLfloat> getAnisotropicFilter() {
        GLint extensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &extensions);
        std::vector<std::string> supported;
        for (u32 ext{}; ext < extensions; ext++) {
            const auto name{glGetStringi(GL_EXTENSIONS, ext)};
            if (name)
                supported.push_back(reinterpret_cast<const char*>(name));
        }
        // Checking if the host supports anisotropic filtering functionality
        GLfloat amount;
        bool enbAFilter{};
        if (std::ranges::find(supported, "GL_EXT_texture_filter_anisotropic") != supported.end()) {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &amount);
            enbAFilter = true;
        }

        return std::make_pair(enbAFilter, amount);
    }
}
