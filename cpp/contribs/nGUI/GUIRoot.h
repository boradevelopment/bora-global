// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: GUIRoot.h
 * Purpose: Root of GUI base classes
 */

#pragma once
#include <memory>
#include "skia/interfaces/IGUIRenderer.h"

class GUIRoot {
public:
    GUIRoot(std::unique_ptr<IGUIRenderer> renderer)
          : renderer(std::move(renderer)) {}

    void render(int width, int height) {
        SkCanvas* canvas = renderer->beginFrame(width, height);

        renderer->endFrame();
    }

private:
    std::unique_ptr<IGUIRenderer> renderer;
};
