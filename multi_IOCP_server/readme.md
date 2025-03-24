---
languages:
- C++
products:
- ISO C++20
- Windows SDK 10.0
page_type: Personal Project
name: "Chatting Program"
urlFragment: "WinAPI-chat-room-project"
description: ""
---

# Multi-User chatting program

This repository is Multi-User chatting game program.

This repository is a personal work piece to practice how multiple clients connect to the server and expedite tasks when multiple requests are made.  

![채팅결과](./Document/서버_클라이언트_로그뷰어_결과.png)

<br/>

## In this repository

* [Chatting Game Server Project readme file](https://github.com/Mgcllee/ChattingGame/blob/master/ChatServer/readme.md)  
* [Chatting Game Server Project code](https://github.com/Mgcllee/ChattingGame/blob/master/ChatServer)  
* [Chatting Game Client Project readme file](https://github.com/Mgcllee/ChattingGame/blob/master/ChatServer/readme.md)  
* [Chatting Game Client Project code](https://github.com/Mgcllee/ChattingGame/blob/master/ChatClient)  

<br/>

## Branches

* master: using multi-thread, multi-iocp  
* one_IOCP_server: using multi-thread, only one iocp  

<br/>

## The limitations of the current project
1. Use a variety of Windows APIs. Therefore, it only works when the operating system is Windows.
2. I planned to manage the user information I accessed with Redis, but it's not possible because Redis doesn't support Windows

<br/>

## License

This repository is licensed with the MIT license.
