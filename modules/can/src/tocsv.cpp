#include <iostream>

#include "can.hpp"

using namespace udpcan::internal;

void writeLine(std::ofstream& of, const std::vector<std::any>& line, const std::vector<std::string>& signals, const std::map<std::string, ENumType>& sig_type, const std::vector<bool>& has_value){
    for(uint32_t i = 0; i < line.size() - 1; i++){
        if(!has_value[i]){
            of << "0,";
            continue;
        }
        ENumType type = sig_type.at(signals[i]);
        if(type == ENumType::NU8) of << (uint16_t)std::any_cast<uint8_t>(line[i]) << ",";
        if(type == ENumType::NU16) of << std::any_cast<uint16_t>(line[i]) << ",";
        if(type == ENumType::NU32) of << std::any_cast<uint32_t>(line[i]) << ",";
        if(type == ENumType::NU64) of << std::any_cast<uint64_t>(line[i]) << ",";
        if(type == ENumType::NI8) of << (int16_t)std::any_cast<int8_t>(line[i]) << ",";
        if(type == ENumType::NI16) of << std::any_cast<int16_t>(line[i]) << ",";
        if(type == ENumType::NI32) of << std::any_cast<int32_t>(line[i]) << ",";
        if(type == ENumType::NI64) of << std::any_cast<int64_t>(line[i]) << ",";
        if(type == ENumType::NF32) of << std::any_cast<float>(line[i]) << ",";
    }
    if(!has_value.back()){
        of << "0" << std::endl;
        return;
    }
    ENumType type = sig_type.at(signals.back());
    if(type == ENumType::NU8) of << (uint16_t)std::any_cast<uint8_t>(line.back()) << std::endl;
    if(type == ENumType::NU16) of << std::any_cast<uint16_t>(line.back()) << std::endl;
    if(type == ENumType::NU32) of << std::any_cast<uint32_t>(line.back()) << std::endl;
    if(type == ENumType::NU64) of << std::any_cast<uint64_t>(line.back()) << std::endl;
    if(type == ENumType::NI8) of << (int16_t)std::any_cast<int8_t>(line.back()) << std::endl;
    if(type == ENumType::NI16) of << std::any_cast<int16_t>(line.back()) << std::endl;
    if(type == ENumType::NI32) of << std::any_cast<int32_t>(line.back()) << std::endl;
    if(type == ENumType::NI64) of << std::any_cast<int64_t>(line.back()) << std::endl;
    if(type == ENumType::NF32) of << std::any_cast<float>(line.back()) << std::endl;
}

int32_t main(int32_t argc, char** argv){
    if(argc != 4){
        std::cout << "Usage: tocsv [DBC File] [INPUT] [OUTPUT.csv]" << std::endl;
        return -1;
    }

    std::string dbc(argv[1]);
    std::string infile(argv[2]);
    std::string outfile(argv[3]);

    CanDatabase db;
    uint32_t res;
    CAN_E_FW_IF_ERR(db.parse(dbc))

    uint64_t eof;
    std::ifstream in;
    CAN_E_FW_IF_ERR(openRead(infile, eof, in))

    std::ofstream of(outfile);

    std::vector<std::string> csv_header = {"time"};
    std::vector<std::string> signals = db.getSignalNames();
    std::copy(signals.cbegin(), signals.cend(), std::back_inserter(csv_header));

    std::map<std::string, ENumType> signal_types = db.getSignalTypes();
    signal_types["time"] = ENumType::NU32;

    std::vector<std::any> last_line(csv_header.size());
    std::vector<bool> has_value(csv_header.size(), false);

    for(uint32_t i = 0; i < csv_header.size() - 1; i++){
        of << csv_header[i] << ',';
    }
    of << csv_header.back() << std::endl;

    uint64_t pos = in.tellg();
    while(pos < eof){
        if(pos + 8 > eof){
            break;
        }

        uint32_t len;
        uint32_t time;
        in.read((char *)&len, 4);
        in.read((char *)&time, 4);
        pos += 8;

        if(pos + len > eof){
            break;
        }

        std::vector<uint8_t> vec(len);
        in.read((char *)vec.data(), len);

        std::vector<std::pair<uint8_t, std::map<std::string, std::any>>> out;
        CAN_E_FW_IF_ERR(db.decode(vec, out))

        last_line[0] = time;
        has_value[0] = true;

        for(uint32_t i = 0; i < csv_header.size(); i++){
            for(const std::pair<uint8_t, std::map<std::string, std::any>>& p : out){
                if(p.second.find(csv_header[i]) != p.second.end()){
                    last_line[i] = p.second.at(csv_header[i]);
                    has_value[i] = true;
                }
            }
        }        
        writeLine(of, last_line, csv_header, signal_types, has_value);
    }

    in.close();
    of.close();
    return CAN_E_SUCCESS;
}