/*******************************************************************************
 * File        : BlockingQueue.h
 * Description : A thread-safe queue that blocks for peeking and popping if the
 *               queue is empty.
 * Author(s)   : Tekin Ozbek <tekin@tekinozbek.net>
 *
 * Copyright (c) 2014 Tekin Ozbek, Ryan Lynar
 *
 * This file is part of pshare.
 *
 * pshare is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * pshare is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * pshare. If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "out.h"

/**
 * @class   BlockingQueue
 * 
 * @brief   Thread-safe blocking queue.
 * 
 * @details A thread-safe queue that blocks execution of thread when front
 *          element is requested if the queue is empty. The underlying queue is
 *          std::queue.
 */
template <typename T>
class BlockingQueue {

    public:
        /**
         * @brief Pushes an element to the queue.
         * 
         * Pushes an element of type T to the end of the queue. This function is
         * thread-safe.
         *
         * @param[in]   elem    Reference to the element that will be inserted
         *                      into the queue.
         *
         */
        void push(const T& elem);

        /**
         * @brief Pops the frontmost element.
         * 
         * Pops the frontmost element from the queue. This function is
         * thread-safe.
         */
        void pop();

        /**
         * @brief Retrieves the frontmost element.
         * 
         * Returns a reference to the frontmost element of the queue. If there
         * are no elements in the queue, the execution of the thread will be
         * blocked (wait) until an element is available. This function is
         * thread-safe.
         *
         * @return Reference to the frontmost element.
         */
        T& front();

    private:
        /// The underlying queue.
        std::queue<T> backing_queue;

        /// Mutex to ensure thread safety.
        std::mutex thread_safety;

        /// The condition variable to block execution when queue is empty.
        std::condition_variable condition;
        
};

template <typename T>
void BlockingQueue<T>::push(const T& elem) {
    
    {
        std::unique_lock<std::mutex> lock(thread_safety);
        backing_queue.push(elem);
    }

    condition.notify_one();
}

template <typename T>
void BlockingQueue<T>::pop() {  

    std::unique_lock<std::mutex> lock(thread_safety);
    backing_queue.pop();
}

template <typename T>
T& BlockingQueue<T>::front() {

    std::unique_lock<std::mutex> lock(thread_safety);
    
    /* if queue is empty wait for a new element, otherwise return front */
    condition.wait(lock, [=]{ return !backing_queue.empty(); });

    T& retval = backing_queue.front();
    backing_queue.pop();
    
    return retval;
}

#endif
