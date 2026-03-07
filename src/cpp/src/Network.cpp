#include "../include/canalize/network/Packet.h"
#include "../include/NativeLog.h"
#include <iostream>

namespace Canalize {
    namespace Network {

        NetworkManager& NetworkManager::getInstance() {
            static NetworkManager instance;
            return instance;
        }

        void NetworkManager::sendToServer(const Packet& packet) {
            // Serialize packet
            std::vector<uint8_t> buffer;
            packet.encode(buffer);
            
            // JNI call to send packet to server...
            NativeLog::info("[Network] Sending packet ID " + std::to_string(packet.getId()) + " to server (" + std::to_string(buffer.size()) + " bytes)");
        }

        void NetworkManager::sendToClient(const Packet& packet, int playerEntityId) {
             // Serialize packet
            std::vector<uint8_t> buffer;
            packet.encode(buffer);
            
            NativeLog::info("[Network] Sending packet ID " + std::to_string(packet.getId()) + " to player " + std::to_string(playerEntityId));
        }

        void CustomPacket::encode(std::vector<uint8_t>& buffer) const {
            // Simple string encoding: length + chars
            size_t len = mData.size();
            buffer.push_back((uint8_t)(len >> 24));
            buffer.push_back((uint8_t)(len >> 16));
            buffer.push_back((uint8_t)(len >> 8));
            buffer.push_back((uint8_t)(len));
            for (char c : mData) buffer.push_back((uint8_t)c);
        }

        void CustomPacket::decode(const std::vector<uint8_t>& buffer) {
            if (buffer.size() < 4) return;
            size_t len = ((size_t)buffer[0] << 24) | ((size_t)buffer[1] << 16) | ((size_t)buffer[2] << 8) | (size_t)buffer[3];
            if (buffer.size() < 4 + len) return;
            mData = std::string((const char*)&buffer[4], len);
        }

    }
}
