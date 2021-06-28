// $Id: cxi.cpp,v 1.1 2020-11-22 16:51:43-08 - - $

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"get" , cxi_command::GET },
   {"put" , cxi_command::PUT },
   {"rm"  , cxi_command::RM  }
};

//help message string
static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() { //print help message
   cout << help;
}

void cxi_ls (client_socket& server) { //already implemented
   cxi_header header; //create cxi_header
   header.command = cxi_command::LS; //set header command to LS
   outlog << "sending header " << header << endl; //outlog message
   send_packet (server, &header, sizeof header); //send packet
   recv_packet (server, &header, sizeof header); //receive packet
   outlog << "received header " << header << endl; //outlog message
   if (header.command != cxi_command::LSOUT) { //didn't receive LSOUT
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl; //error message
   }else { //received LSOUT
      size_t host_nbytes = ntohl (header.nbytes); //store # of bytes
      auto buffer = make_unique<char[]> (host_nbytes + 1); //buffer
      recv_packet (server, buffer.get(), host_nbytes); //recv pack
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0'; //plut buffer with null plug
      cout << buffer.get(); //print out buffer
   }
}

void cxi_get(client_socket& server, const string& filename){
   cxi_header header; //create cxi_header
   header.command = cxi_command::GET; //set header command to GET
   char fn[59]; //declare filename as char[]
   int index = 0; //initialize index
   for(auto c:filename){ //convert filename to char[]
      fn[index] = c;
      index++;
   }
   for(; index < 59; index++) fn[index]='\0'; //fill rest of fn w/ null
   for(int i = 0; i < 59; i++) header.filename[i] = fn[i]; //header fn
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header); //send GET
   recv_packet (server, &header, sizeof header); //receive FILEOUT
   outlog << "received header " << header << endl; 
   if(header.command != cxi_command::FILEOUT){ //didn't receive FILEOUT
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   }else{ //received FILEOUT
      size_t host_nbytes = ntohl (header.nbytes); 
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0';
      //cout << buffer.get(); //print out buffer containing
      ofstream myFile (header.filename, ios::out | ios::binary);
      if(!(myFile.write(buffer.get(), host_nbytes))){ //write failed
         outlog << "get: " << strerror(errno) << endl;
      }else{ //else write succeeded
         outlog << "get " << filename << ": success" << endl;
      }
   }
}

void cxi_put(client_socket& server, const string& filename){
   char fn[59]; //declare fn as char[]
   int index = 0; //initialize index
   for(auto c:filename){ //convert filename string to char[] fn
      fn[index] = c;
      index++;
   }
   for(; index < 59; index++) fn[index] = '\0'; //fil rest of fn w/ null
   struct stat stat_buf; //declare stat_buf 
   int status = stat (fn, &stat_buf); //get fn status
   if(status != 0){ //if the file does not exist
      outlog << "put : File not found: " << strerror (errno) << endl;
      return;
   } //else file exists
   cxi_header header; //create cxi_header
   header.command = cxi_command::PUT; //set header command to PUT
   header.nbytes = ntohl(stat_buf.st_size); //set header nbytes
   for(int i = 0; i < 59; i++) header.filename[i] = fn[i]; //header fn
   outlog << "sending header " << header << endl; 
   send_packet (server, &header, sizeof header); //send PUT (1)
   auto buffer = make_unique<char[]>(stat_buf.st_size); //create buffer
   ifstream myFile (header.filename, ios::in | ios::binary); //ifstream
   myFile.read (buffer.get(), stat_buf.st_size); //read file into buffer
   outlog << "sending " << stat_buf.st_size << " bytes" << endl; 
   send_packet (server, buffer.get(), stat_buf.st_size); //send buf (3)
   recv_packet (server, &header, sizeof header); //receive ACK/NAK (6)
   outlog << "received header " << header << endl;
   if(header.command != cxi_command::ACK){ //print success/error
      outlog << "sent PUT, server did not return ACK" << endl;
   }else{
      outlog << "sent PUT, server returned ACK" << endl;
   }
}

void cxi_rm(client_socket& server, const string& filename){
   cxi_header header; //create cxi_header
   header.command = cxi_command::RM; //set header command to RM
   char fn[59]; //declare fn as char[]
   int index = 0; //initialize index
   for(auto c:filename){ //convert filename string to char[] fn
      fn[index] = c;
      index++; 
   }
   for(; index < 59; index++) fn[index] = '\0'; //fill rest of fn
   for(int i = 0; i < 59; i++) header.filename[i] = fn[i]; //header fn
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header); //send RM (1)
   recv_packet (server, &header, sizeof header); //receive ACK/NAK (4)
   outlog << "received header " << header << endl;
   if(header.command != cxi_command::ACK){
      outlog << "sent RM, server did not return ACK" << endl;
   }else{
      outlog << "sent RM, server returned ACK" << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cxi_exit();
}

vector <string> split (const string& line, const string& delimiter){
   vector<string> words;
   size_t end = 0;
   
   //Loop over the string, splitting out words, and for each word
   //when found, append it to the output vector of strings
   for(;;){
      size_t start = line.find_first_not_of (delimiter, end);
      if(start == string::npos) break;
      end = line.find_first_of(delimiter, start);
      words.push_back(line.substr (start, end - start));
   }
   return words;
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0])); //store execname
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cxi_server_host (args, 0);
   in_port_t port = get_cxi_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line; //line string
         string cmd_str; //command string
         string fn_str; //filename string
         vector<string> client_input; //create vector to store line args
         getline (cin, line); //get line from stdin
         client_input = split(line, " "); //split line via spaces
         cmd_str = client_input[0]; //store command name
         if(client_input.size() > 1){ //if we have two input args
            fn_str = client_input[1]; //store filename
         }
         if (cin.eof()) throw cxi_exit(); //if we reach eof, exit
         outlog << "command " << cmd_str << endl; 
         const auto& itor = command_map.find (cmd_str); //find command
         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;
         switch (cmd) { //check for command input
            case cxi_command::EXIT: //EXIT command
               throw cxi_exit();
               break;
            case cxi_command::HELP: //HELP command
               cxi_help();
               break;
            case cxi_command::LS: //LS command
               cxi_ls (server);
               break;
            case cxi_command::GET: //GET command
               cxi_get (server, fn_str);
               break;
            case cxi_command::PUT: //PUT command
               cxi_put (server, fn_str);
               break;
            case cxi_command::RM: //RM command
               cxi_rm (server, fn_str);
               break;
            default: //invalid command
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      outlog << "caught cxi_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

