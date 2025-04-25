#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include <unistd.h>
#include <sys/socket.h>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

struct ClientData {
  Server *server;
  Connection *conn;
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void chat_with_sender(Server *server, Connection *conn, const std::string &username) {
  Room *room = nullptr;
    
    while (true) {
        Message msg;
        if (!conn->receive(msg)) {
            conn->send(Message(TAG_ERR, "Invalid message"));
            break;
        }

        if (msg.tag == TAG_JOIN) {
            if (room) {
                conn->send(Message(TAG_ERR, "Already in a room"));
                continue;
            }
            room = server->find_or_create_room(msg.data);
            conn->send(Message(TAG_OK, "Joined room: " + msg.data));
        }
    else if (msg.tag == TAG_LEAVE) {
      if (!room) {
        conn->send(Message(TAG_ERR, "Not in a room"));
        continue;
      }
      room = nullptr;
      conn->send(Message(TAG_OK, "Left room"));
    } 
    else if (msg.tag == TAG_SENDALL) {
      if (!room) {
        conn->send(Message(TAG_ERR, "Not in a room"));
        continue;
      }
      room->broadcast_message(username, msg.data);
      conn->send(Message(TAG_OK, "Message sent"));
    } 
    else if (msg.tag == TAG_QUIT) {
      conn->send(Message(TAG_OK, "Bye"));
      break;
    } 
    else {
      conn->send(Message(TAG_ERR, "Invalid command for sender"));
    }
  }
}

void chat_with_receiver(Server *server, Connection *conn, const std::string &username) {
  Message join_msg;
  if (!conn->receive(join_msg) || join_msg.tag != TAG_JOIN) {
      conn->send(Message(TAG_ERR, "Expected join message"));
      return;
  }

  Room *room = server->find_or_create_room(join_msg.data);
  User *user = new User(username);
  room->add_member(user);
  
  conn->send(Message(TAG_OK, "Joined room: " + join_msg.data));

  while (true) {
      Message *msg = user->mqueue.dequeue();
      if (!msg) {
          continue;
      }

      if (!conn->send(*msg)) {
          delete msg;
          break;
      }
      delete msg;
  }

  room->remove_member(user);
  delete user;
}

void *worker(void *arg) {
  pthread_detach(pthread_self());
  ClientData *data = static_cast<ClientData*>(arg);
  Server *server = data->server;
  std::unique_ptr<Connection> conn(data->conn);
  std::unique_ptr<ClientData> client_data(data);

  Message login_msg;
  if (!conn->receive(login_msg)) {
    conn->send(Message(TAG_ERR, "Invalid login"));
    return nullptr;
  }

  if (login_msg.tag == TAG_SLOGIN) {
    conn->send(Message(TAG_OK, "Logged in as sender"));
    chat_with_sender(server, conn.get(), login_msg.data);
  } 
  else if (login_msg.tag == TAG_RLOGIN) {
    conn->send(Message(TAG_OK, "Logged in as receiver"));
    chat_with_receiver(server, conn.get(), login_msg.data);
  } 
  else {
    conn->send(Message(TAG_ERR, "Invalid login tag"));
  }

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port), m_ssock(-1) {
  pthread_mutex_init(&m_lock, nullptr);
}

Server::~Server() {
  pthread_mutex_destroy(&m_lock);
  for (auto &room_pair : m_rooms) {
    delete room_pair.second;
  }
}

bool Server::listen() {
  std::string port_str = std::to_string(m_port);
  m_ssock = open_listenfd(port_str.c_str());
  return m_ssock >= 0;
}

void Server::handle_client_requests() {
  while (true) {
    int client_fd = Accept(m_ssock, nullptr, nullptr);
    if (client_fd < 0) {
      continue;
    }

    ClientData *data = new ClientData();
    data->server = this;
    data->conn = new Connection(client_fd);

    pthread_t thr;
    if (pthread_create(&thr, nullptr, worker, data) != 0) {
      delete data->conn;
      delete data;
      close(client_fd);
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  Guard guard(m_lock);
  auto it = m_rooms.find(room_name);
  if (it != m_rooms.end()) {
    return it->second;
  }
  
  Room *new_room = new Room(room_name);
  m_rooms[room_name] = new_room;
  return new_room;
}