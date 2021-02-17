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
   //while(!empty()){erase(begin());}
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   auto insert_after = end(); //modify lexicographic insert: change end()
   auto insert_before = ++insert_after;
   node* insert = 
       new node(insert_before.where, insert_after.where,pair);
   insert_before.where->prev = insert_;
   insert_after.where->next = insert_;
   return iterator(iterator(insert_));
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   for(auto itor = begin(); itor!=end(); ++itor){
      if(not less(itor->first, that) and not less(that, itor->second)){
	     return itor;
	  }
   }
   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   //save current node
   node* to_delete = position.where;
   //link before and after
   (to_delete->prev)->next = to_delete->next;
   (to_delete->next)->prev = to_deleter->prev;
   //delete current node
   to_delete->next = nullptr;
   to_delete->prev = nullptr;
   delete to_delete;
   return iterator;
}


