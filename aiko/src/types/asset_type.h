#pragma once

#include <random>

namespace aiko
{

    class Aiko;
    class AssetSystem;
    namespace asset
    {

        using ID = unsigned long long;
        constexpr ID INVALID_ID = (ID) -1;

        class Asset
        {
        private:
            ID m_id = INVALID_ID;
            static ID generateID()
            {
                std::random_device rd;
                std::mt19937_64 gen(rd()); // Generator initialized with random_device
                std::uniform_int_distribution<ID> dis;
                return dis(gen); // Generate a random 64-bit number
            }
        public:
            friend class AssetSystem;
            virtual void unload() { };
            Asset() : m_id(generateID()) { }
            ~Asset() = default;
            ID getID() const { return m_id; }

        protected:
            // TODO REMOVE and find better way to link asset to systesm
            // Maybe something like the modules?
            virtual void connect() { };
            AssetSystem* m_assetSystem;
        };

        class TextAsset : public Asset
        {
        private:
            const char* data;
        public:
            TextAsset(const char* d) : Asset(), data(d) {} // Correctly initializing `data`
            ~TextAsset() = default;
            const char* getData() const { return data; } // Getter for `data`
        };

    }
}
