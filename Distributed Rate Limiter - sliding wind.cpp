#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <unordered_set>
// #include<shared_ptr>
#include <memory>
#include <cassert>
using namespace std;
using namespace std::chrono;

/* Distributed Rate Limiter with TTL */

/*
Things to keep in mind:

1. use of server side clocks
2. User of UserRequest to avoid copying of mutex
    2a. If one needs to copy the mutex htey wwill have to define it as a shared_ptr and call make_shared<mutex>() on it during init
3. A cleanup function that will be called by a worker thread to clean inactive user buckets
4. For concurrency:
    4a. Use of a global mutex to make the shared DB (users map) access thread safe
    4b. Use a user specfic mutex to prevent concurrent access from  single users
    4c. This design makes it threadsafe per user while allowing requests concurrently for multiple users
*/

struct UserRequest
{
    queue<int> q;
    mutex mtx;
    int lastRequestTime;

    UserRequest();
};

class RateLimiter
{
public:
    int windSize;
    int limit;
    steady_clock::time_point startTime;
    int ttl;

    RateLimiter(int windSize, int limit, int ttl)
    {
        this->windSize = windSize;
        this->limit = limit;
        this->ttl = ttl;
        startTime = steady_clock::now();
    }

    // return false if request is dropped
    bool processRequest(string userId)
    {
        unique_lock<mutex> lock(global_mtx); // doing only this would make it synchronous
        if (userRequests.find(userId) == userRequests.end())
        {
            userRequests[userId] = new UserRequest();
        }
        auto userEvent = userRequests[userId];
        lock.unlock();

        unique_lock<mutex> lock2(userEvent->mtx); // lock user specfric mutex

        int timestamp = getTimeElapseInSeconds();
        auto &q = userEvent->q;
        while (!q.empty() && q.front() < timestamp - windSize)
            q.pop();

        if (q.size() == limit)
            return false;

        q.push(timestamp);
        userEvent->lastRequestTime = max(userEvent->lastRequestTime, timestamp);

        return true;
    }

    // This will be called on a worker thread every specifed numbers of seconds
    void cleanInactiveUsers()
    {
        unique_lock<mutex> lock(global_mtx);
        unordered_set<string> tobeCleaned;
        for (auto request : userRequests)
        {
            int curTime = this->getTimeElapseInSeconds();
            auto userEvent = request.second;
            if (curTime - userEvent->lastRequestTime >= ttl)
                tobeCleaned.insert(request.first);
        }

        for (auto userId : tobeCleaned)
        {
            delete userRequests[userId]; // free the ptr to prevent mem leak
            userRequests.erase(userId);
        }
    }

    ~RateLimiter()
    {
        ttl = 0;
        cleanInactiveUsers();
    }

private:
    mutex global_mtx;
    unordered_map<string, UserRequest *> userRequests;

    int getTimeElapseInSeconds()
    {
        auto curTime = steady_clock::now();
        auto elapsedTime = duration_cast<seconds>(curTime - startTime);
        return elapsedTime.count();
    }
};

int main()
{
    // RateLimiter *ratelim = new RateLimiter(30, 10. 10);
    RateLimiter rl(5, 3, 10);
    assert(rl.processRequest("alice") == true);
    assert(rl.processRequest("alice") == true);
    assert(rl.processRequest("alice") == true);
    assert(rl.processRequest("alice") == false);
    cout << "[Test] Basic rate limiting test passed.\n";

    // Spawn threads to simulate requests
    vector<thread> workers;
    for (int i = 0; i < 3; ++i) {
        workers.emplace_back([&rl, i] {
            string user = "user" + to_string(i);
            for (int j = 0; j < 10; ++j) {
                bool ok = rl.processRequest(user);
                cout << "[Worker " << i << "] Request #" << j
                     << (ok ? " allowed" : " denied") << " for " << user << "\n";
                this_thread::sleep_for(milliseconds(200));
            }
        });
    }

    // Cleaner thread
    thread cleaner([&rl] {
        for (int k = 0; k < 3; ++k) {
            this_thread::sleep_for(seconds(5));
            rl.cleanInactiveUsers();
        }
    });

    // Join workers
    for (auto& t : workers) t.join();
    cleaner.join();

    cout << "[Main] Simulation complete.\n";
    return 0;
}
