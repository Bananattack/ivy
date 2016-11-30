#ifndef IVY_IMAGE_H
#define IVY_IMAGE_H

#include <memory>
#include <vector>
#include <cstdint>
#include <cstdlib>

namespace ivy {
    class Reader;

    class Image {
        public:
            Image();
            Image(std::size_t width, std::size_t height);
            ~Image();

            std::size_t getWidth() const;
            std::size_t getHeight() const;
            std::uint32_t* getRawCanvas();
            const std::uint32_t* getRawCanvas() const;

            std::uint32_t getPixel(std::size_t x, std::size_t y) const;
            void setPixel(std::size_t x, std::size_t y, std::uint32_t value);
            void resizeCanvas(std::size_t width, std::size_t height);

            bool isIndexed() const;
            bool isRGBA() const;
            std::size_t getPaletteSize() const;
            std::uint32_t getPaletteColor(std::size_t index) const;
            void setPaletteColor(std::size_t index, std::uint32_t color);
            void resizePalette(std::size_t size);

            void convertToIndexed();
            void convertToRGBA();

        private:
            std::size_t width;
            std::size_t height;
            std::vector<std::uint32_t> canvas;
            std::vector<std::uint32_t> palette;
    };

    std::shared_ptr<Image> loadImage(const std::shared_ptr<Reader>& reader);
}

#endif
