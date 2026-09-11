#pragma once

#include <random>
#include <sstream>

#include <aiko_types.h>

namespace aiko
{
    namespace uuid
    {

        class Uuid
        {
        public:

            using Id = string;

            Uuid()
                : m_uuid(generate_uuid_v4())
            {

            }

            Uuid(Id uuid)
                : m_uuid(std::move(uuid))
            {

            }

            const Id& get() const
            {
                return m_uuid;
            }

            bool operator==(const Uuid& other) const
            {
                return m_uuid == other.m_uuid;
            }

        private:

            Id m_uuid;

            static Id generate_uuid_v4()
            {

                static std::random_device              rd;
                static std::mt19937                    gen(rd());
                static std::uniform_int_distribution<> dis(0, 15);
                static std::uniform_int_distribution<> dis2(8, 11);

                std::stringstream ss;
                int i;
                ss << std::hex;
                for (i = 0; i < 8; i++)
                {
                    ss << dis(gen);
                }
                ss << "-";
                for (i = 0; i < 4; i++)
                {
                    ss << dis(gen);
                }
                ss << "-4";
                for (i = 0; i < 3; i++)
                {
                    ss << dis(gen);
                }
                ss << "-";
                ss << dis2(gen);
                for (i = 0; i < 3; i++)
                {
                    ss << dis(gen);
                }
                ss << "-";
                for (i = 0; i < 12; i++)
                {
                    ss << dis(gen);
                };
                return ss.str();
            }

        };

    }
}

namespace std
{

    template<>
    struct hash<aiko::uuid::Uuid>
    {
        size_t operator()(const aiko::uuid::Uuid& id) const noexcept
        {
            return hash<std::string>()(id.get());
        }
    };
}
