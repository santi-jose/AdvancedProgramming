// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $

#include "commands.h"
#include "debug.h"
#include <iomanip>
#include <cstring>

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
   
   //state.root()->get_contents()->readfile();
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()>2){
      cerr << "cd: more than one operand" << endl;
   }else{
      if(words.size()==1){ //no operands
         state.cwd_() = state.root_(); //cd to root
      }else{ //an operand exists, find path
         inode_ptr save_cwd = state.cwd_();
         state.cwd_() = find_path(state , words[1]);
         if(state.cwd_() == nullptr){
            state.cwd_() = save_cwd;
            cerr << words[1] << ": No such pathname" << endl;
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
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      cout << "/:" << endl;
      map<string, inode_ptr> cwd_dirents
                  = state.cwd_()->get_contents()->get_dirents();
      for(auto it = cwd_dirents.begin();
               it != cwd_dirents.end();
               ++it){
         cout << setw(10) << it->second->get_inode_nr();
         cout <<  setw(10) << it->second->get_contents()->size();
         cout << setw(10) << it->first;
         if(it->second->get_contents()->is_dir()){
            cout <<  "/";
         }
         //cout << setw(0);
         cout << endl;
      }
      cout << setw(0);
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //error if file/directory of same name already exists
   if(words.size()<2){
      cerr << "mkdir: missing operand" << endl;
   }else{
      if(words.size() == 2){
         state.cwd_()->get_contents()->mkdir(words[1]);
      }else{
         cerr << "mkdir: only one operand allowed" << endl;  
      }
   }
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
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
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

//helper functions

//find_path sets the cwd inode_ptr to what's given by pathname string
//and returns it, if no pathname exists, it sets cwd to nullptr
inode_ptr& find_path(inode_state& state, const string& line){
   inode_ptr path_node;
   //string root_str {"/"};
   if(line.at(0) == '/'){ //if we start at root
      path_node = state.root_();
      wordvec path = split(line , "/");
      if(path.size()==1){
         return state.root_();
      }
      for(auto it = path.cbegin(); it != path.cend(); ++it){
         map<string, inode_ptr> temp_dirents;
         temp_dirents = path_node->get_contents()->get_dirents();
         if(temp_dirents.find(*it) == temp_dirents.end()){
            state.cwd_() = nullptr;
            break;
            //cerr << (*it) << ": No such file or directory" << endl;
            //break;
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
            //cerr << (*it) << ": No such file or directory" << endl;
            //break;
         }else{
            path_node = temp_dirents.at(*it);
         }
      state.cwd_() = path_node;
      }
   }
   return state.cwd_();
}

