#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  std::string server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = argv[2];
  username = argv[3];

  // TODO: connect to server
  int fd = open_clientfd(server_hostname.c_str(), server_port.c_str());
  if (fd < 0) {
    // could not connect to server
    std::cerr << "Error failed to connect to server";
    return 1;
  }

  // TODO: send slogin message
  std::string msg = "slogin:" + username + "\n";
  rio_writen(fd, msg.c_str(), msg.size()); 

  // preparing to read reply
  rio_t rio;
  rio_readinitb(&rio, fd);
  char buf[Message::MAX_LEN + 1]; 

  ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  if (n <= 0) {
    std::cerr << "Failed to read slogin response";
    return 1;
  }

  std::string line(buf, n);
  size_t seperation = line.find(':');



  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  return 0;
}
