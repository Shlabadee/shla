#ifndef __SHLA_RIFFBTMP_INCLUDED
#define __SHLA_RIFFBTMP_INCLUDED

#include <fstream>
#include <vector>
#include <string>

#include <cstdint>

#include <shla/pixel.hpp>
#include <shla/riff.hpp>

namespace shla
{
    class BTMP : shla::RIFF
    {
        private:
            uint16_t fmt_size;
            uint8_t format;

            uint64_t data_size;
        public:
            std::vector<shla::pixel> image;
            uint32_t    width,
                        height;

            BTMP()
            {
                width = 800;
                height = 600;
                image.resize(width * height);
                this->setFormat(0);
            }

            BTMP(uint32_t _width, uint32_t _height)
            {
                width = _width;
                height = _height;
                image.resize(width * height);
                this->setFormat(0);
            }

            inline void calculateHeader()
            {
                data_size = image.size() * 4;
                fmt_size = sizeof(format) + sizeof(width) + sizeof(height);
                riff_size = fmt_size + data_size + sizeof(fmt_size) + sizeof(data_size) + 4 + 4 + 4;
            }

            void setFormat(uint8_t _format)
            {
                format = _format;
            }

            inline void write(std::string filename)
            {
                this->calculateHeader();
                std::ofstream file(filename, std::ios::binary);

                file.write("RIFF", 4);
                file.write(reinterpret_cast<char*>(&riff_size), sizeof(riff_size));

                file.write("BTMP", 4);

                file.write("fmt ", 4);
                file.write(reinterpret_cast<char*>(&fmt_size), sizeof(fmt_size));
                file.write(reinterpret_cast<char*>(&format), sizeof(format));
                file.write(reinterpret_cast<char*>(&width), sizeof(width));
                file.write(reinterpret_cast<char*>(&height), sizeof(height));

                file.write("data", 4);
                file.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));

                for (uint64_t i = 0; i < image.size(); ++i)
                {
                    file.write(reinterpret_cast<char*>(&image.at(i).r), sizeof(image.at(i).r));
                    file.write(reinterpret_cast<char*>(&image.at(i).g), sizeof(image.at(i).g));
                    file.write(reinterpret_cast<char*>(&image.at(i).b), sizeof(image.at(i).b));
                    file.write(reinterpret_cast<char*>(&image.at(i).a), sizeof(image.at(i).a));
                }
            }

            inline bool set(uint32_t x, uint32_t y, shla::pixel &_p)
            {
                uint32_t pos = (y * height) + x;

                if (pos > image.size())
                {
                    return false;
                }

                image.at(pos) = _p;

                return true;
            }

            inline shla::pixel* get(uint32_t x, uint32_t y)
            {
                uint32_t pos = (y * height) + x;

                if (pos > image.size())
                {
                    return 0;
                }

                return &image.at(pos);
            }
    };
}

#endif
