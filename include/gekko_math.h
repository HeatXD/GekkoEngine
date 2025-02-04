#pragma once

#include <cstdint>

namespace Gekko::Math {

	struct Unit {
	private:
		int32_t _raw;

	public:
		// Unit range: -131071 to 131071 (km/m/cm/unit)
		static constexpr int32_t ONE = 0x4000;
		static constexpr int32_t MAX = 131071;
		static constexpr int32_t MIN = -131071;

		Unit() : _raw(0) {}
		Unit(int32_t val) : _raw(val) {}
		Unit(const Unit& val) : _raw(val._raw) {}

		// create unit from integer with scaling
		inline static Unit FromInt(int32_t val) {
			return Unit(val * ONE);
		}

		inline Unit operator+(const Unit& other) const {
			return Unit(_raw + other._raw);
		}

		inline Unit operator-(const Unit& other) const {
			return Unit(_raw - other._raw);
		}

		inline Unit operator/(const Unit& other) const {
			if (other._raw == 0) {
				throw std::runtime_error("Division by zero");
			}
			return Unit((static_cast<int64_t>(_raw) * ONE + (other._raw / 2)) / other._raw);
		}

		inline Unit operator*(const Unit& other) const {
			// implement
		}

		inline bool operator==(const Unit& other) const {
			return _raw == other._raw;
		}

		// FOR VISUALIZATION PURPOSES ONLY DON'T COMPUTE CRITICAL BEHAVIOUR WITH THIS VALUE
		inline float AsFloat() const {
			return static_cast<float>(_raw) / ONE;
		}
	};

	struct Vec3 {
		Unit x, y, z;
	};
}

