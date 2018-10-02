
#include <thread>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <future>
#include <chrono>
#include <ctime>
#include <ratio>

using namespace std;
using namespace std::chrono;

void task(std::promise<int> && p, std::promise<int> && q, vector<int> ints)
{
    int max = 0;
    int min = 99999999999;
    for (auto &i : ints) {

        if (i < min) {
            min = i;
        }
        if (i > max) {
            max = i;
        }
    }
//    cout << "Min: " << min << endl;
//    cout << "Max: " << max << endl;

    p.set_value(min);
    q.set_value(max);
}

int main()
{
    std::random_device r;
    default_random_engine e(r());

    int inputs = 800000;

    vector<int> test;
    for (size_t i = 0; i < inputs; ++i) {
        test.push_back(i);
    }

    ofstream data("lab1-1.csv", ofstream::out);

    for (size_t num_threads = 0; num_threads <= 3; ++num_threads) {

        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));

        vector<vector<int>> ints;
        for (size_t i = 0; i < total_threads; ++i) {
            vector<int> row;
            for (size_t j = 0; j < inputs / total_threads; ++j) {
                int a = (i * (inputs / total_threads)) + j;
                row.push_back(test.at(a));
            }
            ints.push_back(row);
        }

        data << "num_threads_" << total_threads;

        for (size_t iterations = 0; iterations < 10; ++iterations) {

            auto start = system_clock::now();
            high_resolution_clock::time_point t1 = high_resolution_clock::now();

            vector<thread> threads;
            vector<std::future<int>> fp;
            vector<std::future<int>> fq;

            for (auto &i : ints) {

                std::promise<int> p;
                std::promise<int> q;
                auto f = p.get_future();
                auto g = q.get_future();

                fp.push_back(std::move(f));
                fq.push_back(std::move(g));

                threads.push_back(thread(&task, std::move(p), std::move(q), i));
            }

            for (auto &t : threads) {
                t.join();
            }

            int min = -1;
            for (auto &f : fp) {
                int i = f.get();
                if ((i < min) || (min == -1)) {
                    min = i;
                }
            }
            cout << "min: " << min << endl;

            int max = 0;
            for (auto &f : fq) {
                int i = f.get();
                if (i > max) {
                    max = i;
                }
            }
            cout << "max: " << max << endl;

            auto end = system_clock::now();
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            auto total = end - start;
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            data << ", " << duration_cast<milliseconds>(total).count();
            data << ", " << time_span.count();
        }
        data << endl;
    }

    data.close();

    return 0;
}
