#include <mesh/model.h>

namespace rendermesh::mesh {
    NodeTransform::NodeTransform(const u32 parent, const u32 depth, const aiMatrix4x4& matrix) :
        parent(parent), depth(depth) {
        aiVector3D scaling,
            translation;
        aiQuaternion rot;

        matrix.Decompose(scaling, rot, translation);

        scale = glm::vec3(scaling.x, scaling.y, scaling.z);
        position = glm::vec3(translation.x, translation.y, translation.z);
        rotation = glm::quat(rot.w, rot.x, rot.y, rot.z);
    }
}