Welcome to our Project! Our team includes 3 members: Karina(Leader), Thomas, Mark. 

Title: MESSAGE CENTER PROJECT.

The Message Center Project will allow members who connect to the same Local Area Network to join and use (send/ receive message) according to their roles

> Specification:
> 
- **Message Center** is a background service that: Receive messages from one or more applications that acts as **Talkers**
- Send the received messages to one or more applications that act as **Listeners**
- An application can register to Message Center to be a either Talker or Listener, or both of them
- Work in a Local Network

> Requirements:
> 
- No Login, No Database, No History
- Select a role when app starts
- Apps do not poll for incoming data
- No limited number of App connected to Message Center

Architectural Pattern: Client-Server model using Socket in connection mode (TCP) and multithreading for handling multiple clients. 
Languahe: C

> Run the program:
> 
 $ make
 $ ./server <IP> <port>
 $ ./client <port>
  
> Reference:
> 
  https://github.com/nikhilroxtomar/Chatroom-in-C
