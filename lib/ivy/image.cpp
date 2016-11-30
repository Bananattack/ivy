#include <iostream>

#include <ivy/image.h>
#include <ivy/reader.h>

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #define STBI_ONLY_PNG
    #define STBI_ONLY_GIF
    #define STBI_ONLY_BMP
    #define STBI_NO_SIMD
    #include <stb_image.h>
}

namespace ivy {
    Image::Image()
    : width(0), height(0) {}

    Image::Image(std::size_t width, std::size_t height)
    : width(width), height(height), canvas(width * height) {}

    Image::~Image() {}

    std::size_t Image::getWidth() const {
        return width;
    }

    std::size_t Image::getHeight() const {
        return height;
    }

    std::uint32_t* Image::getRawCanvas() {
        return canvas.data();
    }

    const std::uint32_t* Image::getRawCanvas() const {
        return canvas.data();
    }

    std::uint32_t Image::getPixel(std::size_t x, std::size_t y) const {
        if(x < width || y < height) {
            return canvas[y * width + x];
        } else {
            return 0;
        }
    }

    void Image::setPixel(std::size_t x, std::size_t y, std::uint32_t value) {
        if(x < width || y < height) {
            canvas[y * width + x] = value;
        }
    }

    void Image::resizeCanvas(std::size_t width, std::size_t height) {
        std::vector<uint32_t> canvas(width * height);
        
        for(std::size_t i = 0; i != this->height; ++i) {
            for(std::size_t j = 0; j != this->width; ++j) {
                canvas[i * width + j] = this->canvas[i * this->width + j];
            }
        }

        this->width = width;
        this->height = height;
        this->canvas = std::move(canvas);
    }

    bool Image::isIndexed() const {
        return palette.size() != 0;
    }

    bool Image::isRGBA() const {
        return !isIndexed();
    }

    std::size_t Image::getPaletteSize() const {
        return palette.size();
    }

    std::uint32_t Image::getPaletteColor(std::size_t index) const {
        if(index < palette.size()) {
            return palette[index];
        }
        return 0;
    }

    void Image::setPaletteColor(std::size_t index, std::uint32_t color) {
        if(index < palette.size()) {
            palette[index] = color;
        }
    }

    void Image::resizePalette(std::size_t size) {
        palette.resize(size);
    }

    void Image::convertToIndexed() {
        if(isIndexed()) {
            return;
        }

        for(std::size_t i = 0, size = width * height; i != size; ++i) {
            auto color = canvas[i];
            auto it = std::lower_bound(palette.begin(), palette.end(), color);
            if(it == palette.end() || *it != color) {
                palette.insert(it, color);
            }
        }
    }

    void Image::convertToRGBA() {
        if(isRGBA()) {
            return;
        }
        for(std::size_t i = 0, size = width * height; i != size; ++i) {
            canvas[i] = getPaletteColor(canvas[i]);
        }
        palette.clear();
    }

    std::shared_ptr<Image> loadImage(const std::shared_ptr<Reader>& reader) {
        if(!reader || !reader->isOpen()) {
            return nullptr;
        }

        stbi_io_callbacks callbacks = {
            // read
            [](void* user, char* data, int size) {
                return static_cast<int>(static_cast<Reader*>(user)->read(data, static_cast<std::size_t>(size)));
            },
            // skip
            [](void* user, int n) {
                static_cast<Reader*>(user)->skip(n);
            },
            // eof
            [](void* user) {
                return static_cast<int>(static_cast<Reader*>(user)->eof());
            },
        };

        int x, y, comp;
        if(auto data = stbi_load_from_callbacks(&callbacks, reader.get(), &x, &y, &comp, 4)) {
            auto result = std::make_shared<Image>(static_cast<std::size_t>(x), static_cast<std::size_t>(y));
            auto source = static_cast<std::uint8_t*>(data);
            auto dest = result->getRawCanvas();
            
            for(std::size_t i = 0, size = result->getWidth() * result->getHeight(); i != size; ++i) {
                std::size_t k = i * 4;
                dest[i] = source[k] | (source[k + 1] << 8) | (source[k + 2] << 16) | (source[k + 3] << 24);
            }

            stbi_image_free(data);
            return result;
        }

        return nullptr;
    }
}