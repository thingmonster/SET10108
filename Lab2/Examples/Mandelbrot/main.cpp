#include <iostream>
#include <vector>
#include <future>
#include <math.h>
#include <FreeImagePlus.h>

using namespace std;

// Number of iterations to perform to find pixel value
constexpr size_t max_iterations = 100;

// Dimension of the image (in pixels) to generate
constexpr size_t dim = 256;//192;

// Mandelbrot dimensions are ([-2.1, 1.0], [-1.3, 1.3])
constexpr double xmin = -2.1;
constexpr double xmax = 1.0;
constexpr double ymin = -1.3;
constexpr double ymax = 1.3;

// The conversion from Mandelbrot coordinate to image coordinate
constexpr double integral_x = (xmax - xmin) / static_cast<double>(dim);
constexpr double integral_y = (ymax - ymin) / static_cast<double>(dim);

vector<double> mandelbrot(size_t start_y, size_t end_y)
{
    // Declare values we will use
    double x, y, x1, y1, xx = 0.0;
    size_t loop_count = 0;
    // Where to store the results
    vector<double> results;

    // Loop through each line
    y = ymin + (start_y * integral_y);
    for (size_t y_coord = start_y; y_coord < end_y; ++y_coord)
    {
        x = xmin;
        // Loop through each pixel on the line
        for (size_t x_coord = 0; x_coord < dim; ++x_coord)
        {
            x1 = 0.0, y1 = 0.0;
            loop_count = 0;
            // Calculate Mandelbrot value
            while (loop_count < max_iterations && sqrt((x1 * x1) + (y1 * y1)) < 2.0)
            {
                ++loop_count;
                xx = (x1 * x1) - (y1 * y1) + x;
                y1 = 2 * x1 * y1 + y;
                x1 = xx;
            }
            // Get value where loop completed
            auto val = static_cast<double>(loop_count) / static_cast<double>(max_iterations);
            // Push this value onto the vector
            results.push_back(val);
            // Increase x based on integral
            x += integral_x;
        }
        // Increase y based on integral
        y += integral_y;
    }
    // Return vector
    return results;
}

int main(int agrc, char **argv)
{
    auto num_threads = thread::hardware_concurrency();
    size_t strip_height = dim / num_threads;

    vector<future<vector<double>>> futures;
    for (unsigned int i = 0; i < num_threads; ++i)
        futures.push_back(async(mandelbrot, i * strip_height, (i + 1) * strip_height));

    vector<vector<double>> results;
    for (auto &f : futures)
        results.push_back(f.get());


    fipImage mb;
    mb.setSize(FIT_BITMAP, dim,dim,24);


    unsigned x = 0;
    unsigned y = 0;
    for (auto &r : results) {

        for (auto &l : r) {

            auto d = round(l * 255);
            RGBQUAD rgb = {d,d,d,0};
            mb.setPixelColor(x,y,&rgb);

//            cout << x << " x " << y << " - " << l << " - " << d << endl;

            x++;
            if (x == dim) {
                x = 0;
                y++;
            }

        }
    }

    mb.save("mandelbrot.bmp", false);

    return 0;
}
