#include "../server/QuicServer.h"
#include <iostream>
#include <csignal> 

volatile sig_atomic_t g_signal_received = 0;

// Signal handler function
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "SIGINT received. Exiting...\n";
        g_signal_received = 1;
    }
}

// Function to check if stdin is connected
bool is_stdin_interactive() {
    return isatty(fileno(stdin)) != 0;
}

int main() {
    // Register signal handler for SIGINT
    signal(SIGINT, signal_handler);

    // Create and start the QuicServer instance
    QuicServer *server =
        new QuicServer("10.10.3.201", 6121, 2, "nexus", "../../certs/server.cert",
                       "../../certs/server.key");

    server->Start();

    // Check if stdin is interactive (only wait for getchar if interactive)
    if (is_stdin_interactive()) {
        std::cout << "Press 'q' to exit\n";
        while (!g_signal_received) {
            // Use getchar to wait for 'q' input
            char exit_char = getchar();
            if (exit_char == 'q' || exit_char == 'Q') {
                std::cout << "Received 'q'. Exiting...\n";
                break;
            }
        }
    } else {
        std::cout << "Running in non-interactive mode. Use Ctrl+C to exit.\n";
        while (!g_signal_received) {
            // Do other periodic tasks if needed
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    // Close and delete the server instance
    server->Close();
    delete server;

    return 0;
}