// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   while(!empty()){erase(begin());} 
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   auto it = begin(); //store begin iterator
   for(auto e = end(); it!= e; ++it){ //iterate through listmap
      if(less(pair.first, ((*it).first))){ //pair.first < it->first
         break; //break
      }
   }
   //it now points at element after the pos where we want to insert
   node* a_node = it.where; //store node* to node after insert pos
   node* n_node = new node(a_node , a_node->prev , pair); //new node
   a_node->prev->next = n_node; //point a_node->prev's next to n_node
   a_node->prev = n_node; //point a_node's prev to n_node
   return n_node; //return pointer to new node
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   for(auto it = begin(); it!=end(); ++it){ //iterate through listmap
      if( (not(less(it->first, that))) 
           and (not(less(that, it->first))) ){
         return it; //use less obj func to determine equality, return it
      }
   } //if we escape for loop, no match to input key found
   return end(); //return end() 
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   node* to_delete = position.where; //store node* to delete
   //link before and after node's of node to delete
   node* n_node = to_delete->next; //node* of to_delete's next node
   node* p_node = to_delete->prev; //node* of to_delete's prev node
   p_node->next = n_node; //make p_node point forward to n_node
   n_node->prev = p_node; //make n_node point back to p_node
   //delete current node
   to_delete->next = nullptr;
   to_delete->prev = nullptr;
   delete to_delete;
   return n_node; //return next node of deleted node
}


