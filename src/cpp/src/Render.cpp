#include "../include/canalize/render/RenderSystem.h"
#include <iostream>

namespace Canalize {
    namespace Render {

        void RenderSystem::enableBlend() { /* GL_ENABLE(GL_BLEND) */ }
        void RenderSystem::disableBlend() { /* GL_DISABLE(GL_BLEND) */ }
        void RenderSystem::setShaderColor(float r, float g, float b, float a) { /* Uniform update */ }
        void RenderSystem::bindTexture(int textureId) { /* glBindTexture */ }

        BufferBuilder::BufferBuilder() {}
        
        void BufferBuilder::begin(int mode, int vertexFormat) {
            // Start building vertex buffer
        }

        void BufferBuilder::vertex(float x, float y, float z) {}
        void BufferBuilder::color(float r, float g, float b, float a) {}
        void BufferBuilder::tex(float u, float v) {}
        
        void BufferBuilder::end() {
            // Finish building
        }

        void BufferBuilder::draw() {
            // Upload to GPU and draw
        }

        Tessellator& Tessellator::getInstance() {
            static Tessellator instance;
            return instance;
        }

        BufferBuilder& Tessellator::getBuilder() {
            static BufferBuilder builder;
            return builder;
        }

    }
}
