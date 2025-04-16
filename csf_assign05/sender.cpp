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
  //server_port = std::stoi(argv[2]);
  username = argv[3];
  server_port = argv[2];

  // TODO: connect to server
  int clientfd = open_clientfd((char*)server_hostname.c_str(), (char*)server_port.c_str());
  if (clientfd < 0) {
    std::cerr << "Error: Unable to connect to server " 
              << server_hostname << " on port " << server_port << "\n";
    return 1;
  }

  //Initialize I/O for the connection
  rio_t rio;
  rio_readinitb(&rio, clientfd);

  // TODO: send slogin message
  Message slogin_msg(TAG_SLOGIN, username);
  std::string slogin_encoded = slogin_msg.encode();

  //decode tester, delete eventually
  //Message decodedmsg = Message::decode(slogin_encoded);
  //printf("%s\n", decodedmsg.encode().c_str());
  
  rio_writen(clientfd, slogin_encoded.c_str(), slogin_encoded.size());

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  return 0;
}
