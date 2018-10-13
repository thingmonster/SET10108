#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <fstream>
#include <random>
#include <iomanip>
#include <ctime>
#include <ratio>
#include <chrono>
#include <mutex>

using namespace std;
using namespace std::chrono;

mutex mut;


void incrementAtomic(shared_ptr<atomic<int>> value, int amount)
{
    for (unsigned int i = 0; i < amount; ++i) {
        (*value)++;
    }
}

void incrementStandard(shared_ptr<int> standard, int amount)
{

    for (unsigned int i = 0; i < amount; ++i) {

        mut.lock();
        *standard = *standard + 1;
        mut.unlock();
    }
}

int main(int argc, char **argv)
{

    ofstream data("test.csv", ofstream::out);



    for (size_t increment_type = 0; increment_type <= 1; ++increment_type) {

        if (increment_type == 0) {
            data << "ATOMIC" << endl;
        } else {
            data << "STANDARD" << endl;
        }
        data << "Number of Threads,Times,,,,,,,,,,Average" << endl;

        for (size_t num_threads = 0; num_threads <= 3; ++num_threads) {

            double average = 0.0;

            auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
            data << total_threads << " Threads";

            auto value = make_shared<atomic<int>>();
            auto standard = make_shared<int>();

            auto amount = static_cast<unsigned int>(pow(2.0, 18.0 - num_threads));
            cout << "amount: " << amount << endl;

            for (size_t i = 0; i < 10; ++i) {

                vector<thread> threads;
                // start clock ---------------------------------------------
                high_resolution_clock::time_point start = high_resolution_clock::now();

                for (unsigned int i = 0; i < total_threads; ++i) {
                    if (increment_type == 0) {
                        threads.push_back(thread(incrementAtomic, value, amount));
                    } else {
                        threads.push_back(thread(incrementStandard, standard, amount));
                    }
                }

                for (auto &t : threads)
                    t.join();

                high_resolution_clock::time_point finish = high_resolution_clock::now();
                // end clock ---------------------------------------------

                duration<double> time_span = duration_cast<duration<double>>(finish - start);
                double time = time_span.count() * 1000;

                data << ",";
                data << std::fixed << std::setprecision( 0 );
                data << time;
                data << " ms";

                average += time_span.count();


            }

            average *= 1000 / 10;
//            cout << endl << average << endl;
            cout << *value << endl << *standard << endl << endl;
            data << "," << average << " ms" << endl;

            *value = 0;
            *standard = 0;
        }

        data << endl;
    }

    data.close();
}
