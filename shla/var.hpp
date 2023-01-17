#ifndef __SHLA_VAR_HPP
#define __SHLA_VAR_HPP

#include <sstream>
#include <string>
#include <exception>

#include <cstdint>

namespace shla
{
	enum class VAR_TYPE
	{
		BOOL,
		INTEGER,
		FLOAT,
		STRING,
		OBJECT
	};

	class var
	{
		private:
			std::string _data;
			shla::VAR_TYPE _vtype;
		public:
			var()
			{
				_data = "0";
				_vtype = shla::VAR_TYPE::INTEGER;
			}

			std::string get()
			{
				return _data;
			}

			shla::VAR_TYPE type()
			{
				return _vtype;
			}

			void set(bool b)
			{
				_data = b ? "true": "false";
			}

			void set(uint64_t i)
			{
				std::stringstream ss;
				ss << i;
				ss >> _data;
			}

			void set(double f)
			{
				std::stringstream ss;
				ss << f;
				ss >> _data;
			}

			void set(const char* c)
			{
				_data = std::string(c);
			}

			void set(std::string &s)
			{
				_data = s;
			}
	};
}
#endif
