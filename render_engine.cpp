#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <algorithm>

using namespace std;

struct Color { uint8_t r, g, b; };

static inline double clamp01(double x) { return x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 : x); }

Color get_color(int iter, int max_iter, const string& palette) {
    if (iter >= max_iter) return {0, 0, 0};
    double t = (double)iter / (double)max_iter;

    if (palette == "fire") {
        return {(uint8_t)(clamp01(t * 3.0) * 255), 
                (uint8_t)(clamp01((t - 0.33) * 3.0) * 255), 
                (uint8_t)(clamp01((t - 0.66) * 3.0) * 255)};
    } else if (palette == "ocean") {
        return {(uint8_t)(clamp01(1.0 - t * 2.0) * 255), 
                (uint8_t)(clamp01(1.0 - fabs(t - 0.5) * 2.0) * 255), 
                (uint8_t)(clamp01(t * 2.0) * 255)};
    } else if (palette == "neon") {
        return {(uint8_t)((0.5 + 0.5 * sin(t * 6.28)) * 255),
                (uint8_t)((0.5 + 0.5 * sin(t * 6.28 + 2.09)) * 255),
                (uint8_t)((0.5 + 0.5 * sin(t * 6.28 + 4.18)) * 255)};
    } else if (palette == "dstep") {
        // Динамическая многослойная палитра DSTEP
        double r = 0.5 + 0.5 * cos(3.0 * 3.14159 * t + 0.0);
        double g = 0.5 + 0.5 * cos(3.0 * 3.14159 * t + 0.6);
        double b = 0.5 + 0.5 * cos(3.0 * 3.14159 * t + 1.0);
        return {(uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255)};
    } else if (palette == "magma") {
        return {(uint8_t)(clamp01(t * 3.0 - 0.5) * 255),
                (uint8_t)(clamp01(t * 2.0 - 0.3) * 255),
                (uint8_t)(clamp01(t * 1.5) * 255)};
    } else if (palette == "grayscale") {
        uint8_t v = (uint8_t)(t * 255);
        return {v, v, v};
    } else { // Rainbow
        double h = t * 6.0;
        double x = 1.0 - fabs(fmod(h, 2.0) - 1.0);
        int hi = (int)floor(h) % 6;
        double r = 0, g = 0, b = 0;
        switch (hi) {
            case 0: r = 1; g = x; b = 0; break;
            case 1: r = x; g = 1; b = 0; break;
            case 2: r = 0; g = 1; b = x; break;
            case 3: r = 0; g = x; b = 1; break;
            case 4: r = x; g = 0; b = 1; break;
            default: r = 1; g = 0; b = x; break;
        }
        return {(uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255)};
    }
}

void render_frame(uint8_t* pixels, int width, int height, int rowSize, double zoom, double center_x, double center_y, const string& formula, const string& palette, int max_iter) {
    double aspect = (double)width / (double)height;
    double scale = 4.0 / zoom;

    #pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; y++) {
        double cy_pos = center_y + (y - height / 2.0) * (scale / height);
        for (int x = 0; x < width; x++) {
            double cx_pos = center_x + (x - width / 2.0) * (scale * aspect / width);
            
            double zx = 0.0, zy = 0.0;
            double cx = cx_pos, cy = cy_pos;

            if (formula == "julia") {
                zx = cx_pos; zy = cy_pos;
                cx = -0.7; cy = 0.27015;
            }

            int iter = 0;
            double zx_prev = 0.0, zy_prev = 0.0;

            for (int i = 0; i < max_iter; i++) {
                double zx2 = zx*zx, zy2 = zy*zy;
                if (zx2 + zy2 > 4.0) { iter = i; break; }

                if (formula == "burning_ship") {
                    zy = 2.0 * fabs(zx * zy) + cy; zx = zx2 - zy2 + cx;
                } else if (formula == "tricorn") {
                    zy = -2.0 * zx * zy + cy; zx = zx2 - zy2 + cx;
                } else if (formula == "phoenix") {
                    double nzx = zx2 - zy2 + cx + 0.5667 * zx_prev;
                    double nzy = 2.0 * zx * zy + cy;
                    zx_prev = zx; zy_prev = zy; zx = nzx; zy = nzy;
                } else {
                    zy = 2.0 * zx * zy + cy; zx = zx2 - zy2 + cx;
                }
                iter = i + 1;
            }

            int pIdx = y * rowSize + x * 3;
            Color c = get_color(iter, max_iter, palette);
            pixels[pIdx]     = c.b;
            pixels[pIdx + 1] = c.g;
            pixels[pIdx + 2] = c.r;
        }
    }
}

void generate_bmp_file(const string& filename, int width, int height, double zoom, double center_x, double center_y, const string& formula, const string& palette, int max_iter) {
    int rowSize = (width * 3 + 3) & ~3;
    int dataSize = rowSize * height;
    int fileSize = 54 + dataSize;

    vector<uint8_t> img(fileSize, 0);

    img[0] = 'B'; img[1] = 'M';
    img[2] = fileSize; img[3] = fileSize >> 8; img[4] = fileSize >> 16; img[5] = fileSize >> 24;
    img[10] = 54; img[14] = 40;
    img[18] = width; img[19] = width >> 8;
    img[22] = height; img[23] = height >> 8;
    img[26] = 1; img[28] = 24;

    render_frame(img.data() + 54, width, height, rowSize, zoom, center_x, center_y, formula, palette, max_iter);

    ofstream f(filename, ios::binary);
    f.write((char*)img.data(), img.size());
}

void generate_video(int sec, double start_zoom, double center_x, double center_y, const string& formula, const string& palette, int max_iter) {
    int fps = 30;
    int total_frames = sec * fps;
    int width = 854, height = 480;

    system("rm -rf frames && mkdir -p frames");

    for (int i = 0; i < total_frames; i++) {
        double current_zoom = start_zoom * pow(1.05, i);
        char name[64];
        sprintf(name, "frames/frame_%04d.bmp", i);
        generate_bmp_file(name, width, height, current_zoom, center_x, center_y, formula, palette, max_iter);
    }

    system("ffmpeg -y -framerate 30 -i frames/frame_%04d.bmp -c:v libx264 -pix_fmt yuv420p render.mp4 > /dev/null 2>&1");
}

int main(int argc, char** argv) {
    string mode = (argc > 1) ? argv[1] : "image";
    double zoom = (argc > 2) ? atof(argv[2]) : 1.0;
    double cx   = (argc > 3) ? atof(argv[3]) : -0.75;
    double cy   = (argc > 4) ? atof(argv[4]) : 0.0;
    string formula = (argc > 5) ? argv[5] : "mandelbrot";
    string palette = (argc > 6) ? argv[6] : "rainbow";
    int max_iter   = (argc > 7) ? atoi(argv[7]) : 150;

    if (mode == "video") {
        int sec = (argc > 8) ? atoi(argv[8]) : 5;
        generate_video(sec, zoom, cx, cy, formula, palette, max_iter);
    } else if (mode == "hd") {
        generate_bmp_file("render_hd.bmp", 1920, 1080, zoom, cx, cy, formula, palette, max_iter);
    } else {
        generate_bmp_file("render.bmp", 800, 600, zoom, cx, cy, formula, palette, max_iter);
    }
    return 0;
}
