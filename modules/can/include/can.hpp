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
#include "definitions.h"

/// @brief A namespace to contain all udpcanlib code
namespace udpcan{

    /// @brief A namespace to contain trusted code, not meant to be used by user/includer
    namespace internal{
        
        /// @brief An enum to show the type of a signal value
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

        /// @brief An enum to show the integer type a signal value is mapped to
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

        /// @brief Byte allocated to each signal value of type ENumType in a CanVectorSignalDesc
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

        /// @brief Checks whether character is alphanumeric or +-_
        /// @param c Character to check
        /// @return True if character is alphanumeric or +-_
        bool isValidString(const char c);

        /// @brief Opens a file for reading at its end, acquiring its eof then seeking to its beginning
        /// @param fn The filename
        /// @param end The eof will be copied into this reference
        /// @param in The input file stream will be copied into this reference
        /// @return @ref CAN_E_SUCCESS on success \n
        /// @ref CAN_E_FILE_FS_MALFORM when the file appears corrupted \n
        /// @ref CAN_E_FILE_OPEN_ERRNO on other errors. In this case its recommended for the caller to check the errno variable.
        uint32_t openRead(const std::string& fn, uint64_t& end, std::ifstream& in);

        /// @brief Reads a file until a character is found. If it is not found the file contents until eof are returned.
        /// @param in The input file stream
        /// @param eof The eof
        /// @param delim The character to stop at
        /// @return The string from the current position of the input file stream until the delimiter
        std::string readUntil(std::ifstream& in, const uint64_t eof, const char delim);

        /// @brief Reads and ignores contents of a file until a pattern is found, leaving the input file stream position at the beginning of the pattern
        /// @param in The input file stream
        /// @param eof The eof
        /// @param pattern The pattern to stop at
        /// @return @ref CAN_E_SUCCESS on success \n
        /// @ref CAN_E_I_EOF when the pattern was not found. This leaves the input file stream position at eof
        uint32_t seekUntil(std::ifstream& in, const uint64_t eof, const std::string& pattern);

        /// @brief Reads the next integer number from the input file stream. Characters before finding the number are ignored
        /// @param in The input file stream
        /// @param eof The eof
        /// @param value The found number will be copied here when @ref CAN_E_SUCCESS is returned
        /// @return @ref CAN_E_SUCCESS on success \n
        /// @ref CAN_E_I_EOF eof was reached before a number was found \n
        /// @ref CAN_E_FILE_NUM_READ_ERRNO on other errors. In this case its recommended for the caller to check the errno variable.
        uint32_t readNextNumeric(std::ifstream& in, const uint64_t eof, uint32_t& value);

        /// @brief Reads the next floating point number from the input file stream. Characters before finding the number are ignored
        /// @param in The input file stream
        /// @param eof The eof
        /// @param value The found number will be copied here when @ref CAN_E_SUCCESS is returned
        /// @return @ref CAN_E_SUCCESS on success \n
        /// @ref CAN_E_I_EOF eof was reached before a number was found \n
        /// @ref CAN_E_FILE_NUM_READ_ERRNO on other errors. In this case its recommended for the caller to check the errno variable.
        uint32_t readNextFloating(std::ifstream& in, const uint64_t eof, float& value);

        /// @brief Reads the next string from the input file stream as per @ref isValidString. Characters before finding the number are ignored
        /// @param in The input file stream
        /// @param eof The eof
        /// @param value The found string will be copied here when @ref CAN_E_SUCCESS is returned
        /// @return @ref CAN_E_SUCCESS on success \n
        /// @ref CAN_E_I_EOF eof was reached before a number was found
        uint32_t readNextString(std::ifstream& in, const uint64_t eof, std::string& value);


        /// @brief A struct to implement bit level control over a byte array
        struct Bitarray{
            private:
                /// @brief The byte array
                std::vector<uint8_t> buf;

            public:
                /// @brief Constructs a Bitarray with Bitarray::buf resized to a given size
                /// @param size The size of Bitarray::buf
                Bitarray(const uint32_t size);

