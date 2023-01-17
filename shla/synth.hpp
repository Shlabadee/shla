#ifndef __SHLA_SYNTH_INCLUDED
#define __SHLA_SYNTH_INCLUDED

#include <string>
#include <map>

#include <cmath>
#include <cstdint>

#include <shla/math.hpp>
#include <shla/rand.hpp>

namespace shla
{
	double pwm_width = 0.5;
	bool loop_mode = false, quantizize_tri = true, quantizize_sin = false;
	double tri_rounding = 1.0 / 8.0, sin_rounding = 1.0 / 16.0;
	uint16_t hold_counter = 0, hold_limit;
	double base_tune = 440;
	uint64_t sample = shla::rand();
	double harmonics = 50;
	std::map<std::string, uint16_t> pitch_map;

	inline double GetNoteFrequency(uint16_t note)
	{
		double a = (static_cast<double>(note) - 49.0) / 12.0;
		double b = pow(2, a);
		return b * base_tune;
	}

	inline double pwm(double x)
	{
		if (pwm_width <= 0 || pwm_width >= 1)
		{
			return 0.0;
		}

		x = fmod(x, shla::PI2);
		double tWidth = pwm_width * shla::PI2;
		if (x < tWidth)
		{
			return -1.0;
		}
		else
		{
			return 1.0;
		}
	}

	inline double square_fourier(double x)
	{
		double s;
		for (double n = 1; n <= harmonics; ++n)
		{
			s += 0.5 * (((1 - cos(n * shla::PI)) / n) * sin(n * x));
		}
		return s;
	}

	inline double saw(double x)
	{
		return fmod((static_cast<double>(x) / PI) + 1, 2) - 1;
	}

	inline double saw_fourier(double x)
	{
		double s = 0;
		for (double n = 1; n <= harmonics; ++n)
		{
			s += sin(n * (x + PI)) / n;
		}

		return (-PI / 5) * s;
	}

	inline double tri(double x)
	{
		double s;
		s = (shla::PIU2 * fabs(shla::fmod(x - shla::HPI, shla::PI2) - shla::PI)) - 1;

		if (quantizize_tri)
		{
			s = round(s / tri_rounding) * tri_rounding;
		}

		return s;
	}

	inline double sin(double x)
	{
		double s = ::sin(x);

		if (quantizize_sin)
		{
			s = round(s / sin_rounding) * sin_rounding;
		}

		return s;
	}

	inline double waveform(double frequency, uint32_t sample_rate, double phase, double (*wgen)(double))
	{
		double a = phase / static_cast<double>(sample_rate);
		double b = shla::PI2 * frequency;
		return wgen(b * a);
	}

	inline double noise(uint8_t hold_level)
	{
		if (sample == 0)
		{
			if (loop_mode)
			{
				rstate = 0xFBC0D13BBC13C685;
				sample = rand();
			}
			else
			{
				sample = rand();
				// Ensures that all 64 bits are used
				// before a new sample is generated.
				sample |= 0x8000000000000000;
			}
		}

		uint8_t bit = sample & 1;
		double c = bit == 0 ? -1.0 : 1.0;

		if (hold_level == 0)
		{
			sample = sample >> 1;
		}
		else
		{
			hold_limit = hold_level * 2;

			if (hold_counter >= hold_limit)
			{
				hold_counter = 0;
				sample = sample >> 1;
			}
			else
			{
				++hold_counter;
			}
		}

		return c;
	}

	inline double truenoise()
	{
		return (static_cast<double>(rand()) / static_cast<double>(0xffffffffffffffff)) - 0.5;
	}
}

#endif
