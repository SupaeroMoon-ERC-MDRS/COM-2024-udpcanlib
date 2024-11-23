#pragma once
#include "message_definitions.hpp"

class CanSignalDesc{

};

class CanMessageDesc{

};

class CanDatabase{
    private:

    public:
        CanDatabase();
        ~CanDatabase();

        uint32_t parse(const std::string fn);
};