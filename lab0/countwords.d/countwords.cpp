// $Id: countwords.cpp,v 1.1 2020-01-02 14:32:14-08 - - $

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>
using namespace std;

using wordcount_type = map<string,size_t>;

void scan (wordcount_type& words, istream& infile) {
   static const regex word_rx {"[[:alpha:]]+"};
   for (;;) {
      string line;
      getline (infile, line);
      if (infile.eof()) break;
      for (auto& chr: line) chr = tolower (chr);
      auto itor = sregex_iterator (line.begin(), line.end(), word_rx);
      for (; itor != sregex_iterator(); ++itor) {
         ++words[itor->str()];
      }
   }
}

int main (int argc, char** argv) {
   wordcount_type words;
   string exec_name {basename (argv[0])};
   int exit_status = EXIT_SUCCESS;
   vector<string> filenames (&argv[1], &argv[argc]);
   if (filenames.size() == 0) filenames.push_back ("-");
   for (const auto& filename: filenames) {
      if (filename == "-") scan (words, cin);
      else {
         ifstream infile (filename);
         if (infile) scan (words, infile);
         else {
            exit_status = EXIT_FAILURE;
            cerr << exec_name << ": " << filename << ": "
                 << strerror (errno) << endl;
         }
      }
   }
   for (const auto& word: words) {
      cout << word.first << " " << word.second << endl;
   }
   return exit_status;
}
