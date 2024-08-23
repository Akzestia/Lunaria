## General Questions

### What is Lunaria?
Lunaria is an open-source chat application currently in development. It aims to be a secure, high-performance chat platform with cross-platform support.

### What are the key features of Lunaria?
Some of the planned or in-progress features include:
- End-to-end encryption for secure messaging
- Cross-platform support (Windows, Linux)
- Wayland support for Linux
- Real-time message delivery using the QUIC protocol
- Efficient data storage and retrieval with PostgreSQL/ScyllaDb
- Customizable themes and UI elements
- VPN support

### What technologies does Lunaria use?
Lunaria currently utilizes a range of technologies, including:
- C/C++
- Qt
- Lua
- Protobuf
- PostgreSQL
- ScyllaDb (Migrating to Scylla)
- MsQuic (Microsoft's implementation of the QUIC protocol)

> Note: The technology stack may change in future versions of the project. See the "Future Plans" section for more details.

## Development Questions

### Why is there only one branch in the repository?
As a solo developer, I've currently opted for a single branch workflow to simplify the development process. While this approach works for the current stage of the project, I acknowledge that it's not ideal for larger collaborative efforts. In the future, if there's significant interest in the project, I plan to transition to a more structured branching strategy, including separate development and main branches.

### Why can't I build the project from source?
The project is currently under active development and doesn't fully support building from source. Here's the current status:
- Server: Can be built using Docker. Please refer to [BUILD.md](../BUILD.md) for detailed instructions. Note that you'll need to set up a ScyllaDb cluster.
- Client: Not yet ready for building from source, as it's currently configured for my local environment.

I'm working on making the build process more accessible in future releases.

### When will the project have any releases?
I'm aiming for an Alpha release around late August or early September. This version is expected to include:
- Basic messaging functionality
- File sharing
- Server creation
- Possibly voice communication

Please note that the Alpha release may not include Windows support or screen streaming/sharing features.

### What are the future plans if there's significant interest in the project?
If Lunaria gains significant traction, some potential plans include:
- A complete rewrite of the project from scratch
- Transitioning from C++ to Rust, potentially using the GPUI framework instead of Qt
- Full transition to ScyllaDb, removing support for PostgreSQL
- Implementing a more robust branching strategy
- Expanding the feature set based on user feedback
- Improving cross-platform support

These changes aim to optimize performance, improve scalability, and leverage modern development tools and frameworks. I'm open to community input and will adjust my plans based on user needs and project growth.

### How can I contribute to the project?
As a solo developer, I greatly appreciate interest from the community and am open to contributions. While the project is still in its early stages, here are ways you can get involved:

1. **Initial Contact**: If you're interested in contributing, please reach out to me via 📧 [email](mailto:akzestia@gmail.com). Briefly describe your background and the areas you're interested in contributing to.

2. **Further Collaboration**: After initial contact, I prefer to collaborate through chat messengers for more detailed discussions and coordination.

3. **Code Contributions**: If you're interested in contributing code, we can discuss how your contribution could fit into the project's current state and future plans.

4. **Bug Reports and Feature Requests**: Use the GitHub Issues tab to report bugs or suggest new features. Detailed reports are incredibly helpful for improving Lunaria.

5. **Documentation**: As the project grows, clear documentation will be crucial. If you're interested in helping with documentation, please let me know via email.

6. **Testing**: Once we have alpha or beta releases, thorough testing across different environments will be valuable. Your feedback on usability and performance will be much appreciated.

7. **Spread the Word**: If you find Lunaria interesting, star the repository and share it with others who might be interested.

While the project is still evolving and the codebase is subject to significant changes, I'm excited about the possibility of collaborating with others. I'll be working on setting up contribution guidelines and improving the project structure to facilitate easier contributions in the future.

Your interest and support are greatly appreciated!
## Support and Contact

### Where can I report issues or suggest features?
 Please use the GitHub Issues tab in this repository to report any bugs or suggest new features. I appreciate your feedback and will review all submissions.

### How can I stay updated on Lunaria's development?
The best ways to stay informed are:
1. Watch this GitHub repository
2. Check the releases page periodically
3. Follow project updates in the README

Thank you for your interest in Lunaria! I'm excited to develop this project and look forward to sharing my progress with the community.
