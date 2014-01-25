/*
 * Singly-linked list.
 * Copyright (C) 2013 Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <cstdlib>
#include <cassert>

namespace uavcan
{
/**
 * Classes that are supposed to be linked-listed should derive this.
 */
template <typename T>
class LinkedListNode
{
    T* next_;

protected:
    LinkedListNode()
    : next_(NULL)
    { }

public:
    T* getNextListNode() const { return next_; }

    void setNextListNode(T* node)
    {
        next_ = node;
    }
};

/**
 * Linked list root.
 */
template <typename T>
class LinkedListRoot
{
    T* root_;

public:
    LinkedListRoot()
    : root_(NULL)
    { }

    T* get() const { return root_; }

    unsigned int length() const
    {
        T* node = root_;
        unsigned int cnt = 0;
        while (node)
        {
            cnt++;
            node = node->getNextListNode();
        }
        return cnt;
    }

    void insert(T* node)
    {
        remove(node);  // Making sure there will be no loops
        node->setNextListNode(root_);
        root_ = node;
    }

    bool remove(const T* node)
    {
        if (root_ == NULL || node == NULL)
            return false;

        if (root_ == node)
        {
            root_ = root_->getNextListNode();
            return true;
        }
        else
        {
            T* p = root_;
            while (p->getNextListNode())
            {
                if (p->getNextListNode() == node)
                {
                    p->setNextListNode(p->getNextListNode()->getNextListNode());
                    return true;
                }
                p = p->getNextListNode();
            }
            return false;
        }
    }

    template <typename Fun>
    void map(Fun& fun)
    {
        T* node = root_;
        while (node)
        {
            fun(node);
            node = node->getNextListNode();
        }
    }
};

}
