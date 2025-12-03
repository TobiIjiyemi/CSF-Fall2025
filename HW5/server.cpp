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

void communicate_with_receiver(Connection &conn, Server &server, const std::string username) {
  Message join_msg; 
  if (!conn.receive(join_msg)) {
    return;
  } 

  if (join_msg.tag != TAG_JOIN) {
    conn.send(Message(TAG_ERR, "No join message was received\n"));
    return;
  }

  std::string room_to_join = join_msg.data; 
  
  // Find or create that room
  Room *room = server.find_or_create_room(room_to_join);
  // Create a user for this receiver
  User *user = new User(username);
  // Add this user to the room
  room->add_member(user);
  conn.send(Message(TAG_OK, "Receiver joined room successfully\n"));

  // Now loop 
  while (true) {
    //HELP
    Message new_msg; 
    user->mqueue.dequeue();
    
    // Receiver left
    if (!conn.send(new_msg)) { 
      break;
    }
  }
  user->mqueue.~MessageQueue();
  room->remove_member(user);
  delete user; 
} 

void communicate_with_sender(Connection &conn, Server &server, const std::string username) {
  Message new_msg; 
  Room *current_room = nullptr;

  while (conn.receive(new_msg)) {
    if (new_msg.tag == TAG_JOIN) {
      current_room = server.find_or_create_room(new_msg.data);
      conn.send(Message(TAG_OK, "Joined room successfully\n"));
    } else if (new_msg.tag == TAG_SENDALL) {
      if (current_room == nullptr) {
        conn.send(Message(TAG_ERR, "Not currently in a room\n"));
      } else {
        current_room->broadcast_message(username, new_msg.data);
        conn.send(Message(TAG_OK, "Message sent successfully\n"));
      }
    } else if (new_msg.tag == TAG_LEAVE) {
      if (current_room == nullptr) {
        conn.send(Message(TAG_ERR, "Can't leave a room you are not in\n"));
      } else {
        conn.send(Message(TAG_OK, "Room was left\n"));
        current_room = nullptr;
      }
    } else if (new_msg.tag == TAG_QUIT) {
      conn.send(Message(TAG_OK, "Quit\n"));
      break;
    } else {
      conn.send(Message(TAG_ERR, "Invalid command\n"));
    }
  }
}




void *worker(void *arg) {
  pthread_detach(pthread_self());
  //no need to worry about deletion with unique_ptr
  std::unique_ptr<ClientInfo> info(static_cast<ClientInfo *>(arg));
  

  Message login_msg;
  //read login message
  if (!info->conn->receive(login_msg)) {
    //failed to receive login signal so return nullptr
    return nullptr;
  }

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
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // Delete room objects
  for (auto &room : m_rooms) {
    delete room.second;
  }
  m_rooms.clear();
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  std::string port = std::to_string(m_port);
  //return pointer to port string using .c_str() and feed to open_listenfd
  m_ssock = open_listenfd(port.c_str());
  return (m_ssock >= 0);
}

void Server::handle_client_requests() {
  while (true) {
    int clientfd = Accept(m_ssock, NULL, NULL);
    if (clientfd < 0) {
      continue;
    }
    //when new valid connections are made, create a new object for them
    //then create a new thread for each of those connections
    Connection *conn = new Connection(clientfd);
    ClientInfo *info = new ClientInfo(conn, this);

    pthread_t threadid;
    if (pthread_create(&threadid, NULL, worker, info) != 0) {
      std::cerr << "Failed to create thread" << std::endl;
      delete info;
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  Guard g(m_lock);

  //check if room exists, and if so return the room
  auto it = m_rooms.find(room_name);
  if (it != m_rooms.end()) {
    return it->second;
  }

  //otherwise create a new room with that name and add it to the map of rooms
  Room *new_room = new Room(room_name);
  m_rooms[room_name] = new_room;
  return new_room;
}
