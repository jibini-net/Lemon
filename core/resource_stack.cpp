#include "resource_stack.h"

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
    resource_stack::resource_stack()
    {
        // Push initial empty bucket onto the top of the stack
        this->push();
    }

    void resource_stack::push()
    {
        // Push next empty bucket onto the top of the stack
        // this->allocation_stack.push_back(std::vector<std::function<void()>>());
        this->allocation_stack.emplace_back();
    }

    void resource_stack::pop()
    {
        // Return if there are no buckets
        if (this->allocation_stack.size() == 0)
            return;

        // Retrieve the top of the stack
        auto current = this->allocation_stack.back();
        // Delete all elements in the popped bucket
        for (int i = 0; i < current.size(); i++)
            (current[i])();
        // Pop the top of the stack
        this->allocation_stack.pop_back();
    }
}