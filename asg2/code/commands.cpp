// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $

#include "commands.h"
#include "debug.h"
#include <iomanip>
#include <cstring>
#include <cctype>

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() < 2){
      cerr << "cat: missing operand" << endl;
   }else{
      inode_ptr save_cwd = state.cwd_();
      for(auto it = (words.cbegin() + 1); it != words.cend(); ++it){
         state.cwd_() = find_path(state , (*it));
         if(state.cwd_() == nullptr){
            state.cwd_() = save_cwd;
            cerr << "cat:" << (*it) << ": no such file" << endl;
         }else if (state.cwd_()->get_contents()->is_dir()){
            state.cwd_() = save_cwd;
            cerr << "cat: pathname is a directory" << endl;
         }else{
            wordvec data;
            data = state.cwd_()->get_contents()->readfile();
            for(auto i = data.cbegin(); i != data.cend(); ++i){
               cout << (*i);
               cout << " ";
            }
            cout << endl;
         }
         state.cwd_() = save_cwd;
      }
   }
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()>2){
      cerr << "cd: more than one operand" << endl;
   }else{
      if(words.size()==1 //no operands or root directory input
         or ((words.size()==2)and (words[1].at(0)=='/'))){
         state.cwd_() = state.root_(); //cd to root
      }else{ //an operand exists, find path
         inode_ptr save_cwd = state.cwd_();
         state.cwd_() = find_path(state , words[1]);
         if(state.cwd_() == nullptr
             or (!(state.cwd_()->get_contents()->is_dir()))){
            state.cwd_() = save_cwd;
            cerr << words[1] << ": no such pathname" << endl;
         }
      }
   }
}

