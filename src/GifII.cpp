#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "GifII.h"

GifII::RGBColor::RGBColor(int r, int g, int b)
{
    this->red = r;
    this->green = g;
    this->blue = b;
}

void GifII::createAnimation()
{
    // TODO enable passing file path or file data somehow
    EasyGifReader gif = EasyGifReader::openFile("../testpattern_crop.gif");

    const EasyGifReader::Frame frame = gif.begin();
    const EasyGifReader::PixelComponent *framePixels = frame.pixels();

    // TODO reduce to target number of frames, and store frame delay times

    // loop through each frame and get Apple II image
    int frameIndex = 0;
    for (const EasyGifReader::Frame &easyGifReaderFrame : gif)
    {
        GifII::FrameData frame(easyGifReaderFrame.width(), easyGifReaderFrame.height(), easyGifReaderFrame.pixels());

        // reduce to Apple II lo-res size
        frame.resizeFrame(40, 48); // TODO make these const

        // get pixels in Apple II lo-res format
        std::vector<unsigned char> gifAppleIIPixels = frame.getLoResPixelData(GifII::APPLE_II_PALETTE);

        // TODO write to .slo file
        writeAppleIIPixelsToSlo(gifAppleIIPixels, frameIndex);

        frameIndex++;
    }

    // TODO generate assembly file

    // TODO write all files to Apple II disk image and return
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
    unsigned char currentPixel = 0;

    // for each color, find the closest color in the Apple II palette
    for (int i = 0; i < this->pixels.size(); i++)
    {
        RGBColor color = this->pixels[i];
        int closestColorIndex = 0;
        double closestColorDistance = 442.0; // higher than the hypothetical max distance of ~441.672 from white to black

        for (int j = 0; j < palette.size(); j++)
        {
            GifII::RGBColor appleIIColor = palette[j];
            double distance = color.getDistanceFromColor(appleIIColor);
            if (distance < closestColorDistance)
            {
                closestColorIndex = j;
                closestColorDistance = distance;
            }
        }

        // color index is stored in file as a four-bit value, so we can do two colors at a time
        // and write them as one 8-bit unsigned char value to the file
        if (i % 2 == 1)
        {
            // second value
            currentPixel += closestColorIndex;
            appleIIPixels.push_back(currentPixel);
        }
        else
        {
            // first value
            currentPixel = closestColorIndex;
            currentPixel <<= 4;
        }
    }

    return appleIIPixels;
}

void GifII::writeAppleIIPixelsToSlo(std::vector<unsigned char> pixels, int fileIndex)
{
    // open file stream
    std::string fileName = std::to_string(fileIndex) + ".SLO";
    std::ofstream stream(fileName, std::ios::out | std::ios::binary);

    // write each two-pixel byte to file as raw binary
    for (unsigned char pixel : pixels)
    {
        stream << pixel;
    }

    // dispose of stream
    stream.close();
}

double GifII::RGBColor::getDistanceFromColor(const RGBColor &c)
{
    // Euclidian distance algorithm

    double rDiff = (double)(c.red - this->red);
    double gDiff = (double)(c.green - this->green);
    double bDiff = (double)(c.blue - this->blue);

    return std::sqrt((rDiff * rDiff) + (gDiff * gDiff) + (bDiff * bDiff));
}