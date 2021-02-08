// $Id: file_sys.cpp,v 1.9 2020-10-26 21:32:08-07 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   //DEBUGF ('i', "root = " << root << ", cwd = " << cwd
   //       << ", prompt = \"" << prompt() << "\"");

   //initialize root & cwd inode_ptr's
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   root->get_contents()->get_dirents().emplace("..", root);
   root->get_contents()->get_dirents().emplace(".", root);
   root->get_contents()->set_name("/");
   cwd = root;
   
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

inode_state::~inode_state() {

}

inode_ptr& inode_state::root_() { return root; }

inode_ptr& inode_state::cwd_() { return cwd; }

const string& inode_state::prompt() const { return prompt_; }

void inode_state::prompt(const string& new_prompt){ 
   prompt_ = new_prompt; 
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

base_file_ptr& inode::get_contents(){
   return contents;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

map<string, inode_ptr>& base_file::get_dirents(){
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   for(auto it = data.cbegin();it != data.cend(); ++it){
      size = size + (*it).length();
   }
   if (data.size() > 0){
      size =  size + (data.size() - 1);
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   for(auto it = (words.cbegin()+2); it != words.cend(); ++it){
      data.push_back(*it);
   }
}

bool plain_file::is_dir() const{
   return false;
}

void plain_file::set_name (const string& file_name){
   name = file_name;
}

const string& plain_file::get_name (){
   return name;
}

size_t directory::size() const {
   size_t size {dirents.size()};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   //cant delete . and ..; maybe do this in commands
    //does filename exist? dirents.find(filename)
   if(dirents.find(filename)!=dirents.end()){
      dirents.erase(filename);
   }
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   //does dirname exist? dirents.find(dirname)
   if(dirents.find(dirname) == dirents.end()){
      inode_ptr new_dir = make_shared<inode>(file_type::DIRECTORY_TYPE);
      inode_ptr parent = dirents.at(".");
      new_dir->get_contents()->get_dirents().emplace("..", parent);
      new_dir->get_contents()->get_dirents().emplace(".", new_dir);
      new_dir->get_contents()->set_name(dirname);
      dirents.emplace(dirname, new_dir);
      return new_dir; 
   }else{
      cerr << "mkdir: cannot make directory '"
           << dirname << "': File exists" << endl;
      return nullptr;
   }
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   //does filename exist? dirents.find(filename)
   if(dirents.find(filename) == dirents.end()){
      inode_ptr new_file = make_shared<inode>(file_type::PLAIN_TYPE);
      new_file->get_contents()->set_name(filename);
      dirents.emplace(filename, new_file);
      return new_file;
   }else{
      cerr << "mkfile: cannot make file '"
           << filename << "': File exists" << endl;
      return nullptr;
   }
}

bool directory::is_dir() const{
   return true;
}

map<string, inode_ptr>& directory::get_dirents(){
   return dirents;
}

void directory::set_name (const string& file_name){
   name = file_name;
}

const string& directory::get_name (){
   return name;
}
