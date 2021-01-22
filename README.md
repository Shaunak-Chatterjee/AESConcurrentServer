# AESConcurrentServer

## Pre-requisites
Openssl API - sudo apt-get install libssl-dev

To run the 
* Server - gcc server.c -o server -pthread -lcrypto
* Client - gcc client.c -o client -lcrypto

## Project Description:
This project will have three parts:

Part 1: A tie up will be made between a single server and multiple clients at the same time. The simplest way to write a concurrent server under UNIX is to fork a child process to handle each client.

Part 2: The 2nd part of the project is to encrypt data between server and clients. In our project we will use AES Symmetric Key encryption method to encrypt data from client to server and vice versa. The AES has a symmetric key which is shared between server and all clients previously. The same symmetric key will be used to encrypt and decrypt data at server and clients side.

Part 3: The 3rd part of the project preserves integrity of data between server and clients. In this part each client computes a hash of data using a popular hashing method MD5. All the data from server to clients and clients to server will be hashed. Server and clients both will check sent hash value before accepting the data.

![Alt text](/AESConcurrentServer/blob/main/SS.jpg?raw=true "Screenshot")
