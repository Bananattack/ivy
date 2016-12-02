#ifndef IVY_READER_H
#define IVY_READER_H

#include <cstdio>
#include <memory>
#include <string>

namespace ivy {
    class Reader {
        public:
            virtual ~Reader() {}

            virtual bool isOpen() const = 0;
            virtual bool eof() = 0;
            virtual void skip(int offset) = 0;
            virtual std::size_t read(void* data, std::size_t size) = 0;
    };

    class FileReader : public Reader {
        public:
            FileReader(const std::string& filename);
            FileReader(const std::shared_ptr<std::FILE>& source);
            virtual ~FileReader();

            virtual bool isOpen() const;
            virtual bool eof();
            virtual void skip(int offset);
            virtual std::size_t read(void* data, std::size_t size);

        private:
            std::shared_ptr<std::FILE> file;
    };

    class MemoryReader : public Reader {
        public:
            MemoryReader(const char* buffer);
            MemoryReader(const std::string& buffer);
            virtual ~MemoryReader();
            
            virtual bool isOpen() const;
            virtual bool eof();
            virtual void skip(int offset);
            virtual std::size_t read(void* data, std::size_t size);

        private:
            std::string buffer;
            size_t position;
    };
}

#endif