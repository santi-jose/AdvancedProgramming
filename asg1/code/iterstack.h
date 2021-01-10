// $Id: iterstack.h,v 1.1 2019-12-12 18:19:23-08 - - $

// 
// The class std::stack does not provide an iterator, which is
// needed for this class.  So, like std::stack, class iterstack
// is implemented on top of a container.
// 
// We use private inheritance because we want to restrict
// operations only to those few that are approved.  All functions
// are merely inherited from the container, with only ones needed
// being exported as public.
// 
// No implementation file is needed because all functions are
// inherited, and the convenience functions that are added are
// trivial, and so can be inline.
//
// Any underlying container which supports the necessary operations
// could be used, such as vector, list, or deque.
// 

#ifndef __ITERSTACK_H__
#define __ITERSTACK_H__

#include <vector>
using namespace std;

template <typename value_t, typename container = vector<value_t>>
class iterstack {
   public:
      using value_type = value_t;
      using const_iterator = typename container::const_reverse_iterator;
      using size_type = typename container::size_type;
   private:
      container stack;
   public:
      void clear() { stack.clear(); } //Removes all elements from the vector
      bool empty() const { return stack.empty(); } //Returns whether the vector is empty
      size_type size() const { return stack.size(); } //Returns the number of elements in the vector
      const_iterator begin() { return stack.crbegin(); } //Returns an iterator pointing to top of the stack
      const_iterator end() { return stack.crend(); } //Returns iterator pointing to "element" under bottom of the stack
      void push (const value_type& value) { stack.push_back (value); } //Adds a new element at the end of the vector
      void pop() { stack.pop_back(); } //Removes the last element in the vector
      const value_type& top() const { return stack.back(); } //Returns a reference to the top of the stack
};

#endif

