# ft_irc

ft_irc is a simple IRC (Internet Relay Chat) server implementation written in C. This project is designed to be compliant with the RFC 2812 standard, allowing multiple users to connect, chat, and create channels.

## Features

- Support for multiple users
- Channel creation and management
- Private messages
- Basic IRC commands: `JOIN`, `PART`, `PRIVMSG`, `NICK`, `QUIT`, and more
- Complies with the RFC 2812 standard

## Getting Started

### Prerequisites

Ensure you have the following installed:

- **Make**: Used to build the project.
- **GCC**: The GNU Compiler Collection for compiling C programs.
- **irssi**: A popular terminal-based IRC client for testing your IRC server.

#### Installing Dependencies

- **On Debian/Ubuntu**:
```bash
  sudo apt-get update
  sudo apt-get install make gcc irssi
```
- **On macOS (using homebrew)**:
```bash
brew install make gcc irssi
```
#### Cloning the Repository
To get a local copy up and running, clone the repository using the following command:
```bash
git clone https://github.com/yourusername/ft_irc.git
cd ft_irc
```
#### Building the Project
Once inside the project directory, build the project using `make`:
```bash
make
```
This will compile all the necessary source files and generate the ft_irc executable.

#### Running ft_irc
After building the project, you can start the IRC server with:
```bash
./ft_irc <port> <password>
```
- `<port>`: The port number on which the server will listen.
- `<password>`: The password required for clients to connect to the server.
# Example
```bash
./ft_irc 6667 pass
```
Connecting to the Server
You can connect to your IRC server using irssi or any other IRC client.

To connect using irssi, In another terminal run the following command:
```bash
irssi
```
Then, within irssi, connect to your server:

```bash
/connect localhost <port>
```
You will be prompted to enter a password, You have to enter the password you used when running the server:
```bash
/quote pass <password>
```
then enter your nickname
```bash
/quote nick <nickname>
```
then enter you will be prompted to enter a user name:
```bash
/quote user <username> 0 * :<real_name>
```
Once connected, you can create channels, send private messages to other clients, and more
