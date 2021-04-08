#pragma once

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

namespace lemon
{
    /**
     * @brief Double-ended queue node element.
     * @author Zach Goethel
     */
    template <class T>
    struct deque_node
    {
        T data;
        deque_node* next = nullptr;
    };

    /**
     * Due to unexpected behavior of the STL double-ended queue under Windows,
     * I created a simpler queue which performs to my specification.  The MSVC
     * implementation of the double-ended queue misreported its size and led to
     * intermittent bad function calls.
     * 
     * This implementation does not increment its internal count until after all
     * new links are in place, thus avoiding the aforementioned runtime errors.
     * 
     * @brief Simple double-ended queue implementation.
     * @author Zach Goethel
     */
    template <class T>
    class deque
    {
        protected:
            int len = 0;

            deque_node<T>* head = new deque_node<T>;
            deque_node<T>* last = head;

        public:
            /**
             * @brief Appends the provided element to the queue.
             * @param element Element to append to the queue.
             */
            void add(T element)
            {
                last->next = new deque_node<T>;
                last = last->next;
                last->data = element;

                this->len++;
            }

            /**
             * @brief Pushes the provided element onto the stack.
             * @param element Element to push onto the stack.
             */
            void push(T element)
            {
                T* n = head->next;
                head->next = new deque_node<T>;
                head->next->next = n;
                head->next->data = element;

                this->len++;
            }

            /**
             * @brief Polls and removes the first element of the queue.
             * @return The removed front element of the queue.
             */
            T poll()
            {
                T val = head->next->data;
                auto d = head->next;
                head->next = head->next->next;
                delete d;

                this->len--;
                if (head->next == nullptr)
                    last = head;

                return val;
            }

            /**
             * @brief Pops the top element off of the stack.
             * @return The removed top element of the stack.
             */
            T pop()
            {
                return this->poll();
            }

            /**
             * @brief How many elements are in this double-ended queue.
             * @return The current size of the internal linked list.
             */
            int size()
            {
                return this->len;
            }
    };
}