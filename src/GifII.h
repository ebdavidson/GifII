#pragma once

#include <vector>
#include <EasyGifReader/EasyGifReader.h>

class GifII
{

public:
    // primary class method, converts GIF input into Apple II animation disk
    void createAnimation();

private:
    // used for representing basic RGB colors
    struct Color
    {
        int red;
        int green;
        int blue;

        Color(int r, int g, int b);

        // Calculates the distance between the color and another color, used for finding closest palette match
        double getDistanceFromColor(const Color &c);
    };

    struct FrameData
    {
        int width;
        int height;
        std::vector<GifII::Color> pixels;

        FrameData(int w, int h, const EasyGifReader::PixelComponent *easyGifReaderPixels);

        void resizeFrame(int newWidth, int newHeight);
        std::vector<unsigned char> getLoResPixelData(const std::vector<GifII::Color> &palette);
    };

    void writeAppleIIPixelsToSlo(std::vector<unsigned char> pixels, int fileIndex);

    // These color values taken from this Wikipedia article: https://en.wikipedia.org/wiki/Apple_II_graphics
    // May not be perfect but close enough to approximate color distance
    const std::vector<GifII::Color> APPLE_II_PALETTE = {
        Color(0, 0, 0),       // BLACK
        Color(176, 2, 88),    // MAGENTA
        Color(30, 43, 255),   // DARK BLUE
        Color(206, 45, 255),  // PURPLE
        Color(0, 126, 40),    // DARK GREEN
        Color(128, 128, 128), // GRAY 1
        Color(0, 169, 255),   // MEDIUM BLUE
        Color(158, 171, 255), // LIGHT BLUE
        Color(97, 84, 0),     // BROWN
        Color(255, 86, 0),    // ORANGE
        Color(128, 128, 128), // GRAY 2
        Color(255, 129, 215), // PINK
        Color(49, 210, 0),    // LIGHT GREEN
        Color(225, 212, 0),   // YELLOW
        Color(79, 253, 167),  // AQUAMARINE
        Color(255, 255, 255)  // WHITE
    };
};