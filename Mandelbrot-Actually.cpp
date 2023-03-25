
#include <iostream>  // Input/output streams
#include <fstream>   // File streams
#include <cmath>
#include "tinypng.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <tuple>


const std::vector<std::tuple<int, int, int, int>> GRADIENT = {
    std::make_tuple(0, 0, 0, 0),
    std::make_tuple(1, 95, 15, 64),
    std::make_tuple(5, 154, 3, 30),
    std::make_tuple(7, 259, 139, 36),
    std::make_tuple(12, 227, 100, 20),
    std::make_tuple(15, 15, 76, 92),
    std::make_tuple(20, 2, 48, 71),
    std::make_tuple(255, 227, 89, 21)
};

std::tuple<int, int, int> getColor(int cycles) {
    if (cycles <= 0) {
        return std::make_tuple(0, 0, 0);
    }
    else if (cycles >= 255) {
        return std::make_tuple(227, 89, 21);
    }

    int lower = 0;
    while (lower < GRADIENT.size() - 1 && std::get<0>(GRADIENT[lower + 1]) < cycles) {
        lower++;
    }

    float mRed = (float)(std::get<1>(GRADIENT[lower + 1]) - std::get<1>(GRADIENT[lower])) 
        / (float)(std::get<0>(GRADIENT[lower + 1]) - std::get<0>(GRADIENT[lower]));
    float mGreen = (float)(std::get<2>(GRADIENT[lower + 1]) - std::get<2>(GRADIENT[lower])) 
        / (float)(std::get<0>(GRADIENT[lower + 1]) - std::get<0>(GRADIENT[lower]));
    float mBlue = (float)(std::get<3>(GRADIENT[lower + 1]) - std::get<3>(GRADIENT[lower])) 
        / (float)(std::get<0>(GRADIENT[lower + 1]) - std::get<0>(GRADIENT[lower]));
    
    float bRed = (float)(std::get<1>(GRADIENT[lower]) - mRed * std::get<0>(GRADIENT[lower]));
    float bGreen = (float)(std::get<2>(GRADIENT[lower]) - mGreen * std::get<0>(GRADIENT[lower]));
    float bBlue = (float)(std::get<3>(GRADIENT[lower]) - mBlue * std::get<0>(GRADIENT[lower]));

    float r = (float)(mRed * cycles + bRed);
    float g = (float)(mGreen * cycles + bGreen);
    float b = (float)(mBlue * cycles + bBlue);
    return std::make_tuple((int)r, (int)g, (int)b);
}


auto start = std::chrono::high_resolution_clock::now();
unsigned char imageData[2000][2000][3]; 

int mandel(float cx, float cy, int maxIter) {
    float zx = 0;
    float zy = 0;
    int n = 0;

    for (int i = 0; i < maxIter; i++) {
        const float zx2 = zx * zx;
        const float zy2 = zy * zy;

        const float zxNew = zx2 - zy2 + cx;
        zy = 2 * zx * zy + cy;
        zx = zxNew;
        n++;

        if (zx2 + zy2 > 4.0) {
            return n;
        }
    }
    return maxIter;
}

// Draws part of the mandelbrot, starting from startRow and ending at (and including)
// endRow.
void drawMandelbrot(int startRow, int endRow) {
    auto thread_start = std::chrono::high_resolution_clock::now();

    for (int r = startRow; r <= endRow; r++) {
        for (int c = 0; c < 2000; c++) {
            float cx = c / 1000.0f - 1.5f;
            float cy = -r / 1000.0f + 1.0f;
            int iter = mandel(cx, cy, 255);
            if (iter == 255) {
                iter = 0;
            }
            auto colors = getColor(iter);
            imageData[r][c][0] = std::get<0>(colors);
            imageData[r][c][1] = std::get<1>(colors);
            imageData[r][c][2] = std::get<2>(colors);
        }
    }

    auto thread_stop = std::chrono::high_resolution_clock::now();
    auto thread_duration = std::chrono::duration_cast<std::chrono::microseconds>(thread_stop - thread_start);
    std::cout << "Rows " << startRow << "-" << endRow << ": " << thread_duration.count() << " microseconds" << std::endl;
}

void writeImageToDisk() {
    std::ofstream out("mandel.png", std::ios::binary);
    TinyPngOut pngout(2000, 2000, out);
    pngout.write((uint8_t*)imageData, static_cast<size_t>(2000 * 2000));
}


int main() {

    for (int i = 0; i < 255; i += 5) {
        auto colors = getColor(i);
        std::cout << i << ": "
            << std::get<0>(colors) << ", "
            << std::get<1>(colors) << ", "
            << std::get<2>(colors) << "\n";
    }


    std::thread t1(drawMandelbrot, 0, 499);   
    std::thread t2(drawMandelbrot, 500, 999);  
    std::thread t3(drawMandelbrot, 1000, 1499);  
    std::thread t4(drawMandelbrot, 1500, 1999);  
                                               

    // Wait until threads finish
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    writeImageToDisk();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
}

#include <vector>

int main_adding() {
    int result = 0;
    auto start = std::chrono::high_resolution_clock::now();

    for (int loops = 0; loops < 100; loops++) {
        for (int i = 1; i < 100'000'000; i++) {
            result += i;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
    std::cout << result << std::endl;
}

int main_inorder() {
    std::vector<int> numbers;
    for (int i = 1; i < 100'000'000; i++) {
        numbers.push_back(i);
    }

    int result = 0;
    auto start = std::chrono::high_resolution_clock::now();

    for (int loops = 0; loops < 100; loops++) {
        for (int num : numbers) {
            result += num;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
    std::cout << result << std::endl;
}
