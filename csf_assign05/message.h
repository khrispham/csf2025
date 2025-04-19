#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

struct Message {
  // An encoded message may have at most this many characters,
  // including the trailing newline ('\n'). Note that this does
  // *not* include a NUL terminator (if one is needed to
  // temporarily store the encoded message.)
  static const unsigned MAX_LEN = 255;

  std::string tag;
  std::string data;

  Message() { }

  Message(const std::string &tag, const std::string &data)
    : tag(tag), data(data) { }

  // TODO: you could add helper functions
  //Return a string of the format "[tag]:[data]\n"
  std::string encode() {
    std::string encoded = tag + ":" + data + "\n";
    if (encoded.size() > Message::MAX_LEN) {
      fprintf(stderr, "Encoded message exceeds maximum allowed length.\n");
    }
    return encoded;
  }

  //Update tag and data of this Message struct from string param "[tag]:[data]"
  static Message decode(const std::string &msgstring) {
    std::string delimiter = ":";
    std::size_t pos = msgstring.find(delimiter);
    if (pos == std::string::npos) {
      fprintf(stderr, "Invalid message format: no delimiter found.\n");
    }
    std::string tag_part = msgstring.substr(0, pos);
    std::string data_part = msgstring.substr(pos + 1, msgstring.size() - pos - 1);
    if (!data_part.empty() && data_part.back() == '\n') {
      data_part.pop_back();
    }
    return Message(tag_part, data_part);
  }
};

// standard message tags (note that you don't need to worry about
// "senduser" or "empty" messages)
#define TAG_ERR       "err"       // protocol error
#define TAG_OK        "ok"        // success response
#define TAG_SLOGIN    "slogin"    // register as specific user for sending
#define TAG_RLOGIN    "rlogin"    // register as specific user for receiving
#define TAG_JOIN      "join"      // join a chat room
#define TAG_LEAVE     "leave"     // leave a chat room
#define TAG_SENDALL   "sendall"   // send message to all users in chat room
#define TAG_SENDUSER  "senduser"  // send message to specific user in chat room
#define TAG_QUIT      "quit"      // quit
#define TAG_DELIVERY  "delivery"  // message delivered by server to receiving client
#define TAG_EMPTY     "empty"     // sent by server to receiving client to indicate no msgs available

#endif // MESSAGE_H
