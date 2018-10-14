#include <iostream>
#include <vector>
#include <future>
#include <string>
#include <ctime>
#include <ratio>
#include <iomanip>
#include <math.h>
#include <omp.h>
#include <FreeImagePlus.h>

using namespace std;
using namespace std::chrono;

// Number of iterations to perform to find pixel value
constexpr size_t max_iterations = 100;

// Dimension of the image (in pixels) to generate
constexpr size_t dim = 8192;

// Mandelbrot dimensions are ([-2.1, 1.0], [-1.3, 1.3])
constexpr double xmin = -2.1;
constexpr double xmax = 1.0;
constexpr double ymin = -1.3;
constexpr double ymax = 1.3;

// The conversion from Mandelbrot coordinate to image coordinate
constexpr double integral_x = (xmax - xmin) / static_cast<double>(dim);
constexpr double integral_y = (ymax - ymin) / static_cast<double>(dim);

void hello()
{
    // Get the thread number
    auto my_rank = omp_get_thread_num();
    // Get the number of threads in operation
    auto thread_count = omp_get_num_threads();
    // Display a message

    cout << "Hello from thread " << my_rank << " of " << thread_count << endl;
}

void saveImage(vector<vector<double>> results, std::string flag)
{
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

    if(flag == "mandelbrot.bmp") {
        mb.save("mandelbrot.bmp", false);
    } else {
        mb.save("mandelbromp.bmp", false);
    }
}

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

void threads()
{
    auto num_threads = thread::hardware_concurrency();
    size_t strip_height = dim / num_threads;

    vector<future<vector<double>>> futures;

    for (unsigned int i = 0; i < num_threads; ++i)
        futures.push_back(async(mandelbrot, i * strip_height, (i + 1) * strip_height));

    vector<vector<double>> results;
    for (auto &f : futures)
        results.push_back(f.get());

    saveImage(results, "mandelbrot.bmp");

}

void openmp()
{
    vector<vector<double>> results;
    for (size_t i = 0; i < dim; i++) {
        vector<double> r;
        for (size_t j = 0; j < dim; j++) {
            r.push_back(0);
        }
//        cout << r.size() << endl;
        results.push_back(r);
    }
//        cout << results.size() << endl;



    auto num_threads = thread::hardware_concurrency();
    size_t strip_height = dim / num_threads;
    strip_height = 1;

    double x, y, x1, y1, xx = 0.0;
    size_t loop_count = 0;
    size_t start_y, end_y;


    high_resolution_clock::time_point start = high_resolution_clock::now();


    #pragma omp parallel for num_threads(4) private(start_y, end_y, x, y, x1, y1, xx, loop_count) schedule(static,16)
    for (size_t i = 0; i < dim; ++i)
    {

        start_y = i * strip_height;
        end_y = (i + 1) * strip_height;

//        auto my_rank = omp_get_thread_num();
//        auto thread_count = omp_get_num_threads();
//        std::string test = "Thread " + to_string(my_rank) + " of " + to_string(thread_count) + " | " + to_string(start_y) + " - " + to_string(end_y)+"\n";
//        cout << test;

        y = ymin + (start_y * integral_y);
        for (size_t y_coord = start_y; y_coord < end_y; ++y_coord)
        {
            x = xmin;

            for (size_t x_coord = 0; x_coord < dim; ++x_coord)
            {
                x1 = 0.0, y1 = 0.0;
                loop_count = 0;

                while (loop_count < max_iterations && sqrt((x1 * x1) + (y1 * y1)) < 2.0)
                {
                    ++loop_count;
                    xx = (x1 * x1) - (y1 * y1) + x;
                    y1 = 2 * x1 * y1 + y;
                    x1 = xx;
                }

                auto val = static_cast<double>(loop_count) / static_cast<double>(max_iterations);
                results[y_coord][x_coord] = val;
                x += integral_x;
//                cout << x_coord << " x " << y_coord << endl;
            }
            y += integral_y;
        }
    }


    high_resolution_clock::time_point finish = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(finish - start);
    double time = time_span.count() * 1000;


    cout << std::fixed << std::setprecision( 0 );
    cout << time;
    cout << " ms";

    saveImage(results, "mandelbromp.bmp");

}

int main(int agrc, char **argv)
{

    openmp();
    cout << endl;
//    threads();
    return 0;
}
