#pragma once
#include "../../CanalizeAPI.h"
#include <string>

// Disable MSVC warning C4251 for STL containers in DLL-exported classes
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace Canalize {
    namespace GUI {

        // Abstract base class for all GUI screens
        class CANALIZE_API Screen {
        public:
            Screen(const std::string& title) : mTitle(title) {}
            virtual ~Screen() = default;

            virtual void init() {}
            virtual void render(int mouseX, int mouseY, float partialTicks) {}
            virtual void onClose() {}
            
            // Input events
            virtual bool mouseClicked(double mouseX, double mouseY, int button) { return false; }
            virtual bool keyPressed(int keyCode, int scanCode, int modifiers) { return false; }

            std::string getTitle() const { return mTitle; }

        protected:
            std::string mTitle;
            int width, height;
        };

        // Represents the Minecraft FontRenderer
        class CANALIZE_API FontRenderer {
        public:
            static void drawString(const std::string& text, int x, int y, int color);
            static int getStringWidth(const std::string& text);
        };
    }
}

// Re-enable warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
