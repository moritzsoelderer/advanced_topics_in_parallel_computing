#include <iostream>
#include <thread>
#include <atomic>

class LinkedListElem {
    public:
        std::atomic<int> flag;
        LinkedListElem* next;
};

class MCS {
    std::atomic<LinkedListElem*> tail;

    public:
        void acquire(LinkedListElem* e, int thread_id) {
            LinkedListElem* pred = tail.exchange(e);
            e->next = nullptr;
            if (pred == nullptr) {
                e->flag.store(0);
            }
            else {
                pred->next = e;
                e->flag.store(1);
                while(e->flag.load() == 1) {
                    // *spin
                }
            }
            std::cout << "Acquired: " << thread_id << std::endl;
        }

        void release(LinkedListElem* e,  int thread_id) {
            if(e->next == nullptr) {
                while(!tail.compare_exchange_strong(e, NULL)) {
                    if(e->next != NULL) {
                        e->next->flag.store(0);
                    }
                }
            }
            else {
                e->next->flag.store(0);
            }
            std::cout << "Released: " << thread_id << std::endl;
        }
};

void acquire_and_release(MCS* m, int thread_id) {
    LinkedListElem e;
    m->acquire(&e, thread_id);
    m->release(&e, thread_id);
}

int main() {
    MCS m;

    std::thread t1(acquire_and_release, &m, 0);
    std::thread t2(acquire_and_release, &m, 1);

    t1.join();
    t2.join();
}