void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   bool digit = true;
   
   if(words.size()==1){
      exec::status(0);
   }else if(words.size()==2){
      for(auto it = words[1].cbegin();
               it != words[1].cend();
               ++it){
         if(!isdigit(*it)){
            digit = false;
            break;
         }
      }
      if(digit){
         exec::status(stoi(words[1]));
      }else{
         exec::status(127);
      }
   }
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      map<string, inode_ptr> cwd_dirents;
      cwd_dirents = state.cwd_()->get_contents()->get_dirents();
      cout << state.cwd_()->get_contents()->get_name() << ":" << endl;
      for(auto it = cwd_dirents.cbegin();
               it != cwd_dirents.cend();
               ++it){
         cout << setw(10) << it->second->get_inode_nr();
         cout << setw(10) << it->second->get_contents()->size();
         cout << setw(10) << it->first;
         if(it->second->get_contents()->is_dir()){
            cout <<  "/";
         }
         cout << endl;
      }
      cout << setw(0);
   }else if((words.size() == 2) and (words[1] == string{"/"})){
      map<string, inode_ptr> root_dirents;
      root_dirents = state.root_()->get_contents()->get_dirents();
      cout << state.root_()->get_contents()->get_name() << ":" << endl;
      for(auto it = root_dirents.cbegin(); 
               it!= root_dirents.cend();
               ++it){
         cout << setw(10) << it->second->get_inode_nr();
         cout << setw(10) << it->second->get_contents()->size();
         cout << setw(10) << it->first;
         if(it->second->get_contents()->is_dir()){
            cout << "/";
         }
         cout << endl;
      }
      cout << setw(0);
   }else{ //ls is called with pathname...
      inode_ptr save_cwd = state.cwd_();
      for(auto it = (words.cbegin()+1); it != words.cend(); ++it){
         state.cwd_() = find_path (state , (*it)); //find_path
         if(state.cwd_() == nullptr
             or (!(state.cwd_()->get_contents()->is_dir()))){
            state.cwd_() = save_cwd;
            cerr << "ls:"<< (*it) <<": no such pathname" << endl;
         }else{
            map<string, inode_ptr> cwd_dirents;
            cwd_dirents = state.cwd_()->get_contents()->get_dirents();
            cout << (*(it)) << ":" << endl;
            for(auto i = cwd_dirents.cbegin();
                     i != cwd_dirents.cend();
                     ++i){
               cout << setw(10) << i->second->get_inode_nr();
               cout << setw(10) << i->second->get_contents()->size();
               cout << setw(10) << i->first;
               if(i->second->get_contents()->is_dir()){
                  cout <<  "/";
               }
            cout << endl;
            }
         }
         cout << setw(0);
      }
      state.cwd_() = save_cwd;
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() < 2){
      cerr << "make: missing operand" << endl;
   }else{
      wordvec path_vec = split(words[1] , "/");
      if(path_vec.size() > 1){ //pathname given to mkfile
         string file_name = (*(path_vec.crbegin()));
         path_vec.pop_back();
         string path_str;
         if(words[1].at(0) == '/'){
            path_str.append("/");
         }
         for(auto it = path_vec.cbegin();
                  it != path_vec.cend();
                  ++it){
            path_str.append(*it);
            path_str.append("/");
         }
         inode_ptr save_cwd = state.cwd_();
         state.cwd_() = find_path(state , path_str);
         if(state.cwd_() == nullptr 
            or (!(state.cwd_()->get_contents()->is_dir()))){
            state.cwd_() = save_cwd;
            cerr << "make: no such pathname" << endl;
         }else{
            state.cwd_()->get_contents()->mkfile(file_name);
            if(words.size() > 2){ //contents to file given
               state.cwd_() = find_path(state, file_name);
               state.cwd_()->get_contents()->writefile(words);
            }
         }
         state.cwd_() = save_cwd;
      }else{ //only given name of file to make
         state.cwd_()->get_contents()->mkfile(path_vec[0]);
         if(words.size() > 2){
            inode_ptr save_cwd = state.cwd_();
            state.cwd_() = find_path(state, path_vec[0]);
            state.cwd_()->get_contents()->writefile(words);
            state.cwd_() = save_cwd;
         }
      }
   }
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //error if file/directory of same name already exists
   if(words.size()<2){
      cerr << "mkdir: missing operand" << endl;
   }else{
      if(words.size() == 2){ //we have a pathname to create dir at
         wordvec path_vec = split (words[1] , "/");
         if(path_vec.size() > 1){ //pathname given to mkdir
            string dir_name = (*(path_vec.crbegin()));
            path_vec.pop_back(); //delete directory to create from path
            string path_str;
            if(words[1].at(0) == '/'){
               path_str.append("/");
            }
            for(auto it = path_vec.cbegin();
                     it != path_vec.cend();
                     ++it){
               path_str.append(*it);
               path_str.append("/") ;
            }
            inode_ptr save_cwd = state.cwd_();
            state.cwd_() = find_path(state , path_str);
            if(state.cwd_() == nullptr 
                or (!(state.cwd_()->get_contents()->is_dir()))){
               state.cwd_() = save_cwd;
               cerr << "mkdir: no such pathname" << endl;
            }else{
               state.cwd_()->get_contents()->mkdir(dir_name);
            }
         state.cwd_() = save_cwd;
         }else{ //only given name of dir to create
            state.cwd_()->get_contents()->mkdir(path_vec[0]);
         }
      }else{
         cerr << "mkdir: only one operand allowed" << endl;  
      }
   }
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string new_prompt;
   for(auto it = (words.cbegin()+1); it != words.cend(); ++it){
      new_prompt.append(*it);
      new_prompt.append(" ");
   }
   state.prompt(new_prompt);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(state.cwd_() == state.root_()){
      cout << state.root_()->get_contents()->get_name() << endl;
   }else{
      inode_ptr temp = state.cwd_(); //store cwd
      wordvec pwd_vec; //pwd wordvec to print
      while(temp != state.root_()){
         pwd_vec.push_back(temp->get_contents()->get_name());
         temp = temp->get_contents()->get_dirents().at("..");
      }
      for(auto it = pwd_vec.crbegin();it != pwd_vec.crend(); ++it){
         cout << "/" ;
         cout << (*it) ;
      }
      cout<<endl;
   }
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 2){ //rm takes one operand
      if(words[1] == string{"."} or words[1] == string{".."}){
         cerr << "rm: can't remove . or .. directories" << endl;
      }else{
         inode_ptr save_cwd = state.cwd_();
         state.cwd_() = find_path(state , words[1]);
         if(state.cwd_() == nullptr){
            state.cwd_() = save_cwd;
            cerr << "rm: no such pathname" << endl;
         }else{
            base_file_ptr file = state.cwd_()->get_contents();
            if(file->is_dir()){ //file to remove is directory
               if(state.cwd_() == state.root_()){ //root directory
                  cerr << "rm: can't remove root" << endl;
               }else{
                  if(file->size() == 2){ //if we have 2 dirents
                     string dir_name = file->get_name();
                     inode_ptr parent;
                     parent = file->get_dirents().at("..");
                     file->get_dirents().erase(".");
                     file->get_dirents().erase("..");
                     file = parent->get_contents();
                     file->remove(dir_name);
                  }else{
                     cerr << "rm: can't delete non-empty directory";
                     cerr << endl;
                  }
               }
            }else{ //file to remove is plain_file
               state.cwd_() = save_cwd;
               wordvec path_vec = split(words[1] , "/");
               if(path_vec.size() > 1){ //pathname of file to rm
                  string file_name = (*(path_vec.crbegin()));
                  path_vec.pop_back();
                  string path_str;
                  if(words[1].at(0) == '/'){
                     path_str.append("/");
                  }
                  for(auto it = path_vec.cbegin();
                           it != path_vec.cend();
                           ++it){
                     path_str.append(*it);
                     path_str.append("/");
                  }
                  state.cwd_() = find_path(state , path_str);
                  if(state.cwd_() == nullptr 
                     or (!(state.cwd_()->get_contents()->is_dir()))){
                     state.cwd_() = save_cwd;
                     cerr << "make: no such pathname" << endl;
                  }else{
                     base_file_ptr parent_ptr;
                     parent_ptr = state.cwd_()->get_contents();
                     parent_ptr->get_dirents().erase(file_name);
                  }
               }else{ //only given name of file to remove
                  base_file_ptr parent_ptr;
                  parent_ptr = state.cwd_()->get_contents();
                  parent_ptr->get_dirents().erase(words[1]);
               }
            }
            state.cwd_() = save_cwd;
         }
      }
   }else{
      cerr << "rm: one operand required" << endl;
   }
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 2){ //rm takes one operand
      if(words[1] == string{"."} or words[1] == string{".."}){
         cerr << "rmr: can't remove . or .. directories" << endl;
      }else{
         inode_ptr save_cwd = state.cwd_();
         state.cwd_() = find_path(state, words[1]);
         if(state.cwd_() == nullptr){
            state.cwd_() = save_cwd;
            cerr << "rmr: no such pathname" << endl;
         }else{
            base_file_ptr file = state.cwd_()->get_contents();
            if(file->is_dir()){
               inode_ptr parent = file->get_dirents().at("..");
               rm_recur(state);
               base_file_ptr parent_file = parent->get_contents();
               parent_file->get_dirents().erase(file->get_name());
            }else{
               state.cwd_() = save_cwd;
               wordvec path_vec = split(words[1] , "/");
               if(path_vec.size() > 1){ //pathname of file to rm
                  string file_name = (*(path_vec.crbegin()));
                  path_vec.pop_back();
                  string path_str;
                  if(words[1].at(0) == '/'){
                     path_str.append("/");
                  }
                  for(auto it = path_vec.cbegin();
                           it != path_vec.cend();
                           ++it){
                     path_str.append(*it);
                     path_str.append("/");
                  }
                  state.cwd_() = find_path(state , path_str);
                  if(state.cwd_() == nullptr 
                     or (!(state.cwd_()->get_contents()->is_dir()))){
                     state.cwd_() = save_cwd;
                     cerr << "make: no such pathname" << endl;
                  }else{
                     base_file_ptr parent_ptr;
                     parent_ptr = state.cwd_()->get_contents();
                     parent_ptr->get_dirents().erase(file_name);
                  }
               }else{ //only given name of file to remove
                  base_file_ptr parent_ptr;
                  parent_ptr = state.cwd_()->get_contents();
                  parent_ptr->get_dirents().erase(words[1]);
               }
            }
         }
         state.cwd_() = save_cwd;
      }
   }else{
      cerr << "rmr: one operand required" << endl;
   }
}

