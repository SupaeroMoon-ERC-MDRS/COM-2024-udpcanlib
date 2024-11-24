#pragma once
#include <map>
#include "message_definitions.hpp"

namespace udpcan{
    namespace internal{

        struct Bitarray{
            private:
                std::vector<uint8_t> buf;
            public:
                Bitarray(const std::vector<uint8_t>& init);
                ~Bitarray();
                Bitarray operator&(const Bitarray& rhs);
                Bitarray operator|(const Bitarray& rhs);
                Bitarray operator>>(const uint32_t rhs);

                template<typename IntType>
                IntType as();

                std::vector<uint8_t> get();
        };

        class CanSignalDesc{
            private:
                Bitarray mask;
                uint32_t shift;
                uint32_t signal_length;
                float scale;
                float offset;

            public:
                std::string name;
                uint8_t typeId; // of scale and offset; 0: int32_t 1: float

                CanSignalDesc();
                ~CanSignalDesc();

                uint32_t parse(const std::string& line, const uint32_t message_lenght);

                template<typename NumType32>
                uint32_t decode(const Bitarray& message_payload_bits, NumType32& out);

                template<typename NumType32>
                uint32_t encode(const NumType32 num, const uint32_t message_lenght, Bitarray& out);
        };

        class CanMessageDesc{
            private:
                std::map<std::string, CanSignalDesc> signals;

            public:
                uint8_t id;
                uint32_t message_length;

                CanMessageDesc();
                ~CanMessageDesc();
                
                uint32_t parse(const std::string& lines);

                uint32_t decode(const Bitarray& message_payload_bits, std::map<std::string, int32_t>& out_int, std::map<std::string, float>& out_float);
                uint32_t encode(const std::map<std::string, int32_t>& int_values, const std::map<std::string, float>& float_values, Bitarray& out);
        };

        class CanDatabase{
            private:
                uint16_t dbc_version;
                std::map<uint8_t, CanMessageDesc> messages;

            public:
                CanDatabase();
                ~CanDatabase();

                uint32_t parse(const std::string& fn);
                
                uint32_t decode(const Bitarray& message_all_bits, std::map<std::string, int32_t>& out_int, std::map<std::string, float>& out_float);
                uint32_t encode(const uint8_t id, const std::map<std::string, int32_t>& int_values, const std::map<std::string, float>& float_values, Bitarray& all_out);
        };
    };
};