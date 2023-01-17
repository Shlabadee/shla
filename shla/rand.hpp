#ifndef __SHLA_RAND_INCLUDED
#define __SHLA_RAND_INCLUDED

#include <cstdint>
#include <ctime>

namespace shla
{
	uint64_t rstate;

	#ifdef SHLA_RAND_OLD_ALGO
	inline uint64_t rand()
	{
		rstate ^= rstate << 13;
		rstate ^= rstate >> 7;
		rstate ^= rstate << 17;
		return rstate;
	}
	#else
	inline uint64_t rand()
	{
		rstate ^= rstate >> 12;
		rstate ^= rstate << 25;
		rstate ^= rstate >> 27;
		return rstate * 0x2545F4914F6CDD1DULL;
	}
	#endif // SHLA_RAND_OLD_ALGO
	
	inline void initrstate(bool _usetime)
	{
		rstate = _usetime ? time(0) : 0xFBC0D13BBC13C685;
	}
	
	inline void initrstate()
	{
		initrstate(true);
	}
}

#endif