#include "../include/canalize/gui/Screen.h"
#include <iostream>

namespace Canalize {
    namespace GUI {

        void FontRenderer::drawString(const std::string& text, int x, int y, int color) {
            // In a real implementation, this would call into Java or use OpenGL directly
            // For now, we simulate a draw call log
            // std::cout << "[GUI] DrawString '" << text << "' at (" << x << "," << y << ") color " << std::hex << color << std::dec << std::endl;
        }

        int FontRenderer::getStringWidth(const std::string& text) {
            return (int)text.length() * 6; // Mock width
        }

    }
}
