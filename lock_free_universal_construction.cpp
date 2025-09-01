#include <thread>
#include <atomic>
#include <iostream>
#include <functional>


class Node_Consensus;


struct Node {
    Node *next = nullptr;
    int seq = 0;
    Node_Consensus *cons = nullptr;
    std::function<int()> invocation = nullptr;

    Node(Node *n, int s, Node_Consensus *c, std::function<int()> inv)
        : next(n), seq(s), cons(c), invocation(inv) {}
};

class Node_Consensus {
    std::atomic<Node*> val {nullptr};

    public:
        Node* decide(Node *own_val) {
            Node *expected = nullptr;
            val.compare_exchange_strong(expected, own_val);
            return expected;
        }
};

class Counter {
    int count = 0;
    
    public:
        int increment() {
            return count++;
        }
};

class LF_Counter {
    Node *head = nullptr;
    Node* tail[4] = {nullptr, nullptr, nullptr, nullptr};

    public:
        Counter counter;

        LF_Counter(Counter counter) {
            this->counter = counter;
            Node *first = new Node(nullptr, 0, new Node_Consensus(), nullptr);
            this->head = first;
            for (int i = 0; i < 4; ++i) {
                tail[i] = first;
            }

        }

        int apply(Node *proposed, int thread_id) {
            while(proposed->seq == 0) {
                Node* tail = get_tail();
                Node *win = tail->cons->decide(proposed);
                tail->next = win;
                win->seq = tail->seq + 1;
                this->tail[thread_id] = win;
            }

            std::cout << "Achieved Consensus: " << thread_id << std::endl;

            Counter counter1 = counter;
            Node *current_invocation = head;
            while(current_invocation->next != proposed) {
                current_invocation = current_invocation->next;
                current_invocation->invocation();
            }
            return current_invocation->invocation();
        }

        Node* get_tail() {
            Node *res = tail[0];
            for(int i=1; i<4; i++) {
                if(tail[i]->seq > res->seq) {
                    res = tail[i];
                }
            }
            return res;
        }

};

void achieve_consensus_and_invoke(LF_Counter *lf_counter, int thread_id) {
    Node_Consensus *n = new Node_Consensus();

    Node *proposed = new Node(
        nullptr,
        0,
        n,
        [lf_counter]() {
            return lf_counter->counter.increment();
        }
    );

    int result = lf_counter->apply(proposed, thread_id);

    std::cout << "Replayed invocations: " << thread_id << " - Result: " << result << std::endl;
}


int main() {
    Counter *counter = new Counter();
    LF_Counter *lf_counter = new LF_Counter(*counter);

    std::thread t0(achieve_consensus_and_invoke, lf_counter, 0);
    std::thread t1(achieve_consensus_and_invoke, lf_counter, 1);
    std::thread t2(achieve_consensus_and_invoke, lf_counter, 2);
    std::thread t3(achieve_consensus_and_invoke, lf_counter, 3);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
}