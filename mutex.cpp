#include <mutex>
#include <iostream>
#include <thread>

std::mutex m;

void lock_and_unlock(const int i) {
    m.lock();
    std::cout << "Locked: " << i << std::endl;
    m.unlock();
    std::cout << "Unlocked: " << i << std::endl;

}

int main() {
    std::thread t1(lock_and_unlock, 0);
    std::thread t2(lock_and_unlock, 1);

    t1.join();
    t2.join();
}