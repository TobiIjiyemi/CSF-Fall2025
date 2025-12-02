#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"
#include "csapp.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

//data strcuture for aggregating conn and server into one data structure
//to be passed into pthread_create, also to allow for easy deletion and through
//closure of file descripter and freed connection memory.

struct ClientInfo {
  Connection *conn;
  Server *server;

  ClientInfo(Connection *c, Server *s) : conn(c), server(s) {}
  ~ClientInfo() {
    if (conn) {
      conn->close();
      delete conn;
    }
  }
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  //no need to worry about deletion with unique_ptr
  std::unique_ptr<ClientInfo> info(static_cast<ClientInfo *>(arg));
  

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  Message login_msg;
  //read login message
  if (!info->conn->receive(login_msg)) {
    //failed to receive login signal so return nullptr
    return nullptr;
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  //if logged in as sender
  if (login_msg.tag == TAG_SLOGIN) {
    info->conn->send(Message(TAG_OK, "Logged in as sender"));
    //implement communication with sender function
  } else if (login_msg.tag == TAG_RLOGIN) {
    info->conn->send(Message(TAG_OK, "Logged in as receiver"));
    //implementi communication wtih receiver function
  } else {
    info->conn->send(Message(TAG_ERR, "Invalid login"));
  }
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  std::string port = std::to_string(m_port);
  //return pointer to port string using .c_str() and feed to open_listenfd
  m_ssock = open_listenfd(port.c_str());
  return (m_ssock >= 0);
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
}
