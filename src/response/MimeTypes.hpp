#pragma once

#include "../webserv.hpp"

class MimeTypes
{
    private:
        std::map<std::string, std::string>  mime_types;

    public:
        MimeTypes();
        ~MimeTypes();

        const std::string   getMimeType(const std::string&);
        const std::string   getExeFile(const std::string&);
};
