#ifndef __SHLA_RIFFWAVE_INCLUDED
#define __SHLA_RIFFWAVE_INCLUDED

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <cstdint>
#include <cstring>

#include <shla/riff.hpp>

namespace shla
{
	enum class WAVE_FMT : uint16_t
	{
		PCM = 1,
		IEEE_FLOAT_SINGLE_PRECISION = 3,
		INVALID = 666
	};

	class WAVE : public shla::RIFF
	{
		private:
			uint32_t fmt_size;
			uint32_t byte_rate;
			uint16_t block_align;
			uint32_t data_size;
			WAVE_FMT format;
			uint16_t bit_depth;
			uint32_t num_samples;
		public:
			uint16_t num_channels;
			uint32_t sample_rate;
			uint16_t cbSize;

			std::vector<double> left_channel;
			std::vector<double> right_channel;

			inline WAVE()
			{
				setFormat(WAVE_FMT::PCM);
				sample_rate = 44100;
				num_channels = 1;
			}

			inline void calculateHeader()
			{
				num_samples = left_channel.size();
				block_align = num_channels * (bit_depth / 8);
				byte_rate = sample_rate * block_align;
				data_size = num_samples * num_channels * (bit_depth / 8);
				fmt_size = sizeof(format) + sizeof(num_channels) + sizeof(sample_rate) + sizeof(byte_rate) + sizeof(block_align) + sizeof(bit_depth) + sizeof(cbSize);
				riff_size = 4 + (8 + fmt_size) + (8 + data_size);
				cbSize = 0;
			}

			inline void setFormat(WAVE_FMT fmt)
			{
				format = fmt;

				switch (format)
				{
					case WAVE_FMT::PCM:
						bit_depth = 16;
						break;

					case WAVE_FMT::IEEE_FLOAT_SINGLE_PRECISION:
						bit_depth = 32;
						break;

					default:
						std::cerr << "Unknown format (given format: " << static_cast<int>(format) << ")\n";
						std::terminate();
						break;
				}
			}

			inline WAVE_FMT getFormat()
			{
				return this->format;
			}

			inline void clear()
			{
				left_channel.clear();
				right_channel.clear();
			}

			inline bool read(std::string filename)
			{
				std::ifstream file;
				file.open(filename, std::ios::binary);
				char *buffer;

				if (file.fail())
				{
					std::cerr << "Failed to read '" << filename << "'.\n";
					return false;
				}

				// Make sure the file is a RIFF WAVE file.
				file.seekg(0);
				buffer = new char[4];

				file.read(buffer, 4);

				if (file.fail())
				{
					std::cerr << "Failed to read magic number.\n";
					delete[] buffer;
					return false;
				}

				if (strcmp(buffer, "RIFF") != 0)
				{
					std::cerr << "File is not RIFF. Buffer read: ";
					std::cerr.write(buffer, 4);
					std::cerr << std::endl;
					delete[] buffer;
					return false;
				}

				file.seekg(8);
				file.read(buffer, 4);

				if (file.fail())
				{
					std::cerr << "Failed to read WAVE.\n";
					delete[] buffer;
					return false;
				}

				if (strcmp(buffer, "WAVE") != 0)
				{
					std::cerr << "File is not WAVE. Buffer read: ";
					std::cerr.write(buffer, 4);
					std::cerr << std::endl;
					delete[] buffer;
					return false;
				}

				// Get audio format (PCM or float)
				file.seekg(20);
				{
					WAVE_FMT wave_fmt;
					file.read(reinterpret_cast<char*>(&wave_fmt), 2);

					if (file.fail())
					{
						std::cerr << "Failed to read for format.\n";
						delete[] buffer;
						return false;
					}

					this->setFormat(wave_fmt);
				}

				// Get number of channels
				file.seekg(22);
				file.read(reinterpret_cast<char*>(&num_channels), 2);

				if (file.fail())
				{
					std::cerr << "Failed to read for number of channels.\n";
					delete[] buffer;
					return false;
				}

				// Get sample rate
				file.seekg(24);
				file.read(reinterpret_cast<char*>(&sample_rate), 4);

				if (file.fail())
				{
					std::cerr << "Failed to read for sample rate.\n";
					delete[] buffer;
					return false;
				}

				// Search for data chunk
				uint32_t pos;
				for (pos = 36; true; ++pos)
				{
					file.seekg(pos);
					file.read(buffer, 4);

					if (file.fail())
					{
						std::cerr << "Failed to find data chunk.\n";
						delete[] buffer;
						return false;
					}

					if (strcmp(buffer, "data") == 0)
					{
						break;
					}
				}

				file.seekg(pos + 4);
				file.read(reinterpret_cast<char*>(&data_size), 4);

				if (file.fail())
				{
					std::cerr << "Failed to read data size.\n";
					delete[] buffer;
					return false;
				}

				clear();

				float fSample;
				int16_t iSample;
				double max16 = 32787;

				for (uint32_t i = 0; i < data_size;)
				{
					if (bit_depth == 16)
					{
						file.read(reinterpret_cast<char*>(&iSample), 2);

						if (file.fail())
						{
							std::cerr << "Failed to read for left/single channel.\n";
							return false;
						}

						left_channel.push_back(static_cast<double>(iSample) / max16);
						i += 2;

						if (num_channels == 2)
						{
							file.read(reinterpret_cast<char*>(&iSample), 2);

							if (file.fail())
							{
								std::cerr << "Failed to read for right channel.\n";
								return false;
							}

							right_channel.push_back(static_cast<double>(iSample) / max16);
							i += 2;
						}
					}
					else if (bit_depth == 32)
					{
						file.read(reinterpret_cast<char*>(&fSample), 4);

						if (file.fail())
						{
							std::cerr << "Failed to read for left/single channel.\n";
							return false;
						}

						left_channel.push_back(fSample);
						i += 4;

						if (num_channels == 2)
						{
							file.read(reinterpret_cast<char*>(&fSample), 4);

							if (file.fail())
							{
								std::cerr << "Failed to read for right channel.\n";
								return false;
							}

							right_channel.push_back(fSample);
							i += 4;
						}
					}
				}

				delete[] buffer;
				return true;
			}

