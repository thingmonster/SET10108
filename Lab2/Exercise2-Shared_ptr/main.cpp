#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <memory>

using namespace std;
using namespace std::chrono;


void task() {

}

void monte_carlo_pi(size_t iterations, shared_ptr<double> mcpi)
{
    // Seed with real random number if available
    std::random_device r;
    // Create random number generator
    default_random_engine e(r());
    // Create a distribution - we want doubles between 0.0 and 1.0
    uniform_real_distribution<double> distribution(0.0, 1.0);

    // Keep track of number of points in circle
    unsigned int in_circle = 0;
    // Iterate
    for (size_t i = 0; i < iterations; ++i)
    {
        // Generate random point
        auto x = distribution(e);
        auto y = distribution(e);
        // Get length of vector defined - use Pythagarous
        auto length = sqrt((x * x) + (y * y));
        // Check if in circle
        if (length <= 1.0)
            ++in_circle;
    }
    // Calculate pi
    auto pi = (4.0 * in_circle) / static_cast<double>(iterations);
    *mcpi = *mcpi + pi;
//    cout << *mcpi << endl;
//    cout << pi << endl;
}

int main(int argc, char **argv)
{

    auto mcpi = make_shared<double>(0);

    ofstream data("montecarlo.csv", ofstream::out);

    data << "Iterations,Accuracy of pi,,,,,,,,,,Time" << endl;

    for (size_t iters = 10; iters < 19; ++iters) {

        int iterations = static_cast<unsigned int>(pow(2.0, iters));
        auto start = system_clock::now();
        data << "2^" << iters;

        for (size_t i = 0; i < 10; ++i) {

            *mcpi = 0;

            vector<thread> threads;

            for (size_t n = 0; n < 4; ++n) {
                threads.push_back(thread(monte_carlo_pi, iterations, mcpi));
            }

            for (auto &t : threads)
                t.join();

            *mcpi = *mcpi / 4;

            double accuracy;
            if ((3.14159 - *mcpi < 0.00001) && (3.14159 - *mcpi > -0.00001)) {
                accuracy = 5;
            } else if ((3.14159 - *mcpi < 0.0001) && (3.14159 - *mcpi > -0.0001)) {
                accuracy = 4;
            } else if ((3.14159 - *mcpi < 0.001) && (3.14159 - *mcpi > -0.001)) {
                accuracy = 3;
            } else if ((3.14159 - *mcpi < 0.01) && (3.14159 - *mcpi > -0.01)) {
                accuracy = 2;
            } else if ((3.14159 - *mcpi < 0.1) && (3.14159 - *mcpi > -0.1)) {
                accuracy = 1;
            } else if ((3.14159 - *mcpi < 1) && (3.14159 - *mcpi > -1)) {
                accuracy = 0;
            }

            data << "," << *mcpi << " (" << accuracy << " decimal places)";
        }

        auto end = system_clock::now();
        auto total = end - start;
        data << ", " << duration_cast<milliseconds>(total).count() << endl;
    }


    data.close();
    return 0;
}
