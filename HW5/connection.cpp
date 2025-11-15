#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include <string>

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  // TODO: call rio_readinitb to initialize the rio_t object
  std::string port_str = std::to_string(port);

  m_fd = open_clientfd(const_cast<char*>(hostname.c_str()),
                      const_cast<char*>(port_str.c_str()));

  // Error
  if (m_fd < 0) {
    m_last_result = EOF_OR_ERROR;
    return;
  }

  rio_readinitb(&m_fdbuf, m_fd);
  m_last_result = SUCCESS;
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  if (m_fd >= 0) {
    Close(m_fd);
  }
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return m_fd >= 0;
}

void Connection::close() {
  // TODO: close the connection if it is open
  if (m_fd >= 0) {
    Close(m_fd);
    m_fd = -1;
  }
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  
  if (!is_open()) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  std::string message = msg.tag + ":" + msg.data + "\n";

  ssize_t wrote = rio_writen(m_fd, message.c_str(), message.size());
  if (wrote != (ssize_t)(message.size())) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }
  
  m_last_result = SUCCESS;
  return true; 
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
}
