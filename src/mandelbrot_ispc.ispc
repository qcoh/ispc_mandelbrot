static int mandel(float c_re, float c_im, int count) {
    float z_re = 0.0f;
    float z_im = 0.0f;

    int i;
    for (i = 0; i < count; ++i) {
        if ((z_re * z_re + z_im * z_im) > 4.) {
            break;
        }

        float new_re = z_re*z_re - z_im*z_im + c_re;
        float new_im = 2.f * z_re * z_im + c_im;

        z_re = new_re;
        z_im = new_im;
    }

    return i;
}

static int pixel_value(int input_) {
	if (input_ == 0) {
		return 0;
	}
	return 255;
}

static float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

export void mandelbrot_ispc(uniform float x_min, uniform float x_max,
                            uniform float y_min, uniform float y_max,
                            uniform int width, uniform int height,
                            uniform int maxIterations,
                            uniform int output[])
{
    for (uniform int j = 0; j < height; j++) {
        foreach (i = 0 ... width) {
            float x_normalized = ((float)i)/(width-1);
            float x = lerp(x_min, x_max, x_normalized);

            float y_normalized = ((float)j)/(height-1);
            float y = lerp(y_min, y_max, y_normalized);

            int index = j * width + i;
            output[index] = mandel(x, y, maxIterations);
        }
    }

    foreach (i = 0 ... width * height) {
        if (output[i] != maxIterations) {
            output[i] = 0;
        } else {
            output[i] = 255;
        }
    }
}
