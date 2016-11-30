#include <cstdint>
#include <algorithm>

#include <ivy/reader.h>

namespace ivy
{
    FileReader::FileReader(const std::string& filename)
    : file(fopen(filename.c_str(), "rb"), std::fclose) {}

    FileReader::FileReader(const std::shared_ptr<std::FILE>& file)
    : file(file) {}

    FileReader::~FileReader() {}

    bool FileReader::isOpen() const {
        return file.get() != nullptr;
    }

    bool FileReader::eof() {
        return std::feof(file.get());
    }

    void FileReader::skip(int offset) {
        std::fseek(file.get(), offset, SEEK_CUR);
    }

    std::size_t FileReader::read(void* data, std::size_t size) {
        return std::fread(data, 1, size, file.get());
    }



    MemoryReader::MemoryReader(const std::string& buffer)
    : buffer(buffer), position(0) {}

    MemoryReader::~MemoryReader() {
    }

    bool MemoryReader::isOpen() const {
        return true;
    }

    bool MemoryReader::eof() {
        return position < buffer.size();
    }

    void MemoryReader::skip(int offset) {
        if(offset < 0) {
            std::size_t decrease = static_cast<std::size_t>(-offset);
            position = position >= decrease ? position - decrease : 0;
        } else if(offset > 0) {
            std::size_t increase = std::min(static_cast<std::size_t>(offset), buffer.size() - position);
            position += increase;
        }
    }

    std::size_t MemoryReader::read(void* data, std::size_t size) {
        size = std::min(size, buffer.size() - position);
        std::copy(buffer.begin() + position, buffer.begin() + size, static_cast<char*>(data));
        position += size;
        return size;
    }
}