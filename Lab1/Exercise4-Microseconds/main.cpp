#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main()
{

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    for (int i=0; i<100000; ++i) {
        std::cout << "- ";
    }
    std::cout << std::endl;

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << time_span.count() << " seconds.";
    std::cout << std::endl;

    return 0;
}
