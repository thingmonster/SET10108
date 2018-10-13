#include <iostream>
#include <memory>
#include <thread>
#include "threadsafe_stack.h"

using namespace std;

void pusher(shared_ptr<threadsafe_stack<unsigned int>> stack) {
    for (unsigned int i = 0; i < 100; ++i) {
        stack->push(i);
        this_thread::yield();
    }
}

void popper(shared_ptr<threadsafe_stack<unsigned int>> stack) {
    unsigned int count = 0;
    while (count < 100) {
        count++;
        try {
            auto aval = stack->pop();
            ++count;
        }
        catch (exception e) {
            cout << e.what() << endl;
        }
    }
}

int main()
{
    auto stack = make_shared<threadsafe_stack<unsigned int>>();

    thread t1(popper, stack);
    thread t2(popper, stack);

    t1.join();
    t2.join();

    cout << "stack empty = " << stack->empty() << endl;

    return 0;
}
