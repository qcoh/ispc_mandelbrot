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

static float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

task void mandelbrot_scanline(uniform float x_min, uniform float x_max,
                                uniform float y_min, uniform float y_max,
                                uniform int width, uniform int height,
                                uniform int span,
                                uniform int maxIterations,
                                uniform int output[])
{
    uniform int y_start = taskIndex * span;
    uniform int y_end = min((uniform int)((taskIndex + 1) * span), height);

    foreach (j = y_start ... y_end, i = 0 ... width) {
        float x_normalized = ((float)i)/(width-1);
        float x = lerp(x_min, x_max, x_normalized);

        float y_normalized = ((float)j)/(height-1);
        float y = lerp(y_min, y_max, y_normalized);

        int index = j * width + i;
        output[index] = mandel(x, y, maxIterations);

        if (output[index] != maxIterations) {
            output[index] = 0;
        } else {
            output[index] = 255;
        }
    }
}

export void mandelbrot_task_ispc(uniform float x_min, uniform float x_max,
                            uniform float y_min, uniform float y_max,
                            uniform int width, uniform int height,
                            uniform int maxIterations,
                            uniform int output[])
{
    uniform int span = 8;

    launch[height/span] mandelbrot_scanline(x_min, x_max, y_min, y_max, width, height, span, maxIterations, output);

}
