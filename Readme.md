# Irc
## Overview

Irc is a project that implements an IRC (Internet Relay Chat) server. This project focuses on understanding network programming and the IRC protocol to create a functional chat server.
## Features

-   Implements the core functionalities of an IRC server.
-   Supports multiple clients and channels.
-   Handles IRC commands and messages.
-   Provides user authentication and nickname management.

## Prerequisites

-   A Unix-like operating system.
-   GCC (GNU Compiler Collection) installed.
-   Basic knowledge of C++ programming and network protocols.

## Installation
```
git clone https://github.com/jbettini/Irc.git;
cd Irc;
make
```
## Usage

Run the compiled IRC server executable:
```
./ircserver <port> <password>
```
    <port>: Port number the server will listen on.
    <password>: Password for client connections.

## Example:
```
./ircserver 6667 mypassword
```
## Project Structure

-   src/: Source files.
-   include/: Header files.
-   Makefile: Build configuration.