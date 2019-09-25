#pragma once

template<class type, int length>
class CircleQueue {
private:
    type data[length];
    int head;
    int tail;

public:
    CircleQueue<type, length>()
        : head(0)
        , tail(0) {};
    constexpr int size() const {
        return length;
    }

    bool empty() const {
        return head == tail;
    }

    void push(const type& obj) {
        data[head] = obj;
        head = (head + 1) % length;
        if (head == tail) {
            tail = (tail + 1) % length;
        }
    }

    bool pop(type& obj) {
        if (empty())
            return false;
        obj = data[tail];
        tail = (tail + 1) % length;
        return true;
    }
};