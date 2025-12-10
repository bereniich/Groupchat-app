# GroupChat App

GroupChat is a C++ client–server chat application enabling real-time messaging between multiple clients.  
The server manages continuous incoming connections and broadcasts received messages to all active clients.  
Both the client and server compress outgoing messages using a Huffman coding algorithm and decompress incoming ones.

---

## Features

- Real-time communication over TCP
- Support for multiple connected clients through multithreading
- Centralized broadcasting of messages
- Huffman compression and decompression implemented on both sides
- Custom implementation of binary tree encoding structure
- Clean server shutdown and safe thread termination

---

## Application Operation

### Server:
1. Creates a listening TCP socket
2. Accepts client connections
3. Launches a separate thread for each client
4. Receives compressed messages
5. Broadcasts compressed messages to all connected clients

### Client:
1. Connects to the server
2. Accepts user input
3. Compresses outgoing messages
4. Sends compressed data to the server
5. Receives compressed messages from the server
6. Performs decompression and prints the message

## Building and Running

### Compile the server
g++ server.cpp huffman.cpp -o server -pthread

### Compile the client
g++ client.cpp huffman.cpp -o client -pthread

### Run the server
./server
### Run multiple clients
./client <IP> <PORT> <USERNAME>

## Future Work and Improvements

* Optional encryption
* GUI interface for the client application
* Message history persistence
* File transfer support
* Extended error handling

## Author
Ivan Berenić
Faculty of Technical Sciences – Novi Sad
Department of Computer and Control Engineering

Ako želiš, mogu ti dopuniti README instrukcijama o formatu poruke, listom poznatih problema, ili dijagramom komunikacije.
```
