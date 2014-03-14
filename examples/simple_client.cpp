#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://localhost:4242";

  // initialize the 0MQ context
  zmqpp::context context;

  // generate a push socket
  zmqpp::socket_type type = zmqpp::socket_type::push;
  zmqpp::socket socket (context, type);

  // open the connection
  cout << "Opening connection to " << endpoint << "..." << endl;
  socket.connect(endpoint);

  // send a message
  cout << "Sending text and a number..." << endl;
  zmqpp::message message;
  // compose a message from a string and a number
  message << "Hello World!" << 42;
  socket.send(message);
  
  cout << "Sent message." << endl;
  cout << "Finished." << endl;
}
