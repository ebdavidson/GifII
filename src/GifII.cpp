#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdint>
#include <filesystem>

extern "C"
{
#include <Cadius/Dc_Prodos.h>
#include <Cadius/Prodos_Add.h>
}

#include "GifII.h"

std::string GifII::createAnimation(std::string gifPath)
{
    std::filesystem::create_directories(TEMP_DIRECTORY);

    EasyGifReader gif = EasyGifReader::openFile(gifPath.c_str());

    // reduce to target number of frames, and update frame delay times
    int frameModToKeep = 1; // never skip if less than max frames
    if (gif.frameCount() > MAX_NUM_FRAMES)
    {
        frameModToKeep = ceil(1.0 / ((MAX_NUM_FRAMES * 1.0) / gif.frameCount()));
    }

    // loop through each frame and get Apple II image
    int frameIndex = 0;
    std::vector<short> frameDelaysMs = {};
    for (const EasyGifReader::Frame &easyGifReaderFrame : gif)
    {
        int currDelay = easyGifReaderFrame.delay * 10 / MS_PER_LOOP;
        if (currDelay > 32767)
            currDelay = 32767; // Clamp to max signed 16-bit value
        if (frameIndex % frameModToKeep == 0)
        {
            frameDelaysMs.push_back((short)currDelay);

            GifII::FrameData frame(easyGifReaderFrame.width(), easyGifReaderFrame.height(), easyGifReaderFrame.pixels());

            // reduce to Apple II lo-res size
            frame.resizeFrame(APPLEII_WIDTH, APPLEII_HEIGHT);

            // get pixels in Apple II lo-res format
            std::vector<unsigned char> gifAppleIIPixels = frame.getLoResPixelData(GifII::APPLE_II_PALETTE);

            // write to .slo file
            writeAppleIIPixelsToSlo(gifAppleIIPixels, frameDelaysMs.size() - 1);
        }
        else if (frameIndex > 0)
        {
            // skip this frame, add its delay to previous frame's delay
            frameDelaysMs[frameDelaysMs.size() - 1] += currDelay;
            if (frameDelaysMs[frameDelaysMs.size() - 1] > 32767)
                frameDelaysMs[frameDelaysMs.size() - 1] = 32767;
        }

        frameIndex++;
    }

    // generate delays binary file
    writeDelayBinary(frameDelaysMs);

    // write all files to Apple II disk image
    std::string outPath = createProDosDisk(frameDelaysMs.size(), gifPath);

    // cleanup temp files
    std::filesystem::remove_all(TEMP_DIRECTORY);

    return outPath;
}

GifII::FrameData::FrameData(int w, int h, const EasyGifReader::PixelComponent *easyGifReaderPixels)
{
    this->width = w;
    this->height = h;

    std::vector<GifII::RGBColor> colorPixels = {};

    // EasyGifReader stores each pixel as a 32-byte RGBA value; we can disregard the A value (which just indicates transparency) for this simple approach
    for (int i = 0; i < (width * height * 4); i += 4)
    {
        colorPixels.push_back(
            GifII::RGBColor(
                (int)easyGifReaderPixels[i + 0],
                (int)easyGifReaderPixels[i + 1],
                (int)easyGifReaderPixels[i + 2]));
    }

    this->pixels = colorPixels;
}

void GifII::FrameData::resizeFrame(int newWidth, int newHeight)
{
    // resizes to Apple II image size using nearest-neighbor scaling
    double xScalingRatio = (width * 1.0) / newWidth;
    double yScalingRatio = (height * 1.0) / newHeight;

    std::vector<GifII::RGBColor> newPixels = {};
    for (int r = 0; r < newHeight; r++)
    {
        for (int c = 0; c < newWidth; c++)
        {
            int rOld = std::round(r * yScalingRatio);
            int cOld = std::round(c * xScalingRatio);

            newPixels.push_back(this->pixels.at((rOld * width) + cOld));
        }
    }

    this->pixels = newPixels;
    this->width = newWidth;
    this->height = newHeight;
}

std::vector<unsigned char> GifII::FrameData::getLoResPixelData(const std::vector<GifII::RGBColor> &palette)
{
    std::vector<unsigned char> appleIIPixels = {};
    std::vector<unsigned char> currentTwoRowsPixels = {};

    // pixels are stored as vertically stacked four-bit values in binary, so we have to do two rows at a time
    for (int r = 0; r < 16; r += 2)
    {
        for (int c = 0; c < 40; c++)
        {
            // top third row
            int firstPixel = pixels[(r * 40) + c].getClosestAppleIIPaletteIndex();
            int secondPixel = pixels[((r + 1) * 40) + c].getClosestAppleIIPaletteIndex();
            unsigned char topThirdByte = (secondPixel << 4) + firstPixel;
            appleIIPixels.push_back(topThirdByte);
        }

        for (int c = 0; c < 40; c++)
        {
            // middle third row
            int thirdPixel = pixels[((r + 16) * 40) + c].getClosestAppleIIPaletteIndex();
            int fourthPixel = pixels[((r + 17) * 40) + c].getClosestAppleIIPaletteIndex();
            unsigned char middleThirdByte = (fourthPixel << 4) + thirdPixel;
            appleIIPixels.push_back(middleThirdByte);
        }

        for (int c = 0; c < 40; c++)
        {
            // bottom third row
            int fifthPixel = pixels[((r + 32) * 40) + c].getClosestAppleIIPaletteIndex();
            int sixthPixel = pixels[((r + 33) * 40) + c].getClosestAppleIIPaletteIndex();
            unsigned char bottomThirdByte = (sixthPixel << 4) + fifthPixel;
            appleIIPixels.push_back(bottomThirdByte);
        }

        // accounting for screen holes
        for (int c = 0; c < 8; c++)
        {
            appleIIPixels.push_back((unsigned char)0); // placeholder
        }
    }

    return appleIIPixels;
}

