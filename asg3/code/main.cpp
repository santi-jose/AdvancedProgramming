// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"
#include <libgen.h>

const string cin_name = "-";

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;
using int_str_map = listmap<int,string>;
using int_str_pair = int_str_map::value_type;

void scan_options (int argc, char** argv) { //checks for option flags
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void matchlines(const string& line, str_str_map& kv){
   //regex patterns
   regex comment_regex {R"(^\s*(#.*)?$)"}; 
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   regex nothing_regex {R"(^\s*$)"};
   
   smatch result;
   if (regex_search (line, result, comment_regex)) { 
      //Do nothing
   }else{
      if (regex_search (line, result, key_value_regex)) {
         // = : print all pairs in the listmap lexicographically
         if((regex_search(to_string(result[1]), nothing_regex)) 
            and (regex_search(to_string(result[2]), nothing_regex))){
            for(str_str_map::iterator pair = kv.begin();
                pair != kv.end(); ++pair){
               cout << (*pair).first << " = " << (*pair).second << endl;
            }
         //value, no key: print pairs with value lexicographic order
         }else if((regex_search(to_string(result[1]),nothing_regex)) 
            and(not(regex_search(to_string(result[2]),nothing_regex)))){
            for(str_str_map::iterator pair = kv.begin();
                pair != kv.end(); ++pair){
               if((*pair).second == to_string(result[2])){
                  cout<<(*pair).first <<" = "<<(*pair).second<<endl;
               }
            }
         //key, no value: delete (key,mapped)
         }else if((not
                  (regex_search(to_string(result[1]),nothing_regex))) 
            and (regex_search(to_string(result[2]), nothing_regex))){
            str_str_map::iterator pair = kv.find(to_string(result[1]));
            if(pair != kv.end()){
               kv.erase(pair);
         }
         //key & value: insert (key,mapped) or replace mapped of key
         }else{
            str_str_pair new_pair (to_string(result[1]),
                                   to_string(result[2]));
            str_str_map::iterator pair = kv.find(to_string(result[1]));
            if(pair != kv.end()){
               kv.erase(pair);
            }
            kv.insert(new_pair);
            cout << new_pair.first << " = " << new_pair.second << endl;
         }
      //print key and value field
      }else if (regex_search (line, result, trimmed_regex)) {
         str_str_map::iterator pair = kv.find(to_string(result[1]));
         if(pair != kv.end()){
            cout << (*pair).first << " = " << (*pair).second << endl;
         }else{
            sys_info::exit_status(1);
            cerr << result[1] << ": key not found" << endl;
         }
      }else {
         assert (false and "This can not happen.");
      }
   }
}

void catfile(istream& infile,const string& filename,str_str_map& kv){
   int line_num = 1; //initialize line count
   for(;;) {
      string line;
      getline (infile, line);
      if (infile.eof()) break;
      cout << filename << ": " << line_num << ": " << line << endl;
      matchlines(line, kv); //call matchlines
      ++line_num; //increment line count
   }
}

int main (int argc, char** argv) { //catfile.cpp: how to check inputs
   sys_info::execname (argv[0]); //store execname
   sys_info::exit_status(0);
   scan_options (argc, argv);

   str_str_map keyval; //listmap<string, string>
   int_str_map files; //listmap<int, string>
   
   //store filenames into files listmap<int, string>
   for ( int i = 1 ; i < argc ; ++i) {
      int_str_pair file_pair( i , to_string<char*>(argv[i]) );
      files.insert(file_pair); //insert pair
   }
   
   //check for empty files listmap<int, string>
   if(files.empty()){ //if files listmap is empty
      int_str_pair file_pair(1, cin_name); //create pair
      files.insert(file_pair); //insert cin_name
   }
   
   //iterate through files 
   for(int_str_map::iterator file = files.begin();
       file != files.end(); ++file){
      int_str_pair file_pair = *file;
      if(file_pair.second == cin_name){ //if we are reading from cin
         catfile(cin, file_pair.second, keyval);
      }else{
         ifstream infile(file_pair.second); //construct input filestream
         if(infile.fail()){
            sys_info::exit_status(1); //set exit_status to 1
            cerr << sys_info::execname() << ": " << file_pair.second
            << ": " << "No such file or directory" << endl;
         }else{
            catfile(infile, file_pair.second, keyval);
            infile.close();
         }
      }
   }
   
   return sys_info::exit_status();
}


