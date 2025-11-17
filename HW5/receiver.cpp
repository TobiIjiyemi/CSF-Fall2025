#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // TODO: connect to server
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    std::cerr << "Couldn't connect at server at " << server_hostname << ":" << server_port << "\n";
    return 1;
  }

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  Message rlogin_msg(TAG_RLOGIN, username);
  if (!conn.send(rlogin_msg)) {
    std::cerr << "Failed to send rlogin message\n";
    conn.close();
    return 1;
  }

  Message rlogin_resp;
  if (!conn.receive(rlogin_resp)) {
    std::cerr << "Failed to receive rlogin response\n";
    conn.close();
    return 1;
  }

  Message join_msg(TAG_JOIN, room_name);

  if (!conn.send(join_msg)) {
    std::cerr << "Failed to send join message\n";
    conn.close();
    return 1;
  }

  Message join_resp;
  if (!conn.receive(join_resp)) {
    std::cerr << "Failed to receive join response\n";
    conn.close();
    return 1;
  }

  if (join_resp.tag == TAG_ERR) {
    std::cerr << join_resp.data << "\n";
    conn.close();
    return 1;
  }
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  
  Message server_msg;
  while (conn.receive(server_msg)) {
    if (server_msg.tag == TAG_DELIVERY) {
      std::string data = server_msg.data;
      size_t seperator = data.find(':');
      if (seperator == std::string::npos) {
        std::cerr << "Missign room from message\n";
        continue;
      }
    }
  }
  return 0;
}
