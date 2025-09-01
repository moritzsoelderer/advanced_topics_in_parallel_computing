#include <thread>
#include <atomic>
#include <iostream>

class Barrier {
    std::atomic<bool> flag = false;
    std::atomic<int> counter = 0;

    private:
        int n;

    public:
        Barrier(int n) {
            this->n = n;
        };

        void arrive_and_wait(int thread_id) {
            if(counter.fetch_add(1) == this->n -1) {
                counter.store(0);
                flag.store(true);
                std::cout << "Arrived: " << thread_id << std::endl;
            }
            else {
                std::cout << "Arrived: " << thread_id << std::endl;
                while(flag.load() != true) {
                    // *spin
                }
            }
        }
};

int main() {
    Barrier b(4);
    std::thread t0(Barrier::arrive_and_wait, &b, 0);
    std::thread t1(Barrier::arrive_and_wait, &b, 1);
    std::thread t2(Barrier::arrive_and_wait, &b, 2);
    std::thread t3(Barrier::arrive_and_wait, &b, 3);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
}