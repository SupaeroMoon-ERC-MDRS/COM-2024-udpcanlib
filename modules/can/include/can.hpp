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
                /// @brief The internal byte array buffer.
                std::vector<uint8_t> buf;

            public:
                /// @brief Constructs a Bitarray with Bitarray::buf resized to a given size.
                /// @param size The size of Bitarray::buf.
                Bitarray(const uint32_t size);

                /// @brief Constructs a Bitarray with Bitarray::buf initialized to a given byte array.
                /// @param init The byte array.
                Bitarray(const std::vector<uint8_t>& init);

                /// @brief Constructs a Bitarray with Bitarray::buf initialized to a given size with a given number at the beginning that might be more than 1 byte long.
                /// @tparam IntType Can be any of uint8_t, uint16_t, uint32_t, uint64_t and their signed variants.
                /// @param v The number to place at the beginning of Bitarray::buf.
                /// @param size The total size of Bitarray::buf.
                template<typename IntType>
                Bitarray(const IntType v, const uint32_t size){
                    buf.resize(size);
                    *(IntType*)&buf[0] = v;
                }

                /// @brief A shorthand for Bitarray::buf.cbegin().
                /// @return The const_iterator from Bitarray::buf.cbegin().
                std::vector<uint8_t>::const_iterator cbegin(){return buf.cbegin();}

                /// @brief A shorthand for Bitarray::buf.cend().
                /// @return The const_iterator from Bitarray::buf.cend().
                std::vector<uint8_t>::const_iterator cend(){return buf.cend();}

                /// @brief Destructor, clears the internal buffer.
                ~Bitarray();

                /// @brief Performs binary AND operation on the two Bitarray objects. Same size is assumed.
                /// @param rhs The right hand side operand.
                /// @return The result of the binary AND operation.
                Bitarray operator&(const Bitarray& rhs) const;

                /// @brief Performs binary OR operation on the two Bitarray objects. Same size is assumed.
                /// @param rhs The right hand side operand.
                /// @return The result of the binary OR operation.
                Bitarray operator|(const Bitarray& rhs) const;

                /// @brief Performs right shift operation by a given number of bits.
                /// @param rhs The number of bits to shift right.
                /// @return The result of the right shift operation.
                Bitarray operator>>(const uint32_t rhs) const;

                /// @brief Performs left shift operation by a given number of bits.
                /// @param rhs The number of bits to shift left.
                /// @return The result of the left shift operation.
                Bitarray operator<<(const uint32_t rhs) const;

                /// @brief Performs in-place binary AND operation on the two Bitarray objects. Same size is assumed.
                /// @param rhs The right hand side operand.
                /// @return A reference to the modified left hand side operand.
                Bitarray& operator&=(const Bitarray& rhs);

                /// @brief Performs in-place binary OR operation on the two Bitarray objects. Same size is assumed.
                /// @param rhs The right hand side operand.
                /// @return A reference to the modified left hand side operand.
                Bitarray& operator|=(const Bitarray& rhs);

                /// @brief Performs in-place right shift operation by a given number of bits.
                /// @param rhs The number of bits to shift right.
                /// @return A reference to the modified left hand side operand.
                Bitarray& operator>>=(const uint32_t rhs);

                /// @brief Sets a specified number of bits to 1, starting from a given bit position.
                /// @param start The starting bit position.
                /// @param length The number of bits to set.
                /// @return A reference to the modified operand.
                Bitarray& set(const uint32_t start, const uint32_t length);

                /// @brief A shorthand for Bitarray::buf.size().
                /// @return The size of the Bitarray in bytes.
                uint32_t size() const;

                /// @brief Reads an integer of type IntType from the beginning of the bit array.
                /// @tparam IntType Can be any of uint8_t, uint16_t, uint32_t, uint64_t and their signed variants.
                /// @return The integer read.
                template<typename IntType>
                IntType as() const;

                /// @brief Gives access to a copy of the internal byte array.
                /// @return A copy of the internal byte array.
                std::vector<uint8_t> get() const;
        };

        /// @brief Describes a variable-length signal (vector) within a CAN message.
        class CanVectorSignalDesc{
            public:
                /// @brief The numeric type of the signal's elements.
                ENumType num_type_id;
                /// @brief The name of the signal.
                std::string name;

                /// @brief Default constructor.
                CanVectorSignalDesc();
                /// @brief Destructor.
                ~CanVectorSignalDesc();

                /// @brief Parses a vector signal description from a DBC file stream. Leaves the input file stream position at the end of the signal definition
                /// @param in The input file stream, positioned at the start of the signal definition.
                /// @param eof The end-of-file position for the stream.
                /// @return @ref CAN_E_SUCCESS on success \n
                /// @ref CAN_E_I_EOF on parsing error \n
                /// @ref CAN_E_FILE_NUM_READ_ERRNO on string to number conversion errors. In this case its recommended for the caller to check the errno variable.
                uint32_t parse(std::ifstream& in, const uint64_t eof);

                /// @brief Decodes a vector signal from the message payload.
                /// @tparam NumType The data type to decode into. Must be coherent with CanVectorSignalDesc::num_type_id
                /// @param message_payload The raw byte payload of the message.
                /// @param out The decoded vector signal is written into this reference.
                /// @param start_pos The starting position of this vector signal within the payload.
                /// @param end_pos The position after the last byte of this signal is written into this reference.
                /// @return @ref CAN_E_SUCCESS on success \n
                /// @ref CAN_E_VECTOR_LEN when there are insufficient bytes in the payload.
                template<typename NumType>
                uint32_t decode(const std::vector<uint8_t>& message_payload, std::vector<NumType>& out, const uint32_t start_pos, uint32_t& end_pos) const;

                /// @brief Encodes a vector of values into a byte stream.
                /// @tparam NumType The data type of the values in the vector.Must be coherent with CanVectorSignalDesc::num_type_id
                /// @param val The std::any containing a std::vector<NumType> to be encoded.
                /// @param out The encoded byte stream is written into this reference.
                /// @return @ref CAN_E_SUCCESS on success.
                template<typename NumType>
                uint32_t encode(const std::any val, std::vector<uint8_t>& out) const;
        };

        /// @brief Describes a fixed-size signal within a CAN message.
        class CanSignalDesc{
            private:
                /// @brief Bitmask to isolate the signal within the message payload.
                Bitarray mask;
                /// @brief Bit position to right-shift after masking to get the raw integer value.
                uint32_t shift;
                /// @brief Scale factor to apply to the raw integer value.
                float scale;
                /// @brief Offset to apply after scaling.
                float offset;

                /// @brief Determines the most appropriate numeric type for the signal based on its properties.
                /// @param sign The sign character ("+" or "-") from the DBC file.
                /// @param length The bit length of the signal.
                /// @param scale The scale factor.
                /// @param offset The offset value.
                /// @return The determined ENumType.
                static ENumType determineNumType(const std::string& sign, const uint32_t length, const float scale, const float offset);

                /// @brief Determines the most appropriate raw integer type for the signal.
                /// @param sign The sign character ("+" or "-") from the DBC file.
                /// @param length The bit length of the signal.
                /// @return The determined EIntType.
                static EIntType determineIntType(const std::string& sign, const uint32_t length);

            public:
                /// @brief The final numeric type of the signal.
                ENumType num_type_id;
                /// @brief The intermediate integer type used for coding the signal values.
                EIntType int_type_id;
                /// @brief The name of the signal.
                std::string name;

                /// @brief Default constructor.
                CanSignalDesc();
                /// @brief Constructs a signal descriptor for a message of a given length.
                /// @param message_length The length of the parent CAN message in bytes.
                CanSignalDesc(const uint32_t message_length);
                /// @brief Destructor.
                ~CanSignalDesc();

                /// @brief Parses a signal description from a DBC file stream. Leaves the input file stream position at the end of the signal definition
                /// @param in The input file stream, positioned at the start of the signal definition.
                /// @param eof The end-of-file position for the stream.
                /// @return @ref CAN_E_SUCCESS on success, or an error code on failure \n
                /// @ref CAN_E_SIGNAL_POS_OB if signal is out of bounds \n
                /// @ref CAN_E_I_EOF on parsing error \n
                /// @ref CAN_E_FILE_NUM_READ_ERRNO on string to number conversion errors. In this case its recommended for the caller to check the errno variable.
                uint32_t parse(std::ifstream& in, const uint64_t eof);

                /// @brief Decodes the signal value from the message payload.
                /// @tparam NumType The data type to decode into. Must be coherent with CanSignalDesc::num_type_id
                /// @param message_payload_bits The bit array representing the fixed-size part of the message payload.
                /// @param out The decoded value is written into this reference.
                /// @return @ref CAN_E_SUCCESS on success.
                template<typename NumType>
                uint32_t decode(const Bitarray& message_payload_bits, NumType& out) const;
                
                /// @brief Encodes a numeric value into a Bitarray for this signal.
                /// @tparam NumType The data type of the input number. Must be coherent with CanSignalDesc::num_type_id
                /// @param num The std::any containing the number to encode.
                /// @param out The output Bitarray, correctly shifted and sized for the parent message.
                /// @return @ref CAN_E_SUCCESS on success.
                template<typename NumType>
                uint32_t encode(const std::any num, Bitarray& out) const;
        };

        /// @brief Describes a single CAN message, including its ID, name, and signals.
        class CanMessageDesc{
            private:
                /// @brief A map of fixed-size signals in this message, keyed by signal name.
                std::map<std::string, CanSignalDesc> signals;
                /// @brief A vector of variable-length (vector) signals in this message.
                std::vector<CanVectorSignalDesc> vector_signals;

            public:
                /// @brief The ID of the CAN message.
                uint8_t id;
                /// @brief The length of the fixed-size portion of the message in bytes.
                uint32_t message_length; 
                /// @brief The name of the message.
                std::string name;

                /// @brief Default constructor.
                CanMessageDesc();
                /// @brief Destructor.
                ~CanMessageDesc();
                
                /// @brief Parses a message definition (and its signals) from a DBC file stream, leaves file stream position at end start of the message definition.
                /// @param in The input file stream, positioned at the start of the message definition.
                /// @param eof The end-of-file position for the stream.
                /// @return @ref CAN_E_SUCCESS on success, or an error code on failure \n
                /// @ref CAN_E_SIGNAL_POS_OB if signal is out of bounds \n
                /// @ref CAN_E_I_EOF on parsing error \n
                /// @ref CAN_E_FILE_UNEXPECTED_EOF when file ended with no signals declared \n
                /// @ref CAN_E_FILE_NUM_READ_ERRNO on string to number conversion errors. In this case its recommended for the caller to check the errno variable.
                uint32_t parse(std::ifstream& in, const uint64_t eof);

                /// @brief Gets the names of all signals in this message.
                /// @param vec A set to which the signal names will be added.
                void getSignalNames(std::set<std::string>& vec) const;

                /// @brief Gets the types of all non-vector signals in this message.
                /// @return A map of signal names to their corresponding ENumType.
                std::map<std::string, ENumType> getSignalTypes() const;

                /// @brief Decodes a raw message payload into a map of signal values.
                /// @param message_payload The byte vector containing the message payload (without the ID).
                /// @param out The map where decoded signal names and values (in std::any) will be stored.
                /// @param msg_size The total size of the message including vector signals will be written here.
                /// @return @ref CAN_E_SUCCESS on success.
                uint32_t decode(const std::vector<uint8_t>& message_payload, std::map<std::string, std::any>& out, uint32_t& msg_size) const;
                
                /// @brief Encodes a map of signal values into a full raw CAN message (ID + payload).
                /// @param in The map of signal names to std::any values to encode.
                /// @param out The output byte vector where the encoded message will be stored.
                /// @return @ref CAN_E_SUCCESS on success.
                uint32_t encode(const std::map<std::string, std::any>& in, std::vector<uint8_t>& out) const;
        };

        /// @brief Represents a full CAN database parsed from a DBC file.
        class CanDatabase{
            private:
                /// @brief A map of all message descriptors in the database, keyed by message ID.
                std::map<uint8_t, CanMessageDesc> messages;

                /// @brief Validates the version string from the DBC file and extracts the version number.
                /// @param v The first line of the DBC file.
                /// @return @ref CAN_E_SUCCESS on success \n
                /// @ref CAN_E_FILE_NO_DBC_VERSION when no DBC version was found
                uint32_t validateDBCVersion(const std::string& v);

            public:
                /// @brief The version of the loaded DBC file.
                uint16_t dbc_version;
                
                /// @brief Default constructor.
                CanDatabase();
                /// @brief Destructor.
                ~CanDatabase();

                /// @brief Parses a DBC file and loads all its message and signal definitions.
                /// @param fn The path to the DBC file.
                /// @return @ref CAN_E_SUCCESS on success, or a file/parsing error code on failure.
                uint32_t parse(const std::string& fn);

                /// @brief Gets the fixed-size length for each message in the database.
                /// @return A vector of pairs, where each pair contains a message ID and its fixed-size length.
                std::vector<std::pair<uint8_t, uint32_t>> getMessageSizes() const;

                /// @brief Gets a list of all unique signal names across all messages in the database.
                /// @return A vector of strings containing the signal names.
                std::vector<std::string> getSignalNames() const;

                /// @brief Gets a map of all signal names to their types across all messages.
                /// @return A map of signal names to their corresponding ENumType.
                std::map<std::string, ENumType> getSignalTypes() const;
                
                /// @brief Decodes a byte stream that may contain multiple concatenated CAN messages.
                /// @param message_all The raw byte stream.
                /// @param out A vector of pairs, where each pair contains the message ID and a map of its decoded signals.
                /// @return @ref CAN_E_SUCCESS on success \n
                /// @ref CAN_E_UNKNOWN_MSG_ID if an ID is not in the database \n
                /// @ref CAN_E_PARTIAL_MSG for incomplete messages.
                uint32_t decode(const std::vector<uint8_t>& message_all, std::vector<std::pair<uint8_t, std::map<std::string, std::any>>>& out) const;
                
                /// @brief Encodes a single message given its ID and a map of its signal values.
                /// @param id The ID of the message to encode.
                /// @param in A map of signal names and their values (as std::any) for the specified message.
                /// @param all_out The output byte vector where the full encoded message (ID + payload) will be written.
                /// @return @ref CAN_E_SUCCESS on success \n
                /// @ref CAN_E_I_SIGNAMES if input signal names do not match the message definition.
                uint32_t encode(const uint8_t id, const std::map<std::string, std::any>& in, std::vector<uint8_t>& all_out) const;
        };
    };
};