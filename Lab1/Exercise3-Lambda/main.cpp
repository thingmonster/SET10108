#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;


void task() {

}

void monte_carlo_pi(size_t iterations)
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
}

int lambda()
{
    // Create data file
    ofstream data("lambda.csv", ofstream::out);

    for (size_t num_threads = 0; num_threads <= 3; ++num_threads)
    {
        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
        // Write number of threads
        cout << "Number of threads = " << total_threads << endl;
        // Write number of threads to the file
        data << "num_threads_" << total_threads;
        // Now execute 100 iterations
        for (size_t iters = 0; iters < 10; ++iters)
        {
            // Get the start time
            auto start = system_clock::now();

            // We need to create total_threads threads
            vector<thread> threads;
            for (size_t n = 0; n < total_threads; ++n)
                // Working in base 2 to make things a bit easier
//                threads.push_back(thread(monte_carlo_pi, static_cast<unsigned int>(pow(2.0, 19.0 - num_threads))));
                threads.push_back(thread([num_threads]{

                    int iterations = pow(2.0, 20.0 - num_threads);

                    std::random_device r;
                    default_random_engine e(r());
                    uniform_real_distribution<double> distribution(0.0, 1.0);

                    unsigned int in_circle = 0;
                    for (size_t i = 0; i < iterations; ++i)
                    {
                        auto x = distribution(e);
                        auto y = distribution(e);
                        auto length = sqrt((x * x) + (y * y));
                        if (length <= 1.0)
                            ++in_circle;
                    }
                    auto pi = (4.0 * in_circle) / static_cast<double>(iterations);
//                    cout << pi << endl;


                }));
            // Join the threads (wait for them to finish)
            for (auto &t : threads)
            {
                t.join();
            }
            // Get the end time
            auto end = system_clock::now();
            // Get the total time
            auto total = end - start;
            // Convert to milliseconds and output to file
            data << ", " << duration_cast<milliseconds>(total).count();
        }
        data << endl;
    }
    // Close the file
    data.close();
    return 0;
}

int externalFunction()
{
    // Create data file
    ofstream data("montecarlo.csv", ofstream::out);

    for (size_t num_threads = 0; num_threads <= 3; ++num_threads)
    {
        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
        // Write number of threads
        cout << "Number of threads = " << total_threads << endl;
        // Write number of threads to the file
        data << "num_threads_" << total_threads;
        // Now execute 100 iterations
        for (size_t iters = 0; iters < 10; ++iters)
        {
            // Get the start time
            auto start = system_clock::now();
            // We need to create total_threads threads
            vector<thread> threads;
            for (size_t n = 0; n < total_threads; ++n)
                // Working in base 2 to make things a bit easier
                threads.push_back(thread(monte_carlo_pi, static_cast<unsigned int>(pow(2.0, 20.0 - num_threads))));
            // Join the threads (wait for them to finish)
            for (auto &t : threads)
                t.join();
            // Get the end time
            auto end = system_clock::now();
            // Get the total time
            auto total = end - start;
            // Convert to milliseconds and output to file
            data << ", " << duration_cast<milliseconds>(total).count();
        }
        data << endl;
    }
    // Close the file
    data.close();
    return 0;
}

int main(int argc, char **argv)
{
    lambda();

}

