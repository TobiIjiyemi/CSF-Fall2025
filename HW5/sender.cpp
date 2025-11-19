#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv)
{
  if (argc != 4)
  {
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
    exit(1);
  }

  // TODO: send slogin message
  Message login(TAG_SLOGIN, username);
  if (!conn.send(login)) {
    std::cerr << "Error: failed to send slogin message\n";
    conn.close();
    return 1;
  }

  Message response;
  if (!conn.receive(response)) {
    std::cerr << "Error: failed to read slogin response\n";
    conn.close();
    return 1;
  }

  if (response.tag == TAG_ERR) {
    std::cerr << response.data << "\n";
    conn.close();
    return 1;
  }

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  std::string input;
  while (true) {
    std::cout << " > ";
    if (!std::getline(std::cin, input))
      break;
    if (!input.empty() && input[0] == '/') {
      std::string command;
      std::string arg;
      size_t space = input.find(' ');
      if (space != std::string::npos) {
        command = input.substr(0, space);
        arg = input.substr(space + 1);
      }
      else {
        command = input.substr(1);
        arg = "";
      }
      if (command == "quit") {
        Message quit_msg(TAG_QUIT, "");
        conn.send(quit_msg);
        break;
      }
      else if (command == "leave") {
        Message leave_msg(TAG_LEAVE, "");
        if (!conn.send(leave_msg)) {
          std::cerr << "Failed to send leave message\n";
          continue;
        } if (!conn.receive(response)) {
          std::cerr << "Server did not respond\n";
          continue;
        }
      } else if (command == "/join") {
        Message join_msg(TAG_JOIN, arg);
        if (!conn.send(join_msg)) {
          std::cerr << "Failed to send join message\n";
          continue;
        } if (!conn.receive(response)) {
          std::cerr << "Server did not respond\n";
          continue;
        } 
      } else {
        std::cerr << "Unknown command was entered: " << command << "\n";
      }
    } else {
      Message normal_msg(TAG_SENDALL, input);
      if (!conn.send(normal_msg)) {
        std::cerr << "Failed to send message\n";
        continue;
      } if (!conn.receive(response)) {
        std::cerr << "Server did not respond\n";
        continue;
      }
    }
    if (response.tag == TAG_ERR) {
      std::cerr << response.data << "\n";
    }
  }
  conn.close();
  return 0;
}
