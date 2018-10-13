#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <atomic>

using namespace std;
using namespace std::chrono;


void task() {

}

void monte_carlo_pi(size_t iterations)
{
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
}

void monte_carlo_atomic(size_t iterations)
{
    std::random_device r;
    default_random_engine e(r());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    auto in_circle = make_shared<atomic<int>>();

    for (size_t i = 0; i < iterations; ++i)
    {
        auto x = distribution(e);
        auto y = distribution(e);
        auto length = sqrt((x * x) + (y * y));

        if (length <= 1.0) {
            (*in_circle)++;
        }
    }

    auto pi = (4.0 * *in_circle) / static_cast<double>(iterations);
}


int main(int argc, char **argv)
{

    ofstream data("mcpi_atomic.csv", ofstream::out);
    double average;

    for (size_t type = 0; type<= 1; ++type)
    {
        if (type == 0) {
            data << endl << "ATOMIC" << endl;
        } else {
            data << endl << "STANDARD" << endl;
        }

        for (size_t num_threads = 0; num_threads <= 3; ++num_threads)
        {
            average = 0;

            auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
            cout << "Number of threads = " << total_threads << endl;
            data << "num_threads_" << total_threads;

            for (size_t iters = 0; iters < 10; ++iters)
            {
                vector<thread> threads;

                auto start = system_clock::now();

                for (size_t n = 0; n < total_threads; ++n) {

                    if (type == 0) {
                        threads.push_back(thread(monte_carlo_atomic, static_cast<unsigned int>(pow(2.0, 20.0 - num_threads))));
                    } else {
                        threads.push_back(thread(monte_carlo_pi, static_cast<unsigned int>(pow(2.0, 20.0 - num_threads))));
                    }

                }

                for (auto &t : threads)
                    t.join();

                auto end = system_clock::now();
                auto total = end - start;

                double duration = duration_cast<milliseconds>(total).count();
                average += duration;
                data << ", " << duration;
            }

            average = average / 10;
            data << ", " << average << endl;
        }
    }

//
//    data << endl << "STANDARD" << endl;
//
//    for (size_t num_threads = 0; num_threads <= 3; ++num_threads)
//    {
//        average = 0;
//
//        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
//        cout << "Number of threads = " << total_threads << endl;
//        data << "num_threads_" << total_threads;
//
//        for (size_t iters = 0; iters < 10; ++iters)
//        {
//            vector<thread> threads;
//
//            auto start = system_clock::now();
//
//            for (size_t n = 0; n < total_threads; ++n)
//                threads.push_back(thread(monte_carlo_pi, static_cast<unsigned int>(pow(2.0, 20.0 - num_threads))));
//
//            for (auto &t : threads)
//                t.join();
//
//            auto end = system_clock::now();
//            auto total = end - start;
//
//            double duration = duration_cast<milliseconds>(total).count();
//            average += duration;
//            data << ", " << duration;
//        }
//
//        average /= 10;
//        data << ", " << average << endl;
//    }

    data.close();


    return 0;
}

