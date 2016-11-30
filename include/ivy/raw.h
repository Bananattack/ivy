#ifndef IVY_CHR_H
#define IVY_CHR_H

namespace ivy
{
    enum class RawFormatType {};

    bool loadRawImage(const std::shared_ptr<Reader>& reader, RawFormatType type);
}
#endif