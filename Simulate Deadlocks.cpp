// Simulate a dealine 
#include<mutex>
#include<thread>
#include<iostream>
using namespace std;

mutex m1;
mutex m2;

void t1() {
    m1.lock();
    std::this_thread.sleep_for(chrono::milliseconds(100));
    m2.lock();

    // Aquired both threads
    m2.unlock();
    m1.unlock();
}

void t2() {
    m2.lock();
    std::this_thread.sleep_for(chrono::milliseconds(100));
    m1.lock();

    // Aquired both threads
    m1.unlock();
    m2.unlock();
}

int main() {
    thread th1(t1);
    thread th2(t2);
    th1.join();
    th2.join();
}
