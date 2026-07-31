#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <complex>
#include <string>
#include <cstdint>

using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t file_type{0x4D42};
    uint32_t file_size{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offset_data{54};
};

struct BMPInfoHeader {
    uint32_t size{40};
    int32_t width{0};
    int32_t height{0};
    uint16_t planes{1};
    uint16_t bit_count{24};
    uint32_t compression{0};
    uint32_t size_image{0};
    int32_t x_pixels_per_meter{0};
    int32_t y_pixels_per_meter{0};
    uint32_t colors_used{0};
    uint32_t colors_important{0};
};
#pragma pack(pop)

struct RGB { uint8_t r, g, b; };

RGB get_color(int iter, int max_iter, string palette) {
    if (iter >= max_iter) return {0, 0, 0};
    double t = (double)iter / max_iter;

    if (palette == "fire") {
        return {(uint8_t)(255 * t), (uint8_t)(128 * pow(t, 2)), 0};
    } else if (palette == "ocean") {
        return {0, (uint8_t)(255 * pow(t, 0.5)), (uint8_t)(255 * t)};
    } else if (palette == "neon") {
        return {(uint8_t)(255 * sin(t * 6.28)), (uint8_t)(255 * cos(t * 3.14)), (uint8_t)(255 * t)};
    } else if (palette == "dstep") {
        uint8_t r = (uint8_t)(90 + 165 * sin(iter * 0.1));
        uint8_t g = (uint8_t)(30 + 200 * cos(iter * 0.05));
        uint8_t b = (uint8_t)(150 + 105 * sin(iter * 0.08));
        return {r, g, b};
    } else {
        return {(uint8_t)(255 * sin(t * 10)), (uint8_t)(255 * cos(t * 10)), (uint8_t)(255 * sin(t * 20))};
    }
}

int calculate_pixel(double real, double imag, string formula, int max_iter, double z_pow, double c_pow, double cr, double ci) {
    // --- 1. Треугольник Серпинского ---
    if (formula == "sierpinski_tri") {
        unsigned int x = abs((int)(real * 500));
        unsigned int y = abs((int)(imag * 500));
        return ((x & y) == 0) ? (x ^ y) % max_iter : max_iter;
    }

    // --- 2. Ковёр Серпинского ---
    if (formula == "sierpinski_carpet") {
        long long x = abs((long long)(real * 1000000));
        long long y = abs((long long)(imag * 1000000));
        bool in_carpet = true;
        int iter = 0;
        while (x > 0 || y > 0) {
            if ((x % 3 == 1) && (y % 3 == 1)) {
                in_carpet = false;
                break;
            }
            x /= 3;
            y /= 3;
            iter++;
        }
        return in_carpet ? max_iter : (iter * 15) % max_iter;
    }

    // --- Комплексные фракталы ---
    complex<double> point(real, imag);
    complex<double> z = 0;
    complex<double> c = point;
    complex<double> k(cr, ci);

    if (formula == "julia") { 
        z = point; 
        c = k; 
    }

    // --- 3. Фрактал Ньютона (z^3 - 1 = 0) ---
    if (formula == "newton") {
        z = point;
        for (int i = 0; i < max_iter; ++i) {
            if (abs(z) < 1e-6) return max_iter;
            complex<double> z_prev = z;
            z = z - (pow(z, z_pow) - 1.0) / (z_pow * pow(z, z_pow - 1.0));
            if (norm(z - z_prev) < 1e-6) return i * 3;
        }
        return max_iter;
    }

    // --- 4. Фрактал Феникса ---
    if (formula == "phoenix") {
        z = point;
        complex<double> z_prev = 0;
        complex<double> p_c = (cr != 0.0 || ci != 0.0) ? k : complex<double>(0.5667, -0.5);
        for (int i = 0; i < max_iter; ++i) {
            if (norm(z) > 4.0) return i;
            complex<double> z_next = pow(z, z_pow) + p_c.real() + p_c.imag() * z_prev;
            z_prev = z;
            z = z_next;
        }
        return max_iter;
    }

    // --- Стандартные и Перпендикулярный Мандельброт ---
    for (int i = 0; i < max_iter; ++i) {
        if (norm(z) > 4.0) return i;

        if (formula == "mandelbrot" || formula == "julia") {
            z = pow(z, z_pow) + pow(c, c_pow) + (formula == "mandelbrot" ? k : complex<double>(0,0));
        } else if (formula == "perpendicular") { // --- 5. Перпендикулярный Мандельброт ---
            z = complex<double>(abs(z.real()), z.imag());
            z = pow(z, z_pow) + pow(c, c_pow) + k;
        } else if (formula == "burning_ship") {
            z = complex<double>(abs(z.real()), abs(z.imag()));
            z = pow(z, z_pow) + pow(c, c_pow);
        } else if (formula == "tricorn") {
            z = pow(conj(z), z_pow) + pow(c, c_pow);
        } else {
            z = pow(z, z_pow) + c;
        }
    }
    return max_iter;
}

int main(int argc, char* argv[]) {
    int width = 800, height = 600, max_iter = 150;
    double zoom = 1.0, cx = -0.75, cy = 0.0;
    double z_pow = 2.0, c_pow = 1.0, cr = 0.0, ci = 0.0;
    string formula = "mandelbrot", palette = "dstep", filename = "render.bmp";

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--width") width = stoi(argv[++i]);
        else if (arg == "--height") height = stoi(argv[++i]);
        else if (arg == "--zoom") zoom = stod(argv[++i]);
        else if (arg == "--cx") cx = stod(argv[++i]);
        else if (arg == "--cy") cy = stod(argv[++i]);
        else if (arg == "--iter") max_iter = stoi(argv[++i]);
        else if (arg == "--formula") formula = argv[++i];
        else if (arg == "--palette") palette = argv[++i];
        else if (arg == "--out") filename = argv[++i];
        else if (arg == "--zpow") z_pow = stod(argv[++i]);
        else if (arg == "--cpow") c_pow = stod(argv[++i]);
        else if (arg == "--cr") cr = stod(argv[++i]);
        else if (arg == "--ci") ci = stod(argv[++i]);
    }

    BMPHeader file_header;
    BMPInfoHeader info_header;
    info_header.width = width;
    info_header.height = height;
    
    int row_stride = (width * 3 + 3) & ~3;
    info_header.size_image = row_stride * height;
    file_header.file_size = file_header.offset_data + info_header.size_image;

    vector<uint8_t> image(info_header.size_image, 0);

    double aspect = (double)width / height;
    double scale = 4.0 / zoom;

    #pragma omp parallel for collapse(2)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double real = cx + ((double)x / width - 0.5) * scale * aspect;
            double imag = cy + ((double)y / height - 0.5) * scale;
            
            int iter = calculate_pixel(real, imag, formula, max_iter, z_pow, c_pow, cr, ci);
            RGB color = get_color(iter, max_iter, palette);

            int index = y * row_stride + x * 3;
            image[index] = color.b;
            image[index + 1] = color.g;
            image[index + 2] = color.r;
        }
    }

    ofstream file(filename, ios::binary);
    file.write((char*)&file_header, sizeof(file_header));
    file.write((char*)&info_header, sizeof(info_header));
    file.write((char*)image.data(), image.size());

    return 0;
}
