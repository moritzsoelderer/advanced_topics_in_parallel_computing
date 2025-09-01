#include <thread>
#include <atomic>
#include <iostream>
#include <vector>

class Sense_Reversed_Barrier {
    std::atomic<bool> sense = false;
    std::vector<bool> l_senses;
    std::atomic<int> counter = 0;

    private:
        int n;

    public:
        Sense_Reversed_Barrier(int n) : l_senses(n, false) {
            this->n = n;
        };

        void arrive_and_wait(int thread_id) {
            l_senses[thread_id] = !l_senses[thread_id];

            if(counter.fetch_add(1) == this->n -1) {
                counter.store(0);
                sense.store(!sense);
            }
            else {
                std::cout << "Arrived: " << thread_id << std::endl;
                while(sense.load() != l_senses[thread_id]) {
                    // *spin
                }
            }
        }
};


void arrive_and_wait_times_two(Sense_Reversed_Barrier* b, int thread_id) {
    b->arrive_and_wait(thread_id);
    std::cout << "Completed first barrier: " << thread_id << std::endl;
    b->arrive_and_wait(thread_id);
}

int main() {
    Sense_Reversed_Barrier b(4);
    std::thread t0(arrive_and_wait_times_two, &b, 0);
    std::thread t1(arrive_and_wait_times_two, &b, 1);
    std::thread t2(arrive_and_wait_times_two, &b, 2);
    std::thread t3(arrive_and_wait_times_two, &b, 3);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
}