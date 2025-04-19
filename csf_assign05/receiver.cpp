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
    std::cerr << "Could not connect to server at " << server_hostname << ":" << server_port << "\n";
    return 1;
  }



  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  // Send rlogin message
  Message login_msg(TAG_RLOGIN, username);
  if (!conn.send(login_msg)) {
    std::cerr << "Failed to send login message\n";
    return 1;
  }

  // Receive response to rlogin
  Message login_resp;
  if (!conn.receive(login_resp)) {
    std::cerr << "Failed to receive login response\n";
    return 1;
  }

    if (login_resp.tag == TAG_ERR) {
    std::cerr << login_resp.data << "\n";
    return 1;
  }

  // Send join message
    Message join_msg(TAG_JOIN, room_name);
  if (!conn.send(join_msg)) {
    std::cerr << "Failed to send join message\n";
    return 1;
  }
  // Receive response to join
  Message join_resp;
  if (!conn.receive(join_resp)) {
    std::cerr << "Failed to receive join response\n";
    return 1;
  }
  //handle issues
    if (join_resp.tag == TAG_ERR) {
    std::cerr << join_resp.data << "\n";
    return 1;
  }

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)


  // Main message loop
  while (true) {
    Message msg;
    if (!conn.receive(msg)) {
      std::cerr << "Unable to receive message\n";
      return 1;
    }

    if (msg.tag == TAG_DELIVERY) {
      //Find the colons other than the first colon and keep them as they are legal
      std::size_t first_colon = msg.data.find(':');
      std::size_t second_colon = msg.data.find(':', first_colon + 1);
      
      if (first_colon != std::string::npos && second_colon != std::string::npos) {
        std::string sender = msg.data.substr(first_colon + 1, second_colon - first_colon - 1);
        std::string message_text = msg.data.substr(second_colon + 1);
        std::cout << sender << ": " << message_text << "\n";
      }
    } else if (msg.tag == TAG_ERR) {
      std::cerr << msg.data << "\n";
      return 1;
    }
  }


  return 0;
}

