#pragma once
#include <filesystem>

namespace Engine
{ 

    class RawTexture
    {
    public:
        RawTexture(const std::filesystem::path& filepath);
        ~RawTexture();

        int IsHDR() const { return hdr_; }
        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }
        void* GetData() const { return data_; }
        uint32_t GetRowPitch() const { return rowPitch_; }

    private:
        bool hdr_;
        int width_;
        int height_;
        int rowPitch_;
        void* data_ = nullptr;
    };

}

