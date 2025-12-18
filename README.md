<p align="center">
  <img src="https://raw.githubusercontent.com/ayogun/42-project-badges/main/badges/ft_irce.png" alt="ft_irc badge" width="150">
</p>

<h1 align="center">ft_irc</h1>

<p align="center">
  <img src="https://img.shields.io/badge/42-Lyon-blue?style=for-the-badge&logo=42&logoColor=white" alt="42 Lyon">
  <img src="https://img.shields.io/badge/Score-125%2F100-brightgreen?style=for-the-badge" alt="Score">
  <img src="https://img.shields.io/badge/Language-C++98-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++">
</p>

<p align="center">
  An IRC server implementation in C++98, compliant with RFC 1459 standards.
</p>

---

## Description

**ft_irc** is a fully functional Internet Relay Chat (IRC) server built from scratch in C++98. This project implements the core functionalities of an IRC server, allowing multiple clients to connect, communicate in channels, and exchange private messages using standard IRC clients like irssi, WeeChat, or HexChat.

The server handles multiple simultaneous connections using `poll()` for non-blocking I/O operations, ensuring efficient resource management and responsive client handling.

## Features

- **Multi-client support** - Handle multiple simultaneous connections
- **Channel management** - Create, join, and manage IRC channels
- **User authentication** - Password-protected server access
- **Operator privileges** - Channel operator commands and permissions
- **Private messaging** - Direct user-to-user communication
- **Channel modes** - Support for various channel configuration modes

## Implemented Commands

### User Commands

| Command | Description |
|---------|-------------|
| `PASS` | Authenticate with server password |
| `NICK` | Set or change nickname |
| `USER` | Set username and realname |
| `QUIT` | Disconnect from server |
| `PRIVMSG` | Send private messages to users or channels |

### Channel Commands

| Command | Description |
|---------|-------------|
| `JOIN` | Join a channel |
| `TOPIC` | View or set channel topic |
| `INVITE` | Invite a user to a channel |
| `KICK` | Remove a user from a channel |
| `MODE` | Set channel or user modes |

### Supported Modes

| Mode | Description |
|------|-------------|
| `i` | Invite-only channel |
| `t` | Topic restricted to operators |
| `k` | Channel key (password) |
| `o` | Channel operator privilege |
| `l` | User limit for channel |

## Project Structure

```
ft_irc/
├── code/
│   ├── server/
│   │   ├── commands/      # IRC command implementations
│   │   ├── header/        # Header files
│   │   ├── parsing/       # Message parsing
│   │   └── replies/       # IRC numeric replies
│   └── main.cpp
├── Makefile
└── motd.txt               # Message of the day
```

## Compilation

```bash
# Compile the server
make

# Clean object files
make clean

# Full clean (including binary)
make fclean

# Recompile
make re
```

## Usage

```bash
# Start the server
./ircserv <port> <password>

# Example
./ircserv 6667 mypassword
```

### Connecting with an IRC Client

```bash
# Using irssi
irssi -c localhost -p 6667 -w mypassword

# Using netcat (for testing)
nc localhost 6667
```

### Basic IRC Commands Example

```
PASS mypassword
NICK mynick
USER myuser 0 * :My Real Name
JOIN #channel
PRIVMSG #channel :Hello everyone!
PRIVMSG othernick :Private message here
```

## Authors

| Author | GitHub |
|--------|--------|
| **vzuccare** | [@FumiVZ](https://github.com/FumiVZ) |
| **machrist** | [@Denos-soneD](https://github.com/Denos-soneD) |
| **mwojtasi** | [@SUPE4COOKIE](https://github.com/SUPE4COOKIE) |

---

<p align="center">
  Made with dedication at <strong>42 Lyon</strong>
</p>
