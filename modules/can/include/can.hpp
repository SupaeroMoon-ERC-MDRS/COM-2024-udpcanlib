#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include "message_definitions.hpp"

namespace udpcan{
    namespace internal{     

        bool isValidString(const char c);
        uint32_t openRead(const std::string& fn, uint64_t& end, std::ifstream& in);
        std::string readUntil(std::ifstream& in, const uint64_t eof, const char delim);
        uint32_t seekUntil(std::ifstream& in, const uint64_t eof, const std::string& pattern);
        uint32_t readNextNumeric(std::ifstream& in, const uint64_t eof, uint32_t& value);
        uint32_t readNextFloating(std::ifstream& in, const uint64_t eof, float& value);
        uint32_t readNextString(std::ifstream& in, const uint64_t eof, std::string& value);


        struct Bitarray{
            private:
                std::vector<uint8_t> buf;
            public:
                Bitarray(const uint32_t size);
                Bitarray(const std::vector<uint8_t>& init);
                ~Bitarray();
                Bitarray operator&(const Bitarray& rhs) const;
                Bitarray operator|(const Bitarray& rhs) const;
                Bitarray operator>>(const uint32_t rhs) const;

                Bitarray& operator&=(const Bitarray& rhs);
                Bitarray& operator|=(const Bitarray& rhs);
                Bitarray& operator>>=(const uint32_t rhs);

                Bitarray& set(const uint32_t start, const uint32_t length);
                uint32_t size() const;

                template<typename IntType>
                IntType as() const;

                std::vector<uint8_t> get() const;
        };

        enum ENumType64{
            UINT = 0,
            INT,
            FLOAT,
        };

        enum EIntType{
            U8 = 0,
            U16,
            U32,
            U64,
            I8,
            I16,
            I32,
            I64,
        };

        class CanSignalDesc{
            private:
                Bitarray mask;
                uint32_t shift;
                float scale;
                float offset;

                static ENumType64 determineNumType64(const std::string& sign, const float scale, const float offset);
                static EIntType determineIntType(const std::string& sign, const uint32_t length);

            public:
                ENumType64 num_type64_id;
                EIntType int_type_id;
                std::string name;

                CanSignalDesc();
                CanSignalDesc(const uint32_t message_length);
                ~CanSignalDesc();

                uint32_t parse(std::ifstream& in, const uint64_t eof);

                template<typename NumType64>
                uint32_t decode(const Bitarray& message_payload_bits, NumType64& out) const;

                template<typename NumType64>
                uint32_t encode(const NumType64 num, const uint32_t message_lenght, Bitarray& out) const;
        };

        class CanMessageDesc{
            private:
                std::map<std::string, CanSignalDesc> signals;

            public:
                uint8_t id;
                uint32_t message_length;
                std::string name;

                CanMessageDesc();
                ~CanMessageDesc();
                
                uint32_t parse(std::ifstream& in, const uint64_t eof);

                uint32_t decode(const Bitarray& message_payload_bits, std::map<std::string, int64_t>& out_int, std::map<std::string, float>& out_float) const;
                uint32_t encode(const std::map<std::string, int64_t>& int_values, const std::map<std::string, float>& float_values, Bitarray& out) const;
        };

        class CanDatabase{
            private:
                uint16_t dbc_version;
                std::map<uint8_t, CanMessageDesc> messages;

                uint32_t validateDBCVersion(const std::string& v);

            public:
                CanDatabase();
                ~CanDatabase();

                uint32_t parse(const std::string& fn);
                
                uint32_t decode(const Bitarray& message_all_bits, std::map<std::string, int64_t>& out_int, std::map<std::string, float>& out_float) const;
                uint32_t encode(const uint8_t id, const std::map<std::string, int64_t>& int_values, const std::map<std::string, float>& float_values, Bitarray& all_out) const;
        };
    };
};