			inline void write(std::string filename)
			{
				this->calculateHeader();
				std::ofstream file(filename, std::ios::binary);

				file.write("RIFF", 4);
				file.write(reinterpret_cast<char*>(&riff_size), sizeof(riff_size));
				file.write("WAVE", 4);

				file.write("fmt ", 4);
				file.write(reinterpret_cast<char*>(&fmt_size), sizeof(fmt_size));
				file.write(reinterpret_cast<char*>(&format), sizeof(format));
				file.write(reinterpret_cast<char*>(&num_channels), sizeof(num_channels));
				file.write(reinterpret_cast<char*>(&sample_rate), sizeof(sample_rate));
				file.write(reinterpret_cast<char*>(&byte_rate), sizeof(byte_rate));
				file.write(reinterpret_cast<char*>(&block_align), sizeof(block_align));
				file.write(reinterpret_cast<char*>(&bit_depth), sizeof(bit_depth));
				file.write(reinterpret_cast<char*>(&cbSize), sizeof(cbSize));

				if (bit_depth == 32)
				{
					file.write("fact", 4);
					uint32_t s = 4;
					file.write(reinterpret_cast<char*>(&s), sizeof(s));
					file.write(reinterpret_cast<char*>(&num_samples), sizeof(num_samples));
				}

				file.write("data", 4);
				file.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));

				double max16 = 32767;

				if (bit_depth == 16)
				{
					double a;
					int16_t c;

					for (size_t i = 0; i < left_channel.size(); ++i)
					{
						a = max16 * left_channel[i];

						if (a < -max16)
						{
							a = -max16;
						}
						else if(a > max16)
						{
							a = max16;
						}

						c = static_cast<int16_t>(a);
						file.write(reinterpret_cast<char*>(&c), sizeof(c));

						if (num_channels == 2)
						{
							a = max16 * right_channel[i];

							if (a < -max16)
							{
								a = -max16;
							}
							else if (a > max16)
							{
								a = max16;
							}

							c = static_cast<int16_t>(a);
							file.write(reinterpret_cast<char*>(&c), sizeof(c));
						}
					}
				}
				else
				{
					float a;
					for (size_t i = 0; i < left_channel.size(); ++i)
					{
						a = static_cast<float>(left_channel.at(i));
						file.write(reinterpret_cast<char*>(&a), sizeof(a));

						if (num_channels == 2)
						{
							a = static_cast<float>(right_channel.at(i));
							file.write(reinterpret_cast<char*>(&a), sizeof(a));
						}
					}
				}

				file.close();
			}
	};
}

#endif
