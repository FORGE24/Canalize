#pragma once
#include "../../CanalizeAPI.h"
#include <vector>
#include <string>
#include <cstdint>

// Disable MSVC warning C4251 for STL containers in DLL-exported classes
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace Canalize {
    namespace Network {

        // Base packet class
        class CANALIZE_API Packet {
        public:
            virtual ~Packet() = default;
            virtual void encode(std::vector<uint8_t>& buffer) const = 0;
            virtual void decode(const std::vector<uint8_t>& buffer) = 0;
            virtual int getId() const = 0;
        };

        // Network manager to send/receive packets
        class CANALIZE_API NetworkManager {
        public:
            static NetworkManager& getInstance();

            void sendToServer(const Packet& packet);
            void sendToClient(const Packet& packet, int playerEntityId); // Simplified target

            // Register a packet handler
            // template<typename T> void registerPacket(int id, std::function<void(const T&, Context&)> handler);
        };

        // Example custom packet
        class CANALIZE_API CustomPacket : public Packet {
        public:
            CustomPacket() = default;
            CustomPacket(const std::string& data) : mData(data) {}

            void encode(std::vector<uint8_t>& buffer) const override;
            void decode(const std::vector<uint8_t>& buffer) override;
            int getId() const override { return 1; }

            std::string getData() const { return mData; }

        private:
            std::string mData;
        };
    }
}

// Re-enable warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
