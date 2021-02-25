// $Id: listmap.h,v 1.25 2020-11-20 10:39:00-08 - - $

#ifndef __LISTMAP_H__
#define __LISTMAP_H__

#include "debug.h"
#include "xless.h"
#include "xpair.h"

#define SHOW_LINK(FLAG,PTR) { \
           DEBUGB (FLAG, #PTR << "=" << PTR \
                   << ": next=" << PTR->next \
                   << ", prev=" << PTR->prev); \
        }

template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
class listmap { //listmap class, like map from library
   public:
      using key_type = key_t; //key of pair
      using mapped_type = mapped_t; //value mapped to key
      using value_type = xpair<const key_type, mapped_type>; //pair
   private:
      less_t less; //less function object declaration
      struct node; //forward declaration of node struct to use in link
      struct link { //link struct
         node* next{}; //next node ptr
         node* prev{}; //prev node ptr
         link (node* next_, node* prev_): next(next_), prev(prev_){}
      }; //link constructor w/ default init params ^
      struct node: link { //node class derived from base class link
         value_type value{}; //value_type default constructor
         node (node* next_, node* prev_, const value_type& value_):
               link (next_, prev_), value(value_){}
      }; //node constructor w/ default init params ^
      node* anchor() { return static_cast<node*> (&anchor_); }
      link anchor_ {anchor(), anchor()}; //anchor link init
   public:
      class iterator; //forward declaration of nested class: iterator
      listmap(){}; //listmap default constructor, creates an anchor
      listmap (const listmap&); //listmap copy constructor
      listmap& operator= (const listmap&); //operator= copy assignment
      ~listmap(); //listmap destructor
      iterator insert (const value_type&); //insert (key, mapped) pair
      iterator find (const key_type&); //find key_type pair
      iterator erase (iterator position); //erase value_type @ given pos
      iterator begin() { return anchor()->next; } //return begin()
      iterator end() { return anchor(); } //return end() iterator
      bool empty() const { return anchor_.next == &anchor_; } //empty?
      operator bool() const { return not empty(); } //default bool
};


template <typename key_t, typename mapped_t, class less_t>
class listmap<key_t,mapped_t,less_t>::iterator {
   private:
      friend class listmap<key_t,mapped_t,less_t>; //friend of listmap
      listmap<key_t,mapped_t,less_t>::node* where {nullptr}; //ctor
      iterator (node* where_): where(where_){}; //iterator ctor
   public:
      iterator() {} //default constructor
      value_type& operator*() { //returns value_type&, (key,mapped)
         SHOW_LINK ('b', where);
         return where->value;
      }
      value_type* operator->() { return &(where->value); }
      iterator& operator++() { where = where->next; return *this; }
      iterator& operator--() { where = where->prev; return *this; }
      bool operator== (const iterator& that) const {
         return this->where == that.where;
      }
      bool operator!= (const iterator& that) const {
         return this->where != that.where;
      }
      operator bool() const{ return where != nullptr; }
};

#include "listmap.tcc" //including within header file
#endif