void GifII::writeAppleIIPixelsToSlo(std::vector<unsigned char> pixels, int fileIndex)
{
    // open file stream
    std::string fileName = TEMP_DIRECTORY + "X" + std::to_string(fileIndex) + ".SLO";
    std::ofstream stream(fileName, std::ios::out | std::ios::binary);

    // write each byte of two vertical pixels to file as raw binary
    for (unsigned char pixel : pixels)
    {
        stream << pixel;
    }

    // dispose of stream
    stream.close();
}

void GifII::writeDelayBinary(std::vector<short> frameDelaysMs)
{
    // open file stream
    std::string fileName = TEMP_DIRECTORY + "DELAY.SLO";
    std::ofstream stream(fileName, std::ios::out | std::ios::binary);

    // remove this to create delays suitable for emulating with enhanced read access
    // see my blog post for more info on why I effectively disabled delay logic but didn't remove it
    for (int i = 0; i < frameDelaysMs.size(); i++)
    {
        frameDelaysMs[i] = 1;
    }

    // add the number of frames to the beginning of the list for use in the BASIC program
    frameDelaysMs.insert(frameDelaysMs.begin(), frameDelaysMs.size());

    // write frame count and each two-byte delay value to file as raw binary
    // force big-endian so it's correct layout for Apple II
    for (short delay : frameDelaysMs)
    {
        unsigned char largeByte = delay / 256;
        unsigned char smallByte = delay % 256;
        stream << largeByte;
        stream << smallByte;
    }

    // dispose of stream
    stream.close();
}

std::string GifII::createProDosDisk(int frameCount, std::string sourcePath)
{
    // get name for disk
    int lastDelimiter = sourcePath.find_last_of("/");
    if (lastDelimiter >= 0)
    {
        sourcePath = sourcePath.substr(lastDelimiter + 1);
    }

    lastDelimiter = sourcePath.find_last_of(".");
    sourcePath = sourcePath.substr(0, lastDelimiter);
    sourcePath += ".po";

    // create a copy of template disk
    std::ifstream templateStream("GIFII_TEMPLATE_DISK.po", std::ios::binary | std::ios::in);
    std::ofstream diskStream(sourcePath, std::ios::binary | std::ios::out);
    diskStream << templateStream.rdbuf();
    templateStream.close();
    diskStream.close();

    // use DiskImg methods to load template ProDos disk and add our files
    std::string templateImgPath = sourcePath;
    prodos_image *img = LoadProdosImage(templateImgPath.data());

    std::string fileToAdd = "";
    std::string folder = "/PRODOS/";
    for (int i = 0; i < frameCount; i++)
    {
        fileToAdd = TEMP_DIRECTORY + "X" + std::to_string(i) + ".SLO";
        AddFile(img, fileToAdd.data(), folder.data(), 1);
    }
    fileToAdd = TEMP_DIRECTORY + "DELAY.SLO";
    AddFile(img, fileToAdd.data(), folder.data(), 1);

    return sourcePath;
}

GifII::RGBColor::RGBColor(int r, int g, int b)
{
    this->red = r;
    this->green = g;
    this->blue = b;
}

int GifII::RGBColor::getClosestAppleIIPaletteIndex()
{
    int closestColorIndex = 0;
    double closestColorDistance = 442.0; // higher than the hypothetical max distance of ~441.672 from white to black

    for (int j = 0; j < APPLE_II_PALETTE.size(); j++)
    {
        GifII::RGBColor appleIIColor = APPLE_II_PALETTE[j];
        double distance = this->getDistanceFromColor(appleIIColor);
        if (distance < closestColorDistance)
        {
            closestColorIndex = j;
            closestColorDistance = distance;
        }
    }
    return closestColorIndex;
}

double GifII::RGBColor::getDistanceFromColor(const RGBColor &c)
{
    // Euclidian distance algorithm

    double rDiff = (double)(c.red - this->red);
    double gDiff = (double)(c.green - this->green);
    double bDiff = (double)(c.blue - this->blue);

    return std::sqrt((rDiff * rDiff) + (gDiff * gDiff) + (bDiff * bDiff));
}