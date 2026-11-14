#pragma once

#include <vector>
#include <EasyGifReader/EasyGifReader.h>
#include <string>

class GifII
{

public:
    // primary class method, converts GIF input into Apple II animation disk
    std::string createAnimation(std::string gifPath);

private:
    const int APPLEII_WIDTH = 40;
    const int APPLEII_HEIGHT = 48;
    const int MAX_NUM_FRAMES = 47;
    const double MS_PER_LOOP = 9.63;
    const std::string TEMP_DIRECTORY = "./gifii_temp/";

    // used for representing basic RGB colors
    struct RGBColor
    {
        int red;
        int green;
        int blue;

        RGBColor(int r, int g, int b);

        // get closest Apple II color as its graphics index
        int getClosestAppleIIPaletteIndex();

        // get Euclidian distance from another color, used to find closest palette match
        double getDistanceFromColor(const RGBColor &c);
    };

    struct FrameData
    {
        int width;
        int height;
        std::vector<GifII::RGBColor> pixels;

        FrameData(int w, int h, const EasyGifReader::PixelComponent *easyGifReaderPixels);

        void resizeFrame(int newWidth, int newHeight);
        std::vector<unsigned char> getLoResPixelData(const std::vector<GifII::RGBColor> &palette);
    };

    void writeAppleIIPixelsToSlo(std::vector<unsigned char> pixels, int fileIndex);

    void writeDelayBinary(std::vector<short> frameDelaysMs);

    std::string createProDosDisk(int frameCount, std::string sourcePath);

    // These color values taken from this Wikipedia article: https://en.wikipedia.org/wiki/Apple_II_graphics
    // May not be perfect but close enough to approximate color distance
    const static inline std::vector<GifII::RGBColor> APPLE_II_PALETTE = {
        RGBColor(0, 0, 0),       // BLACK
        RGBColor(176, 2, 88),    // MAGENTA
        RGBColor(30, 43, 255),   // DARK BLUE
        RGBColor(206, 45, 255),  // PURPLE
        RGBColor(0, 126, 40),    // DARK GREEN
        RGBColor(128, 128, 128), // GRAY 1
        RGBColor(0, 169, 255),   // MEDIUM BLUE
        RGBColor(158, 171, 255), // LIGHT BLUE
        RGBColor(97, 84, 0),     // BROWN
        RGBColor(255, 86, 0),    // ORANGE
        RGBColor(128, 128, 128), // GRAY 2
        RGBColor(255, 129, 215), // PINK
        RGBColor(49, 210, 0),    // LIGHT GREEN
        RGBColor(225, 212, 0),   // YELLOW
        RGBColor(79, 253, 167),  // AQUAMARINE
        RGBColor(255, 255, 255)  // WHITE
    };
};