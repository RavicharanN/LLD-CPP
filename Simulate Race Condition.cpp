// Simulates a Race condition. How?
// Each thread might be concurrently writing back to the counter after increments
// since the varialbe is not atomic, it could lead to lost increments (<2000 total value)
// Solution: use mutex and aquire a lock before modifying or use atomic<int> counter (#include<atomic>)
#include <mutex>
#include <thread>
#include <iostream>
using namespace std;

int counter = 1;

void inc_cnt()
{
    int n = 1000;
    while (n--)
        counter++;
}

int main()
{
    thread th1(inc_cnt);
    thread th2(inc_cnt);
    th1.join();
    th2.join();
}
