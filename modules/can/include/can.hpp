#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>
#include <cstdint>
#include <any>
#include <set>
#include <iostream>
#include "definitions.h"

namespace udpcan{
    namespace internal{
        
        enum ENumType{
            NU8 = 0,
            NU16,
            NU32,
            NU64,
            NI8,
            NI16,
            NI32,
            NI64,
            NF32
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

        const std::map<ENumType, uint8_t> type_size = {
            {ENumType::NU8, 1},
            {ENumType::NU16, 2},
            {ENumType::NU32, 4},
            {ENumType::NU64, 8},
            {ENumType::NI8, 1},
            {ENumType::NI16, 2},
            {ENumType::NI32, 4},
            {ENumType::NI64, 8},
            {ENumType::NF32, 4},
        };

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

                template<typename IntType>
                Bitarray(const IntType v, const uint32_t size){
                    buf.resize(size);
                    *(IntType*)&buf[0] = v;
                }

                std::vector<uint8_t>::const_iterator cbegin(){return buf.cbegin();}
                std::vector<uint8_t>::const_iterator cend(){return buf.cend();}

                ~Bitarray();
                Bitarray operator&(const Bitarray& rhs) const;
                Bitarray operator|(const Bitarray& rhs) const;
                Bitarray operator>>(const uint32_t rhs) const;
                Bitarray operator<<(const uint32_t rhs) const;

                Bitarray& operator&=(const Bitarray& rhs);
                Bitarray& operator|=(const Bitarray& rhs);
                Bitarray& operator>>=(const uint32_t rhs);

                Bitarray& set(const uint32_t start, const uint32_t length);
                uint32_t size() const;

                template<typename IntType>
                IntType as() const;

                std::vector<uint8_t> get() const;
        };

        class CanVectorSignalDesc{
            public:
                ENumType num_type_id;
                std::string name;

                CanVectorSignalDesc();
                ~CanVectorSignalDesc();

                uint32_t parse(std::ifstream& in, const uint64_t eof);

                // from start_pos, uint32_t length then length*num_type_id, sets end_pos as start_pos + 4 + length * sizeof(num_type_id)
                template<typename NumType>
                uint32_t decode(const std::vector<uint8_t>& message_payload, std::vector<NumType>& out, const uint32_t start_pos, uint32_t& end_pos) const;

                // creates a bitarray as 4 byte length + length * num_type_id. These are then concatenated message level.
                template<typename NumType>
                uint32_t encode(const std::any val, std::vector<uint8_t>& out) const;
        };

        class CanSignalDesc{
            private:
                Bitarray mask;
                uint32_t shift;
                float scale;
                float offset;

                static ENumType determineNumType(const std::string& sign, const uint32_t length, const float scale, const float offset);
                static EIntType determineIntType(const std::string& sign, const uint32_t length);

            public:
                ENumType num_type_id;
                EIntType int_type_id;
                std::string name;

                CanSignalDesc();
                CanSignalDesc(const uint32_t message_length);
                ~CanSignalDesc();

                uint32_t parse(std::ifstream& in, const uint64_t eof);

                template<typename NumType>
                uint32_t decode(const Bitarray& message_payload_bits, NumType& out) const;

                template<typename NumType>
                uint32_t encode(const std::any num, Bitarray& out) const;
        };

        class CanMessageDesc{
            private:
                std::map<std::string, CanSignalDesc> signals;
                std::vector<CanVectorSignalDesc> vector_signals;

            public:
                uint8_t id;
                uint32_t message_length; // before vectors
                std::string name;

                CanMessageDesc();
                ~CanMessageDesc();
                
                uint32_t parse(std::ifstream& in, const uint64_t eof);
                void getSignalNames(std::set<std::string>& vec) const;
                std::map<std::string, ENumType> getSignalTypes() const;

                uint32_t decode(const std::vector<uint8_t>& message_payload, std::map<std::string, std::any>& out, uint32_t& msg_size) const;
                uint32_t encode(const std::map<std::string, std::any>& in, std::vector<uint8_t>& out, const uint16_t version) const;
        };

        class CanDatabase{
            private:
                std::map<uint8_t, CanMessageDesc> messages;

                uint32_t validateDBCVersion(const std::string& v);

            public:
                uint16_t dbc_version;
                
                CanDatabase();
                ~CanDatabase();

                uint32_t parse(const std::string& fn);
                std::vector<std::pair<uint8_t, uint32_t>> getMessageSizes() const;
                std::vector<std::string> getSignalNames() const;
                std::map<std::string, ENumType> getSignalTypes() const;
                
                uint32_t decode(const std::vector<uint8_t>& message_all, std::vector<std::pair<uint8_t, std::map<std::string, std::any>>>& out) const;
                uint32_t encode(const uint8_t id, const std::map<std::string, std::any>& in, std::vector<uint8_t>& all_out) const;
        };
    };
};