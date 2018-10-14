#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <omp.h>

using namespace std;
using namespace std::chrono;

// Generates a vector of random values
vector<unsigned int> generate_values(size_t size)
{
    // Create random engine
    random_device r;
    default_random_engine e(r());
    // Generate random numbers
    vector<unsigned int> data;
    for (size_t i = 0; i < size; ++i)
    data.push_back(e());
    return data;
}

void sort(vector<unsigned int> data) {

    int temp;

    for (int i = 0; i < data.size(); i++) {
        for (int j = 1; j < data.size() - i; j++) {
            if (data[j - 1] > data[j])  {
                temp = data[j-1];
                data[j-1] = data[j];
                data[j] = temp;
            }
        }
    }
}

void parallel_sort(vector<unsigned int>& values)
{
    // Get the number of threads
    auto num_threads = thread::hardware_concurrency();
    // Get the number of elements in the vector
    auto n = values.size();
    // Declare the variables used in the loop
    int i, tmp, phase;
    // Declare parallel section
#pragma omp parallel num_threads(num_threads) default(none) shared(values, n) private(i, tmp, phase)
    for (phase = 0; phase < n; ++phase)
    {
        // Determine which phase of the sort we are in
        if (phase % 2 == 0)
        {
            // Parallel for loop.  Each thread jumps forward 2 so no conflict
#pragma omp for
            for (i = 1; i < n; i += 2)
            {
                // Check if we should swap values
                if (values[i - 1] > values[i])
                {
                    // Swap values
                    tmp = values[i - 1];
                    values[i - 1] = values[i];
                    values[i] = tmp;
                }
            }
        }
        else
        {
            // Parallel for loop.  Each thread jumps forward 2 so no conflict
#pragma omp for
            for (i = 1; i < n; i += 2)
            {
                // Check is we should swap values
                if (values[i] > values[i + 1])
                {
                    // Swap values
                    tmp = values[i + 1];
                    values[i + 1] = values[i];
                    values[i] = tmp;
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    double average;
    ofstream results("bubble.csv", ofstream::out);
    for (size_t size = 8; size <= 12; ++size)
    {
        average = 0;
        results << pow(2, size) << ", ";
        for (size_t i = 0; i < 100; ++i)
        {
            cout << "Generating " << i << " for " << pow(2, size) << " values" << endl;
            auto data = generate_values(static_cast<size_t>(pow(2, size)));
            cout << "Sorting" << endl;

            auto start = system_clock::now();
            parallel_sort(data);
            auto end = system_clock::now();
            auto total = duration_cast<milliseconds>(end - start).count();

            average += total;
            results << total << ",";
        }
        average /= 100;
        results << "," << average << endl;
    }
    results.close();

    return 0;
}


int main2(int argc, char **argv)
{
    double average;
    ofstream results("bubble.csv", ofstream::out);
    for (size_t size = 8; size <= 11; ++size)
    {
        average = 0;
        results << pow(2, size) << ", ";
        for (size_t i = 0; i < 100; ++i)
        {
            cout << "Generating " << i << " for " << pow(2, size) << " values" << endl;
            auto data = generate_values(static_cast<size_t>(pow(2, size)));
            cout << "Sorting" << endl;

            auto start = system_clock::now();
            sort(data);
            auto end = system_clock::now();
            auto total = duration_cast<milliseconds>(end - start).count();

            average += total;
            results << total << ",";
        }
        average /= 100;
        results << "," << average << endl;
    }
    results.close();

    return 0;
}
