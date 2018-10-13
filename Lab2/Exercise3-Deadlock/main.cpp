#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <random>

using namespace std;

mutex mut;
mutex mut2;

void increment(shared_ptr<int> value)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,1);

    for (unsigned int i = 0; i < 10000; ++i)
    {

        auto n = distribution(generator);
        cout << n << endl;

        if (n == 0) {

            mut.lock();
            mut2.lock();
            *value = *value + 1;
            mut2.unlock();
            mut.unlock();

        } else {

            mut2.lock();
            mut.lock();
            *value = *value + 1;
            mut.unlock();
            mut2.unlock();

        }
    }
}

int main(int argc, char **argv)
{
    auto value = make_shared<int>(0);
    auto value2 = make_shared<int>(0);

    vector<thread> threads;

    for (unsigned int i = 0; i < 2; ++i)
        threads.push_back(thread(increment, value));

    for (auto &t : threads)
        t.join();

    cout << "Value = " << *value << endl;
}
