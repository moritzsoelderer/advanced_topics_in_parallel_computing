#include <thread>
#include <atomic>
#include <iostream>

// Writer Preference
class RWLock {
    std::atomic<int> state = 0;

    int writer_add = 1;
    int reader_add = 2;

    public:
        void reader_acquire(int thread_id) {
         while (true) {
            int s = state.load();
            if (s % 2 == 0) {
                if (state.compare_exchange_weak(s, s + reader_add)) {
                    break;
                }
            }
        }
            std::string out = "Reader Acquired: " + std::to_string(thread_id);
            std::cout << out << std::endl; 
        };

        void reader_release(int thread_id) {
            state.fetch_add(-reader_add);
            std::string out = "Reader Released: " + std::to_string(thread_id);
            std::cout << out << std::endl; 
        };

        void writer_acquire(int thread_id) {
            state.fetch_add(writer_add);
            while (state.load() != 1) {
                /*spin*/
            }
            std::string out = "Writer Acquired: " + std::to_string(thread_id);
            std::cout << out << std::endl; 
        };

        void writer_release(int thread_id) {
            state.fetch_add(-writer_add);
            std::string out = "Writer Released: " + std::to_string(thread_id);
            std::cout << out<< std::endl; 
        };
};


void reader_acquire_and_release(RWLock* l, int thread_id) {
    l->reader_acquire(thread_id);
    l->reader_release(thread_id);
}

void writer_acquire_and_release(RWLock* l, int thread_id) {
    l->writer_acquire(thread_id);
    l->writer_release(thread_id);
}


int main() {
    RWLock l;

    std::thread t0(reader_acquire_and_release, &l, 0);
    std::thread t1(reader_acquire_and_release, &l, 1);
    std::thread t2(reader_acquire_and_release, &l, 2);
    std::thread t3(reader_acquire_and_release, &l, 3);
    std::thread t4(writer_acquire_and_release, &l, 4);


    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}