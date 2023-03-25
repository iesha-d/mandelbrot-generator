# Mandelbrot Generator

![mandelbrot render](mandel.png)

This repo houses performance experiments in C++ for drawing a Mandelbrot fractal.

I used this codebase to experiment with multithreading, low-level optimization,
and fast access to memory. It's nothing special in terms of the output image.

The strategy I use for multithreading is to split the image into horizontal
chunks, each of the same size, and then ask one thread to render each.
The number of threads matches the number of cores on the CPU.
This gives about a 3.5x speedup when 4 threads are used.

## Reading the Code

Everything is in Mandelbrot-Actually.cpp. Here's how I have it organized:
- RGB tuples for the output color gradient are stored in the vector `GRADIENT`.
- `getColor` assigns RGB values to the pixels from their iteration count.
- Threads are created in `main`.
- Each thread runs `drawMandelbrot` and draws a range of rows.
- The drawing time is recorded.

## Libraries

I used [tinypng](https://www.nayuki.io/page/tiny-png-output) (tinypng.cpp/.hpp)
to output the image of the Mandelbrot.

