#include <iostream>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

void increment(shared_ptr<int> value)
{
    cout << "hi" << endl;
    for (unsigned int i = 0; i < 1000; ++i)
        *value = *value + 1;
}

int main()
{
    auto value = make_shared<int>(0);

    // Create number of threads hardware natively supports
    auto num_threads = thread::hardware_concurrency();
    vector<thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i)
        threads.push_back(thread(increment, value));

    for (auto &t : threads)
        t.join();

    cout << "Value = " << *value << endl;
    return 0;
}
