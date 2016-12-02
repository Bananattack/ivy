#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ivy/image.h>
#include <ivy/reader.h>
#include <ivy/option_parser.h>

const char* const APPNAME = "ivy";

void fatal(const std::string& message) {
    std::fprintf(stderr, "%s: fatal: %s\n", APPNAME, message.c_str());
    std::exit(1);
}

int main(int argc, char** argv) {
    enum class OptionType {
        None,
        Output,
        Format,
    };

    auto parser = ivy::OptionParser<OptionType> {
        {OptionType::Output, "output", 'o', true},
        {OptionType::Format, "format", true},
    };

    if(!parser.parse(argc, argv)) {
        fatal(parser.getError());
    }

    std::string output;
    std::string input;

    for(const auto& option : parser.getOptions())
    {
        if(option.type == OptionType::Output) {
            if(output.empty()) {
                output = option.value;
            } else {
                fatal("output filename was specified more than once.");
            }
        } else if(option.type == OptionType::Format) {            
        } else if(option.type == OptionType::None) {
            if(input.empty()) {
                input = option.value;
            }
            else {
                // TODO: Do we support multiple files? How would that even work.
                fatal("input filename was specified more than once.");
            }
        }
    }

    if(input.empty()) {
        fatal("no input given.");
    }

    // TODO: Auto-detect output extension from input.
    // .png/.gif -> .chr
    // .chr -> .png

    // TODO: detect palette from indexed png/gif


    // Desired features:
    // Tileset:
    //      - translate a paletted image into an image into CHR format.
    //      - extract palettes from an image.
    //      - translate a specially formatted image into a palette.
    //      - translate 
    //
    // Background Maps:
    //      - raw tile mode: digest an image into unique tiles and palettes. emit tileset + palette set + tilemap + attribute map (choose which files you want, their formats and their paths)
    //      - metatile mode: generate unique metatile areas over these primitive layers for better compressed maps.
    //      - metatile mode (NES support): support attributes being specified per-metatile instead of tile.
    //
    // Sprites:
    //      - Single frames to metasprites
    //      - GIF to metasprites (local metasprites, or global) + palette data
    //      - local palettes or global palette.
    //      - number of maximum sprite layers (1 = no layering because only a single layer)
    //      - option for layer alignment (original GB has weird x-sort if two overlapping sprites don't completely align).
    //      - 8x8 or 8x16, etc, as appropriate for the console targetted.
    //
    // Reversal: (lower priority, maybe we can assume you always have the originals)
    //      - convert CHR back into bitmap image. (optional palette)
    //      - convert metatile map + tilemap + attributes (optional palette)
    //      - convert metsprites -> png or GIF

    auto reader = std::make_shared<ivy::FileReader>(input);
    auto image = ivy::loadImage(reader);
    if(!image) {
        fatal(std::string() + "input file '" + input + "' could not be opened.");
    }

    std::cout << "Width: " << image->getWidth() << std::endl;
    std::cout << "Height: " << image->getHeight() << std::endl;

    image->convertToIndexed();

    std::cout << "Palette: " << image->getPaletteSize() << std::endl;
    for(std::size_t i = 0, size = image->getPaletteSize(); i != size; ++i) {
        std::cout << "Color " << i << ": #" << std::hex << image->getPaletteColor(i) << std::endl;
    }

    return 0;
}