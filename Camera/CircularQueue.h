/*
 *  Copyright (C) 2019 刘臣轩
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

template <class type, int length>
class CircularQueue {
private:
    type data[length];
    int head;
    int tail;

public:
    CircularQueue<type, length>()
        : head(0)
        , tail(0) {};
    constexpr int size() const
    {
        return length;
    }

    bool empty() const
    {
        return head == tail;
    }

    void push(const type& obj)
    {
        data[head] = obj;
        head = (head + 1) % length;
        if (head == tail) {
            tail = (tail + 1) % length;
        }
    }

    bool pop(type& obj)
    {
        if (empty())
            return false;
        obj = data[tail];
        tail = (tail + 1) % length;
        return true;
    }
};
