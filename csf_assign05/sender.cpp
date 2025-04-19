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

    std::string server_hostname = argv[1];
    std::string server_port = argv[2];
    std::string username = argv[3];

    // Connect to server
    Connection conn;
    conn.connect(server_hostname, std::stoi(server_port));
    if (!conn.is_open()) {
        std::cerr << "Error: Unable to connect to server " 
                 << server_hostname << " on port " << server_port << "\n";
        return 1;
    }

    // Send slogin message
    Message slogin_msg(TAG_SLOGIN, username);
    if (!conn.send(slogin_msg)) {
        std::cerr << "Error: Failed to send login message\n";
        return 1;
    }

    // Receive login response
    Message login_resp;
    if (!conn.receive(login_resp)) {
        std::cerr << "Error: Failed to receive login response\n";
        return 1;
    }

    if (login_resp.tag == TAG_ERR) {
        std::cerr << login_resp.data << "\n";
        return 1;
    }

    // Loop that goes until the person quits and sever allows to quit
    std::string input;
    while (std::getline(std::cin, input)) {
        input = trim(input);
        if (input.empty()) continue;

        // Handle commands
        if (input[0] == '/') {
            std::size_t space_pos = input.find(' ');
            //take the rest of the command after seeing the "/" command
            std::string cmd = input.substr(1, space_pos - 1);
            std::string arg;
            if (space_pos != std::string::npos) {
                arg = input.substr(space_pos + 1);
            } else {
                arg = "";
            }


            
            Message msg;
            if (cmd == "join") {
                msg = Message(TAG_JOIN, arg);
            } else if (cmd == "leave") {
                msg = Message(TAG_LEAVE, "Leaving the room");
            } else if (cmd == "quit") {
                msg = Message(TAG_QUIT, "Quitting");
                if (!conn.send(msg)) {
                    std::cerr << "Error: Failed to send quit message\n";
                    return 1;
                }
                
                // Wait for server response before exiting
                Message quit_resp;
                if (!conn.receive(quit_resp)) {
                    std::cerr << "Error: Failed to receive quit response\n";
                    return 1;
                }
                
                if (quit_resp.tag == TAG_ERR) {
                    std::cerr << quit_resp.data << "\n";
                }
                return 0;
            } else {
                std::cerr << "Error: Invalid command\n";
                continue;
            }

            if (!conn.send(msg)) {
                std::cerr << "Error: Failed to send message\n";
                continue;
            }

            // Get server response
            Message resp;
            if (!conn.receive(resp)) {
                std::cerr << "Error: Failed to receive response\n";
                continue;
            }

            if (resp.tag == TAG_ERR) {
                std::cerr << resp.data << "\n";
            }
        } else {
            // Regular message to send to room
            Message msg(TAG_SENDALL, input);
            if (!conn.send(msg)) {
                std::cerr << "Error: Failed to send message\n";
                continue;
            }

            // Get server response
            Message resp;
            if (!conn.receive(resp)) {
                std::cerr << "Error: Failed to receive response\n";
                continue;
            }

            if (resp.tag == TAG_ERR) {
                std::cerr << resp.data << "\n";
            }
        }
    }

    return 0;
}