                /// @brief Constructs a Bitarray with Bitarray::buf initialized to a given byte array
                /// @param init The byte array
                Bitarray(const std::vector<uint8_t>& init);

                /// @brief Constructs a Bitarray with Bitarray::buf initialized to a given size with a given number at the beginning that might be more than 1 byte long.
                /// @tparam IntType Can be any of uint8_t, uint16_t, uint32_t, uint64_t and their signed variants
                /// @param v The number at the beginning of Bitarray::buf
                /// @param size The size of Bitarray::buf
                template<typename IntType>
                Bitarray(const IntType v, const uint32_t size){
                    buf.resize(size);
                    *(IntType*)&buf[0] = v;
                }

                /// @brief A shorthand for Bitarray::buf.cbegin()
                /// @return The iterator from Bitarray::buf.cbegin()
                std::vector<uint8_t>::const_iterator cbegin(){return buf.cbegin();}

                /// @brief A shorthand for Bitarray::buf.cend()
                /// @return The iterator from Bitarray::buf.cend()
                std::vector<uint8_t>::const_iterator cend(){return buf.cend();}

                ~Bitarray();

                /// @brief Performs binary and operation on the two Bitarray objects. Same size is assumed
                /// @param rhs The right hand side operand
                /// @return The result of the binary and operation
                Bitarray operator&(const Bitarray& rhs) const;

                /// @brief Performs binary or operation on the two Bitarray objects. Same size is assumed
                /// @param rhs The right hand side operand
                /// @return The result of the binary or operation
                Bitarray operator|(const Bitarray& rhs) const;

                /// @brief Performs right shift operation by rhs
                /// @param rhs The right hand side operand
                /// @return The result of the right shift operation
                Bitarray operator>>(const uint32_t rhs) const;

                /// @brief Performs left shift operation by rhs
                /// @param rhs The right hand side operand
                /// @return The result of the left shift operation
                Bitarray operator<<(const uint32_t rhs) const;

                /// @brief Performs in place binary and operation on the two Bitarray objects. Same size is assumed
                /// @param rhs The right hand side operand
                /// @return A reference of the left hand side operand after the binary and operation
                Bitarray& operator&=(const Bitarray& rhs);

                /// @brief Performs in place binary or operation on the two Bitarray objects. Same size is assumed
                /// @param rhs The right hand side operand
                /// @return A reference of the left hand side operand after the binary or operation
                Bitarray& operator|=(const Bitarray& rhs);

                /// @brief Performs in place right shift operation by rhs
                /// @param rhs The right hand side operand
                /// @return A reference of the left hand side operand after the right shift operation
                Bitarray& operator>>=(const uint32_t rhs);

                /// @brief Sets lenght amount of bits from bit position start
                /// @param start The starting bit position
                /// @param length The number of bits to set
                /// @return A reference of the left hand side operand after the bits were set
                Bitarray& set(const uint32_t start, const uint32_t length);

                /// @brief A shorthand for Bitarray::buf.size()
                /// @return The size of the Bitarray in bytes
                uint32_t size() const;

                /// @brief Reads an integer of IntType 
                /// @tparam IntType Can be any of uint8_t, uint16_t, uint32_t, uint64_t and their signed variants
                /// @return The integer read
                template<typename IntType>
                IntType as() const;

                /// @brief Gives access to a copy of the internal byte array
                /// @return The copy of the internal byte array
                std::vector<uint8_t> get() const;
        };

        class CanVectorSignalDesc{
            public:
                ENumType num_type_id;
                std::string name;

                CanVectorSignalDesc();
                ~CanVectorSignalDesc();

                uint32_t parse(std::ifstream& in, const uint64_t eof);

                template<typename NumType>
                uint32_t decode(const std::vector<uint8_t>& message_payload, std::vector<NumType>& out, const uint32_t start_pos, uint32_t& end_pos) const;

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