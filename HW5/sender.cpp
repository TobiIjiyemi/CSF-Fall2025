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
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // TODO: connect to server
  Connection conn; 
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    std::cerr << "Error: failed to connect to server\n";
    return 1;
  }

  // TODO: send slogin message
  Message login("slogin", username);
  if(!conn.send(login)) {
    std::cerr << "Error: failed to send slogin message\n";
    return 1;
  }

  Message response;
  if(!conn.receive(response)) {
    std::cerr << "Error: failed to read slogin response\n";
    return 1;
  }

  // preparing to read reply

  




  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  return 0;
}
