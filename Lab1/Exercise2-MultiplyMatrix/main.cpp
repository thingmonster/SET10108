#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>

using namespace std;
using namespace std::chrono;


void task(vector<int> v, vector<vector<int>> m) {

    vector<vector<int>> dots;

    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < m.size(); ++j) {
            int dot = v.at(i) * m.at(j).at(i);
        }
    }
}

int main()
{
    // parameters etc

    int cols = 512;
    int rows = 512;
    ofstream data("lab1-1.csv", ofstream::out);

    // populate vector and matrix

    vector<int> vect;
    for (size_t i = 1; i <= cols; ++i) {
        vect.push_back(i);
    }

    vector<vector<int>> matrix;
    vector<int> row;

    for (size_t j = 1; j <= cols * rows; ++j) {
        row.push_back(j);
        if ((j % cols == 0) && ( j > 0)) {
            matrix.push_back(row);
            row.clear();
        }

    }

    // for n^m threads

    for (size_t num_threads = 0; num_threads <= 4; ++num_threads) {

        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
        data << "num_threads_" << total_threads;

        // split matrix into portions

        vector<vector<vector<int>>> tasks;

        for (size_t t = 0; t < total_threads; ++t) {

            int min = (rows / total_threads) * t;
            int max = min + (rows / total_threads);
            cout << min << " - " << max << endl;

            vector<vector<int>> row;
            for (size_t r = min; r < max; ++r) {
                row.push_back(matrix.at(r));
            }
            tasks.push_back(row);

        }

        cout << endl;


        // number of iterations for each configuration

        for (size_t iterations = 0; iterations < 30; ++iterations) {

            vector<thread> threads;
            auto start = system_clock::now();

            for (size_t thread_qty = 0; thread_qty < total_threads; ++thread_qty) {
                threads.push_back(thread(&task, vect, tasks.at(thread_qty)));
            }

            for (auto &t : threads) {
                t.join();
            }

            auto end = system_clock::now();
            auto total = end - start;
            data << ", " << duration_cast<milliseconds>(total).count();

        }
        data << endl;
    }

    data.close();




    return 0;
}