//helper functions

//find_path sets the cwd inode_ptr to what's given by pathname string
//and returns it, if no pathname exists, it sets cwd to nullptr
inode_ptr& find_path(inode_state& state, const string& line){
   inode_ptr path_node;
   inode_ptr save_cwd = state.cwd_();
   if(line.at(0) == '/'){ //if we start at root
      path_node = state.root_();
      wordvec path = split(line , "/");
      for(auto it = path.cbegin(); it != path.cend(); ++it){
         map<string, inode_ptr> temp_dirents;
         temp_dirents = path_node->get_contents()->get_dirents();
         if(temp_dirents.find(*it) == temp_dirents.end()){
            state.cwd_() = nullptr;
            break;
         }else{
            path_node = temp_dirents.at(*it);
         }
         state.cwd_() = path_node;
      }
   }else{ //else we start at cwd
      path_node = state.cwd_();
      wordvec path = split(line , "/");
      for(auto it = path.cbegin(); it!= path.cend(); ++it){
         map<string, inode_ptr> temp_dirents;
         temp_dirents = path_node->get_contents()->get_dirents();
         if(temp_dirents.find(*it) == temp_dirents.end()){
            state.cwd_() = nullptr;
            break;
         }else{
            path_node = temp_dirents.at(*it);
         }
         state.cwd_() = path_node;
      }
   }
   return state.cwd_();
}

//rm_recur is a recursive function to call rm on directories
//recursively
void rm_recur(inode_state& state){
   base_file_ptr file;
   file = state.cwd_()->get_contents();
   for(auto it = file->get_dirents().cbegin(); 
            it != file->get_dirents().cend();
            ++it){
      if((it->first == string{".."}) or (it->first == string{"."})){
         continue;
      }else{
         if(it->second->get_contents()->is_dir()){ //dir
            state.cwd_() = it->second; //go to dir child
            rm_recur(state); //recur
            file->get_dirents().erase(it->first); //erase dir
         }else{ //plainfile
            file->get_dirents().erase(it->first);
         }
      }
   }
   file->get_dirents().erase(string{".."});
   file->get_dirents().erase(string{"."});
}

//ls_recur is a recursive function to call ls on directories
//recursively


