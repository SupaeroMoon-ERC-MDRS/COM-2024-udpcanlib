#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include "message_definitions.hpp"

namespace udpcan{
    namespace internal{

        struct Bitarray{
            private:
                std::vector<uint8_t> buf;
            public:
                Bitarray(const std::vector<uint8_t>& init);
                Bitarray(const uint32_t size);
                ~Bitarray();
                Bitarray operator&(const Bitarray& rhs) const;
                Bitarray operator|(const Bitarray& rhs) const;
                Bitarray operator>>(const uint32_t rhs) const;

                Bitarray& operator&=(const Bitarray& rhs);
                Bitarray& operator|=(const Bitarray& rhs);
                Bitarray& operator>>=(const uint32_t rhs);

                template<typename IntType>
                IntType as() const;

                std::vector<uint8_t> get() const;
        };

        class CanSignalDesc{
            private:
                Bitarray mask;
                uint32_t shift;
                float scale;
                float offset;

            public:
                std::string name;

                CanSignalDesc(uint32_t message_length);
                ~CanSignalDesc();

                uint32_t parse(const std::string& line, const uint32_t message_lenght);

                template<typename NumType32>
                uint32_t decode(const Bitarray& message_payload_bits, NumType32 out) const;

                template<typename NumType32>
                uint32_t encode(const NumType32 num, const uint32_t message_lenght, Bitarray& out) const;
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

                uint32_t decode(const Bitarray& message_payload_bits, std::map<std::string, int32_t>& out_int, std::map<std::string, float>& out_float) const;
                uint32_t encode(const std::map<std::string, int32_t>& int_values, const std::map<std::string, float>& float_values, Bitarray& out) const;
        };

        class CanDatabase{
            private:
                uint16_t dbc_version;
                std::map<uint8_t, CanMessageDesc> messages;

                uint32_t read(const std::string& fn, uint64_t& end, std::ifstream& in) const;
                uint32_t validateDBCVersion(const std::string& v);

            public:
                CanDatabase();
                ~CanDatabase();

                uint32_t parse(const std::string& fn);
                
                uint32_t decode(const Bitarray& message_all_bits, std::map<std::string, int32_t>& out_int, std::map<std::string, float>& out_float) const;
                uint32_t encode(const uint8_t id, const std::map<std::string, int32_t>& int_values, const std::map<std::string, float>& float_values, Bitarray& all_out) const;
        };
    };
};