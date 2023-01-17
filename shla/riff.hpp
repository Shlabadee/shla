#ifndef __SHLA_RIFF_INCLUDED
#define __SHLA_RIFF_INCLUDED

#include <cstdint>

namespace shla
{
	class RIFF
	{
		protected:
			uint32_t riff_size;
		public:
			uint32_t GetRIFFSize()
			{
				return riff_size;
			}
	};
}

#endif