#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object

  //set up rio buffer
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  // Convert port number to string
  std::string port_str = std::to_string(port);
  
  // Open client connection
  m_fd = open_clientfd(const_cast<char*>(hostname.c_str()), 
                     const_cast<char*>(port_str.c_str()));
  

  //handle if we cannot connect to the server
  if (m_fd < 0) {
    m_last_result = EOF_OR_ERROR;
    return;
  }
  
  // Initialize Rio buffer
  rio_readinitb(&m_fdbuf, m_fd);
  m_last_result = SUCCESS;

}

Connection::~Connection() {
  // TODO: close the socket if it is open
  if (is_open()) {
    Close(m_fd);
    m_fd = -1;
  }
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return m_fd >= 0;

}

void Connection::close() {
  // TODO: close the connection if it is open

  //Comment for Khris: Is this the same as the destructor for connection? I'm not sure lmfao 
  //I feel like I'm prolly missing something for the destructor then
  if (is_open()) {
    Close(m_fd);
    m_fd = -1;
  }
}

bool Connection::send(const Message &msg) {
  //if there is no connection, return false
  // and set m_last_result to INVALID_MSG
  if (!is_open()) {
    m_last_result = INVALID_MSG;
    return false;
  }


  // TODO: send a message


  // Format the message correctly"
  std::string formatted_msg = msg.tag + ":" + msg.data + "\n";
  ssize_t bytes_written = rio_writen(m_fd, formatted_msg.c_str(), formatted_msg.length());
  // return true if successful, false if not
  // make sure that m_last_result is error if issues occur

    if (bytes_written != (ssize_t)formatted_msg.length()) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  if (!is_open()) {
    m_last_result = INVALID_MSG;
    return false;
  }

  char buf[Message::MAX_LEN + 1];
  //using rio read the line. Note: Rio does not strip the newline characters.
  ssize_t bytes_read = rio_readlineb(&m_fdbuf, buf, Message::MAX_LEN);
  

  //if we haven't read anything, return false
  if (bytes_read <= 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  // Remove trailing newline characters until there are no more newlines
  std::string line(buf);
  while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
    line.pop_back();
  }

  // Parse tag and data
  std::size_t colon_pos = line.find(':');
  if (colon_pos == std::string::npos) {
    m_last_result = INVALID_MSG;
    return false;
  }
  //user
  msg.tag = line.substr(0, colon_pos);
  //what it says
  msg.data = line.substr(colon_pos + 1);

  //set the last result to a successful receive
  m_last_result = SUCCESS;
  return true;
}
