#include <iostream>
#include <thread>
#include <chrono>
#include <olc.h>

enum class CustomMsgTypes : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

class CustomClient : public client_interface<CustomMsgTypes> {
public:
    void PingServer() {
        std::cout << "PingServer called\n"; // Debug message
        message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerPing;
        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
        msg << timeNow;
        Send(msg);
        std::cout << "Ping message sent to server.\n"; // Debug message
    }
    void MessageAll() {
        std::cout << "MEssage All Called\n";
        message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::MessageAll;
        Send(msg);
        std::cout << "Message sent to server\n";
    }
};

void InputHandler(bool& bQuit, CustomClient& c) {
    while (!bQuit) {
        std::string inputLine;
        std::getline(std::cin, inputLine);
        for (char ch : inputLine) {
            if (ch == '1') {
                c.PingServer();
            }
            else if (ch == '2') {
                c.MessageAll();
            }
            else if (ch == '3') {
                bQuit = true;
                break;
            }
        }
    }
}

int main() {
    CustomClient c;
    if (c.Connect("127.0.0.1", 60000)) {
        std::cout << "Connected to server.\n";
    }
    else {
        std::cout << "Failed to connect to server.\n";
        return 1;
    }

    bool bQuit = false;
    std::thread inputThread(InputHandler, std::ref(bQuit), std::ref(c));

    while (!bQuit) {
        if (c.IsConnected()) {
            if (!c.Incoming().empty()) {
                std::cout << "Message received in client.\n"; // Debug message
                auto msg = c.Incoming().pop_front().msg;
                switch (msg.header.id) {
                case CustomMsgTypes::ServerPing:
                {
                    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                    std::chrono::system_clock::time_point timeThen;
                    msg >> timeThen;
                    std::cout << "RTT: " << std::chrono::duration<double>(timeNow - timeThen).count() << " seconds\n";
                }
                break;
                case CustomMsgTypes::ServerAccept: {
                    std::cout << "Server has accepted the id\n";
                }
                break;
                case CustomMsgTypes::ServerMessage: {
                    uint32_t clientId;
                    msg >> clientId;
                    std::cout << "Hello From [" << clientId << "]\n";
                }
                break;
                default:
                    std::cout << "Received unknown message ID: " << static_cast<uint32_t>(msg.header.id) << "\n";
                    break;
                }
            }
            else {
                // Optional sleep to prevent tight looping
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        else {
            std::cout << "Server Down\n";
            bQuit = true;
        }
    }

    inputThread.join();
    return 0;
}
