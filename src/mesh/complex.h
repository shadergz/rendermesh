#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <buffer/submit.h>
#include <mesh/model.h>
namespace rendermesh::mesh {
    class Complex {
    public:
        explicit Complex(const std::shared_ptr<buffer::Submit>& submit, const std::filesystem::path& path);

        void populateBuffers() const;
        void draw() const;
    private:
        std::shared_ptr<buffer::Submit> submitter;
        std::vector<Model> meshes;

        void processMesh(Model& modelMesh, const aiMesh& implMesh, const aiScene& scene) const;
        std::vector<buffer::Texture> getTextureMaps(const aiMaterial& material, aiTextureType type) const;

         std::filesystem::path objDir;
    };
}
