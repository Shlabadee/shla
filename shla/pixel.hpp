#ifndef __SHLA_PIXEL_INCLUDED
#define __SHLA_PIXEL_INCLUDED

#include <algorithm>

#include <cmath>
#include <cstdint>

#include <shla/math.hpp>

namespace shla
{
	struct pixel
	{
	public:
		uint8_t r, g, b, a;

		pixel()
		{
			r = 0;
			g = 0;
			b = 0;
			a = 255;
		}

		void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
		{
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}

		void set(uint8_t _r, uint8_t _g, uint8_t _b)
		{
			this->set(_r, _g, _b, 255);
		}

		void set(pixel &p)
		{
			r = p.r;
			g = p.g;
			b = p.b;
			a = p.a;
		}

		pixel operator=(pixel *p)
		{
			r = p->r;
			g = p->g;
			b = p->b;
			a = p->a;

			return *this;
		}

		pixel operator=(const pixel p)
		{
			r = p.r;
			g = p.g;
			b = p.b;
			a = p.a;

			return *this;
		}

		void blend(pixel &p)
		{
			double a2 = static_cast<double>(p.a) / 255;

			r = static_cast<uint8_t>((r * a2) + (p.r * (1 - a2)));
			g = static_cast<uint8_t>((g * a2) + (p.g * (1 - a2)));
			b = static_cast<uint8_t>((b * a2) + (p.b * (1 - a2)));
			a = 255;
		}

		void new_blend(pixel &p)
		{
			float _r1, _g1, _b1, _a1, _r2, _g2, _b2, _a2, _ao;
			_r1 = static_cast<float>(this->r);
			_g1 = static_cast<float>(this->g);
			_b1 = static_cast<float>(this->b);
			_a1 = static_cast<float>(this->a);
			_r2 = static_cast<float>(p.r);
			_g2 = static_cast<float>(p.g);
			_b2 = static_cast<float>(p.b);
			_a2 = static_cast<float>(p.a);

			_ao = _a1 + (_a2 * (1 - _a1));

			r = static_cast<uint8_t>(((_r1 * _a1) + ((_r2 * _a2) * (1 - _a1))) / _ao);
			g = static_cast<uint8_t>(((_g1 * _a1) + ((_g2 * _a2) * (1 - _a1))) / _ao);
			b = static_cast<uint8_t>(((_b1 * _a1) + ((_b2 * _a2) * (1 - _a1))) / _ao);
		}

		void fromHSV(double H, double S, double V)
		{
			if (H > 360 || H < 0 || S > 100 || S < 0 || V > 100 || V < 0)
			{
				r = 255;
				g = 0;
				b = 255;
			}
			double s = S / 100;
			double v = V / 100;
			double C = s * v;
			double X = C * (1 - fabs(shla::fmod(H / 60.0, 2) - 1));
			double m = v - C;
			double rt,
			gt,
			bt;
			if (H >= 0 && H < 60)
			{
				rt = C,
				gt = X,
				bt = 0;
			}
			else if (H >= 60 && H < 120)
			{
				rt = X,
				gt = C,
				bt = 0;
			}
			else if (H >= 120 && H < 180)
			{
				rt = 0,
				gt = C,
				bt = X;
			}
			else if (H >= 180 && H < 240)
			{
				rt = 0,
				gt = X,
				bt = C;
			}
			else if (H >= 240 && H < 300)
			{
				rt = X,
				gt = 0,
				bt = C;
			}
			else
			{
				rt = C,
				gt = 0,
				bt = X;
			}
			r = static_cast<uint8_t>((rt + m) * 255);
			g = static_cast<uint8_t>((gt + m) * 255);
			b = static_cast<uint8_t>((bt + m) * 255);
		}

		double* toHSV()
		{
			double rt, gt, bt;

			// Normalize RGB values
			rt = static_cast<double>(r) / 255.0;
			gt = static_cast<double>(g) / 255.0;
			bt = static_cast<double>(b) / 255.0;

			double cmax = std::max(rt, std::max(gt, bt));
			double cmin = std::min(rt, std::min(gt, bt));
			double difference = cmax - cmin;
			double h = -1, s = -1;

			// Calculate hue
			if (cmax == cmin)
			{
				h = 0;
			}
			else if (cmax == rt)
			{
				h = fmod(60 * ((gt - bt) / difference) + 360, 360);
			}
			else if (cmax == gt)
			{
				h = fmod(60 * ((bt - rt) / difference) + 120, 360);
			}
			else if (cmax == bt)
			{
				h = fmod(60 * ((rt - gt) / difference) + 240, 360);
			}

			// Calculate saturation
			if (cmax == 0)
			{
				s = 0;
			}
			else
			{
				s = (difference / cmax) * 100;
			}

			// Calculate value
			double v = cmax * 100;

			// Place data in array and return
			static double hsv[3];
			hsv[0] = h;
			hsv[1] = s;
			hsv[2] = v;
			return hsv;
		}
	};
}
#endif
