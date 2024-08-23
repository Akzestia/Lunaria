# Lunaria

<p align="center">
  <img src="./github-pages/images/tenor-nibutani.gif" alt="Lunaria Logo" width="200" height="200">
</p>

<h3 align="center">Welcome to Lunaria</h3>

[English](README.md) | [日本語](README_ja.md) | [Українська](README_ua.md)

Lunaria is an open-source chat application built around the QUIC protocol, aiming for secure and efficient communication.

## Status

🚧 **Active Development** 🚧

This project has been in active development for about 2 months by a solo developer. Many core features are being implemented, and a pre-alpha release is targeted for the end of August. Your interest and feedback are appreciated as the application progresses.

## About

[FAQ](docs/FAQ.md)
Lunaria is a chat application that prioritizes security and efficiency. It's being developed to leverage the QUIC protocol and provide a communication platform that can potentially be self-hosted and customized.

## Features in Development

- Security enhancements using QUIC protocol
- Self-hostable server
- Peer-to-peer communication within VPN networks
- Client customization using Lua scripts
- CLI/chatbot functionality powered by Lua

## Technologies Being Implemented

This project is actively using or integrating the following technologies:

- **Programming Languages**
  - C++
  - C
  - Shell
  - Lua
- **Libraries/Frameworks**
  - Qt - for GUI development
  - pqxx - PostgreSQL C++ Library
  - cassandra - C++ driver for ScyllaDB
  - Protobuf - for data serialization
  - NetBird - VPN solution
  - MsQuic - Cross-platform QUIC protocol implementation
  - Abseil - C++ Common Library by Google
  - OpenSSL - for cryptographic operations
- **Databases**
  - PostgreSQL
  - ScyllaDB (migration in progress)
- **Tools**
  - g++ - Compiler
  - CMake - Build system
  - Git - Version control

## Target Platforms

Lunaria is being developed to support:
- Linux (x11 | Wayland)
- Windows

## Planned Language Support

Localization is planned for:
- Ukrainian
- Japanese
- English

## Building from Source

As the project is approaching its first pre-alpha release, build instructions are being finalized. This README will be updated with detailed build instructions when the pre-alpha version is released at the end of August.

## GUI Preview

Here are preview images of the current graphical user interface (GUI):

<img src="./GUI/Server.png" alt="Server GUI Preview">

<div align="center">
    <img width=390 src="./GUI/Sign in(1).png" alt="Sign In Preview">
    &nbsp; &nbsp; &nbsp; &nbsp;
    <img width=390 src="./GUI/Sign up(1).png" alt="Sign Up Preview">
</div>

<img src="./GUI/Group 56.png" alt="Chat Interface Preview">

<img src="./GUI/Group 58.png" alt="Settings Preview">

<img src="./GUI/Group 57.png" alt="User Profile Preview">

## Contributing

As a solo project in active development, Lunaria is not currently open for general contributions. However, your interest is greatly appreciated.

### Future Plans for Collaboration

If there's significant interest in contributing to the project, I'm considering a complete rewrite with the following goals:

1. Implementing coding conventions and standardization
2. Establishing a structured development workflow
3. Creating a more collaborative-friendly architecture

This rewrite would aim to make Lunaria more accessible for multiple developers to work on simultaneously.

If you're passionate about the project and would like to discuss potential involvement in its future development, please feel free to reach out directly:

[Email](mailto:akzestia@gmail.com)

To stay updated on the project's progress and any announcements regarding collaboration opportunities, please star and watch this repository.

## Contact

For any questions, feedback, or interest in Lunaria, please feel free to reach out:

[Email](mailto:akzestia@gmail.com)

Thank you for your interest in Lunaria! Your support and feedback are invaluable to this project's development.
