#pragma once

#include <cstdint>
#include <array>

using std::uint8_t;

template <typename T, int Width, int Height>
struct Image : public std::array<T, Width * Height> {
	void set(int x, int y, uint8_t c) noexcept {
		(*this)[y * Width + x] = c;
	}

	T const * data() const noexcept {
		return &(*this)[0];
	}

	static constexpr int s_width = Width;
	static constexpr int s_height = Height;
};

using image = Image<uint8_t, 1600, 1200>;

inline float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

