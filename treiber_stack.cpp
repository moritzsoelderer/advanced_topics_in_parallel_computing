#include <thread>
#include <atomic>
#include <iostream>


// implementation needs 16 byte atomic operation support
// uint_ptr should get rid of this problem
struct Node {
    Node *next;
    int val;
};

struct Counted_Pointer {
    Node *ptr;
    short seq;
};

class Treiber_Stack {
    std::atomic<Counted_Pointer> tos; //top of stack

    public:
        void push(Node *to_push, int thread_id) {
            Counted_Pointer expected = tos.load();
            while (true) {
                Counted_Pointer new_tos = {to_push, expected.seq};
                if(tos.compare_exchange_weak(expected, new_tos)) {
                    to_push->next = expected.ptr;
                    std::cout << "Pushed: " << thread_id << std::endl;
                    return;
                }
            }
        }

        Node* pop(int thread_id) {
            Counted_Pointer expected = tos.load();
            if (expected.ptr == nullptr) {
                return nullptr;
            }
            while (true) {
                Counted_Pointer new_tos = {expected.ptr->next, expected.seq+1};
                if(tos.compare_exchange_weak(expected, new_tos)) {
                    std::cout << "Popped: " << thread_id << std::endl;
                    return expected.ptr;
                }
            }
        }
};


void push_push_and_pop(Treiber_Stack *st, int thread_id) {
    Node *n1 = new Node{nullptr, thread_id * 10 + 1};
    Node *n2 = new Node{nullptr, thread_id * 10 + 2};
    st->push(n1, thread_id);
    st->push(n2, thread_id);
    st->pop(thread_id);
}

int main() {
    Treiber_Stack *st = new Treiber_Stack();
    std::thread t0(push_push_and_pop, st, 0);
    std::thread t1(push_push_and_pop, st, 1);
    std::thread t2(push_push_and_pop, st, 2);
    std::thread t3(push_push_and_pop, st, 3);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
}