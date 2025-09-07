#include <iostream>
#include <thread>


class PetersonsAlgorithm {
    int turn;
    bool interested[2] = {false, false};

    public:
        void acquire(int thread_id) {
            int other = 1 -thread_id;
            interested[thread_id] = true;
            turn = other;

            while(interested[other] && turn == other) {
                // *spin
            }
            std::cout << "Acquired: " << thread_id << std::endl;
        }

        void release(int thread_id) {
            interested[thread_id] = false;
            std::cout << "Released: " << thread_id << std::endl;
        }
};

void acquire_peterson(int thread_id, PetersonsAlgorithm* p) {
    p->acquire(thread_id);
    p->release(thread_id);
} 

int main() {
    PetersonsAlgorithm p;

    std::thread t1(acquire_peterson, 0, &p);
    std::thread t2(acquire_peterson, 1, &p);

    t1.join();
    t2.join();
}