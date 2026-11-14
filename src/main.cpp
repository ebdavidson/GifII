#include "GifII.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 2)
        {
            throw std::runtime_error("Must provide GIF filename");
        }

        std::string gifPath = argv[1];
        std::string fileEnding = gifPath.substr(gifPath.length() - 4);
        transform(fileEnding.begin(), fileEnding.end(), fileEnding.begin(), ::toupper);

        if (fileEnding.compare(".GIF") != 0)
        {
            throw std::runtime_error("Invalid path, file must be GIF");
        }

        if (!std::filesystem::exists(gifPath))
        {
            throw std::runtime_error("Invalid path, file does not exist");
        }

        GifII gifII = GifII();
        std::string diskName = gifII.createAnimation(gifPath);

        std::cout << "File generated: " << diskName << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}
