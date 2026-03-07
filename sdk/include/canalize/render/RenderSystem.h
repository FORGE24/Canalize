#pragma once
#include "../../CanalizeAPI.h"

namespace Canalize {
    namespace Render {

        // Wrapper for RenderSystem (OpenGL state manager)
        class CANALIZE_API RenderSystem {
        public:
            static void enableBlend();
            static void disableBlend();
            static void setShaderColor(float r, float g, float b, float a);
            static void bindTexture(int textureId);
        };

        // Simplified BufferBuilder for drawing primitives
        class CANALIZE_API BufferBuilder {
        public:
            BufferBuilder();
            
            void begin(int mode, int vertexFormat);
            void vertex(float x, float y, float z);
            void color(float r, float g, float b, float a);
            void tex(float u, float v);
            void end();
            
            // Direct draw
            void draw();
        };

        class CANALIZE_API Tessellator {
        public:
            static Tessellator& getInstance();
            BufferBuilder& getBuilder();
        };
    }
}
