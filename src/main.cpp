#include <cstdint>
#include <array>
#include <valarray>
#include <string>
#include <chrono>
#include <iostream>
#include <memory>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "mandelbrot_ispc.h"
#include "mandelbrot_task_ispc.h"

#include "benchmark.h"
#include "image.h"

constexpr int MAX_ITERATIONS = 100;

void mandelbrot_cpp(image& img) {
	// (-2, 0.47) x (-1.12, 1.12)
	// (https://en.wikipedia.org/wiki/Mandelbrot_set)
	float const x_min = -2.0;
	float const x_max = 0.47;
	float const y_min = -1.12;
	float const y_max = 1.12;

	for (int x = 0; x < img.s_width; x++) {
		for (int y = 0; y < img.s_height; y++) {
			float const x_normalized = static_cast<float>(x) / (img.s_width - 1);
			float const x_0 = lerp(x_min, x_max, x_normalized);

			float const y_normalized = static_cast<float>(y) / (img.s_height - 1);
			float const y_0 = lerp(y_min, y_max, y_normalized);

			float x_ = 0.0;
			float y_ = 0.0;

			int iteration = 0;

			while (iteration < MAX_ITERATIONS && x_*x_ + y_*y_ < 4) {
				auto const tmp = x_*x_ - y_*y_ + x_0;
				y_ = 2*x_*y_ + y_0;
				x_ = tmp;
				iteration++;
			}

			if (iteration == MAX_ITERATIONS) {
				img.set(x, y, 255);
			}
		}
	}
}

void mandelbrot_valarray(image& img) {
	float const x_min = -2.0;
	float const x_max = 0.47;
	float const y_min = -1.12;
	float const y_max = 1.12;

	std::valarray<float> cs_real(img.s_width * img.s_height); // fun exercise: try to use {} syntax for ctor
	std::valarray<float> cs_imag(img.s_width * img.s_height);

	for (int x = 0; x < img.s_width; x++) {
		for (int y = 0; y < img.s_height; y++) {
			float const x_normalized = static_cast<float>(x) / (img.s_width - 1);
			cs_real[y * img.s_width + x] = lerp(x_min, x_max, x_normalized);
			float const y_normalized = static_cast<float>(y) / (img.s_height - 1);
			cs_imag[y * img.s_width + x] = lerp(y_min, y_max, y_normalized);
		}
	}

	std::valarray<float> zs_real(img.s_width * img.s_height);
	std::valarray<float> zs_imag(img.s_width * img.s_height);
	//zs_real = 0;
	//zs_imag = 0;

	std::valarray<float> tmp_real(img.s_width * img.s_height);
	std::valarray<float> tmp_imag(img.s_width * img.s_height);

	std::valarray<uint8_t> bitmap(img.s_width * img.s_height);
	bitmap = 255;

	for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
		// zs = zs**2 + cs
		tmp_real = zs_real * zs_real - zs_imag * zs_imag + cs_real;
		tmp_imag = 2 * zs_real * zs_imag + cs_imag;

		zs_real = tmp_real;
		zs_imag = tmp_imag;

		bitmap[(zs_real * zs_real + zs_imag * zs_imag) > 4] = 0;
	}

	std::copy(std::begin(bitmap), std::end(bitmap), std::begin(img));
}


int main() {
	{
		benchmark b{"DEFAULT"};
		image img{}; // fun experiment: Remove "{}" and look at the resulting image.
	
		mandelbrot_cpp(img);
		stbi_write_png("output.png", img.s_width, img.s_height, 1, img.data(), img.s_width );
	}

	{
		benchmark b{"VALARRAY"};

		image img{};
		mandelbrot_valarray(img);
		stbi_write_png("valarray.png", img.s_width, img.s_height, 1, img.data(), img.s_width );
	}

	{
		benchmark b{"ISPC"};

		auto img = std::make_unique<Image<int, 1600, 1200>>();
		ispc::mandelbrot_ispc(-2.0, 0.47, -1.12, 1.12, img->s_width, img->s_height, MAX_ITERATIONS, &(*img)[0]);
		image converted_img{};
		std::copy(std::begin(*img), std::end(*img), std::begin(converted_img));

		stbi_write_png("ispc.png", img->s_width, img->s_height, 1, converted_img.data(), img->s_width );
	}

	{
		benchmark b{"ISPC w/ tasks"};

		auto img = std::make_unique<Image<int, 1600, 1200>>();
		ispc::mandelbrot_task_ispc(-2.0, 0.47, -1.12, 1.12, img->s_width, img->s_height, MAX_ITERATIONS, &(*img)[0]);
		image converted_img{};
		std::copy(std::begin(*img), std::end(*img), std::begin(converted_img));

		stbi_write_png("ispc_task.png", img->s_width, img->s_height, 1, converted_img.data(), img->s_width );
	}
}
