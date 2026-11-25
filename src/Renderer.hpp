#pragma once
#include <Geode/Geode.hpp>
#include <vector>
#include <cstdio>

using namespace geode::prelude;

class BotRenderer {
public:
    bool isRendering = false;
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixelBuffer;
#ifdef GEODE_IS_WINDOWS
    FILE* ffmpegPipe = nullptr;
#endif

    static BotRenderer& get() {
        static BotRenderer instance;
        return instance;
    }

    bool startRender() {
#ifdef GEODE_IS_WINDOWS
        if (isRendering) return false;

        auto glView = CCEGLView::get();
        auto frameSize = glView->getFrameSize();
        width = static_cast<int>(frameSize.width);
        height = static_cast<int>(frameSize.height);

        // Ensure even dimensions for libx264
        width -= width % 2;
        height -= height % 2;

        pixelBuffer.resize(width * height * 3);

        auto outPath = (Mod::get()->getSaveDir() / "render.mp4").string();
        auto cmd = fmt::format(
            "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s {}x{} -r 60 -i - "
            "-vf vflip -c:v libx264 -preset fast -crf 18 -pix_fmt yuv420p \"{}\"",
            width, height, outPath
        );

        ffmpegPipe = _popen(cmd.c_str(), "wb");
        if (!ffmpegPipe) {
            return false;
        }

        isRendering = true;
        return true;
#else
        return false; // Desktop FFmpeg piping is Windows-only
#endif
    }

    void captureFrame() {
#ifdef GEODE_IS_WINDOWS
        if (!isRendering || !ffmpegPipe) return;

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer.data());
        fwrite(pixelBuffer.data(), 1, pixelBuffer.size(), ffmpegPipe);
#endif
    }

    void stopRender() {
#ifdef GEODE_IS_WINDOWS
        if (!isRendering) return;
        isRendering = false;
        if (ffmpegPipe) {
            _pclose(ffmpegPipe);
            ffmpegPipe = nullptr;
        }
#endif
    }
};
