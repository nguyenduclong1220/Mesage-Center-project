Welcome to our Project! Our team includes 3 members: Karina(Leader), Thomas, Mark. 

Title: MESSAGE CENTER PROJECT.

**SOFTWARE DEVELOPMENT LIFE CYCLE DOCUMENTATION**
> https://torpid-sorrel-047.notion.site/Message-Center-Project-4f3e279e026d4084811a3f2352d5b2ff

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

> Architectural Pattern: Client-Server model using Socket in connection mode (TCP) and multithreading for handling multiple clients. 
>
> Language: C

> Run the program:
> 
-  $ make
-  $ ./server (IP) (port)
-  $ ./client (port)
  
> Reference:
> 
  https://github.com/nikhilroxtomar/Chatroom-in-C
