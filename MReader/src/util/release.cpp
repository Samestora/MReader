#include "util/release.h"

namespace MRUtil {
    void ReleaseTextures(std::vector<ID3D11ShaderResourceView*>& textures) {
        for (auto& texture : textures)
        {
            if (texture)
            {
                texture->Release();  // Release the texture
                texture = nullptr;   // Nullify the pointer to avoid dangling references
            }
        }

        // Clear the vector itself
        textures.clear();
        textures.shrink_to_fit();
    }
}