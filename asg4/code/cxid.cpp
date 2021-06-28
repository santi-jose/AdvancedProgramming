// $Id: cxid.cpp,v 1.3 2020-12-12 22:09:29-08 - - $

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
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

void reply_ls (accepted_socket& client_sock, cxi_header& header) {
   const char* ls_cmd = "ls -l 2>&1";
   FILE* ls_pipe = popen (ls_cmd, "r"); //c library stuff
   if (ls_pipe == NULL) { 
      outlog << ls_cmd << ": " << strerror (errno) << endl;
      header.command = cxi_command::NAK;
      header.nbytes = htonl (errno); 
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   int status = pclose (ls_pipe);
   if (status < 0) outlog << ls_cmd << ": " << strerror (errno) << endl;
              else outlog << ls_cmd << ": exit " << (status >> 8)
                          << " signal " << (status & 0x7F)
                          << " core " << (status >> 7 & 1) << endl;
   header.command = cxi_command::LSOUT;
   header.nbytes = htonl (ls_output.size());
   memset (header.filename, 0, FILENAME_SIZE);
   outlog << "sending header " << header << endl;
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str(), ls_output.size());
   outlog << "sent " << ls_output.size() << " bytes" << endl;
}

void reply_get(accepted_socket& client_sock, cxi_header& header){
   struct stat stat_buf; //declare struct stat to call stat() on
   int status = stat (header.filename, &stat_buf); //call stat()
   if(status != 0){ //if the file does not exist
      outlog << "get: " << strerror (errno) << endl; //error message
      header.command = cxi_command::NAK; //set command to NAK
      header.nbytes = htonl(errno); //set nbytes to htonl(errno)
      send_packet (client_sock, &header, sizeof header); //send NAK
      return; //escape func
   } //else file exists
   header.command = cxi_command::FILEOUT; //send ACK when testing
   header.nbytes = htonl (stat_buf.st_size); //nbytes to stat_buf size
   outlog << "sending header " << header << endl;
   send_packet (client_sock, &header, sizeof header); //send FILEOUT
   auto buffer = make_unique<char[]>(stat_buf.st_size); //create buffer
   ifstream myFile (header.filename, ios::in | ios::binary); //ifstream
   myFile.read (buffer.get(), stat_buf.st_size); //read file into buffer
   send_packet(client_sock, buffer.get(), stat_buf.st_size); //send out
   outlog << "sent " << stat_buf.st_size << " bytes" << endl;
}

void reply_put(accepted_socket& client_sock, cxi_header& header){
   size_t host_nbytes = htonl (header.nbytes); //check nbytes
   auto buffer = make_unique<char[]> (host_nbytes + 1);  //make buffer
   recv_packet (client_sock, buffer.get(), host_nbytes);//(4) buf->file
   outlog << "received " << host_nbytes << " bytes" << endl;
   buffer[host_nbytes] = '\0';
   //cout << buffer.get(); //print out buffer containing received bytes
   ofstream myFile (header.filename, ios::out | ios::binary);
   if(!(myFile.write(buffer.get(), host_nbytes))){ //write failed
      outlog << "put: " << strerror(errno) << endl;
      header.command = cxi_command::NAK; //set command to NAK (5.1)
      header.nbytes = htonl(errno); //set nbytes to htonl(errno)
      send_packet (client_sock, &header, sizeof header); //send NAK
      return; //escape func
   } //else write succeeded
   header.command = cxi_command::ACK; //set command to ACK (5.2)
   header.nbytes = htonl (host_nbytes + 1); //nbytes to host_nbytes + 1
   outlog << "put " << header.filename << ": success" << endl;
   send_packet(client_sock, &header, sizeof header); //send ACK
}

void reply_rm(accepted_socket& client_sock, cxi_header& header){
   int status; //declare status
   status = unlink(header.filename); //unlink file
   if(status == -1){ //(3)
      outlog << "rm: " << strerror(errno) << endl;
      header.command = cxi_command::NAK; //set command to NAK
      outlog << "sending header " << header << endl;
      send_packet (client_sock, &header, sizeof header); //send NAK
   }else if(status == 0){
      outlog << "rm " << header.filename << ": success" << endl;
      header.command = cxi_command::ACK; //set command to ACK
      outlog << "sending header " << header << endl;
      send_packet (client_sock, &header, sizeof header); //send ACK
   }
}


void run_server (accepted_socket& client_sock) {
   outlog.execname (outlog.execname() + "*");
   outlog << "connected to " << to_string (client_sock) << endl;
   try {
      for (;;) {
         cxi_header header; 
         recv_packet (client_sock, &header, sizeof header);
         outlog << "received header " << header << endl;
         switch (header.command) { //check header command
            case cxi_command::LS: //LS command received
               reply_ls (client_sock, header);
               break;
            case cxi_command::GET: //GET command received
               reply_get (client_sock, header);
               break;
            case cxi_command::PUT: //PUT command received (2)
               reply_put (client_sock, header);
               break;
            case cxi_command::RM: //RM command received (2)
               reply_rm (client_sock, header);
               break;
            default:
               outlog << "invalid client header:" << header << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      outlog << "caught cxi_exit" << endl;
   }
   outlog << "finishing" << endl;
   throw cxi_exit(); //server doesn't return to main
}

void fork_cxiserver (server_socket& server, accepted_socket& accept) {
   pid_t pid = fork(); //clones itself
   if (pid == 0) { // child
      server.close(); //close deaemon socket
      run_server (accept); //run server
      throw cxi_exit(); //exit program when server exits
   }else { //not child
      accept.close();
      if (pid < 0) {
         outlog << "fork failed: " << strerror (errno) << endl;
      }else {
         outlog << "forked cxiserver pid " << pid << endl;
      }
   }
}


void reap_zombies() {
   for (;;) {
      int status;
      pid_t child = waitpid (-1, &status, WNOHANG);
      if (child <= 0) break;
      outlog << "child " << child
             << " exit " << (status >> 8)
             << " signal " << (status & 0x7F)
             << " core " << (status >> 7 & 1) << endl;
   }
}

void signal_handler (int signal) {
   outlog << "signal_handler: caught " << strsignal (signal) << endl;
   reap_zombies();
}

void signal_action (int signal, void (*handler) (int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigfillset (&action.sa_mask);
   action.sa_flags = 0;
   int rc = sigaction (signal, &action, nullptr);
   if (rc < 0) outlog << "sigaction " << strsignal (signal)
                      << " failed: " << strerror (errno) << endl;
}


int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   signal_action (SIGCHLD, signal_handler); //catch death of child
   in_port_t port = get_cxi_server_port (args, 0); //get server port
   try {
      server_socket listener (port); //listen in on port
      for (;;) {
         outlog << to_string (hostinfo()) << " accepting port "
             << to_string (port) << endl;
         accepted_socket client_sock; //accepted_socket talk to client
         for (;;) {
            try {
               listener.accept (client_sock); //accept call to client
               break; //break if listener.accept succeeds
            }catch (socket_sys_error& error) {
               switch (error.sys_errno) {
                  case EINTR:
                     outlog << "listener.accept caught "
                         << strerror (EINTR) << endl;
                     break;
                  default:
                     throw;
               }
            }
         }
         outlog << "accepted " << to_string (client_sock) << endl;
         try { //forks server w/ listener and client_socket
            fork_cxiserver (listener, client_sock);
            reap_zombies(); //gets rid of dead children
         }catch (socket_error& error) {
            outlog << error.what() << endl;
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

