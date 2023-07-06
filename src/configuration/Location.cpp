#include "Location.hpp"

Location::Location()
    : location_path(), method(), root(), upload(), autoindex(), index(), redirection()
{ }

Location::~Location() {
    this->clear();
}

Location::Location(const Location& other)
{
    *this = other;
}

Location& Location::operator=(const Location& other) {
    if (this != &other) {
        this->location_path = other.location_path;
        this->method = other.method;
        this->root = other.root;
        this->upload = other.upload;
        this->autoindex = other.autoindex;
        this->index = other.index;
        this->redirection = other.redirection;
        this->cgi = other.cgi;
    }
    return *this;
}

const std::vector<std::string>& Location::getMethod() const {
    return this->method;
}

void Location::setMethod(std::string value, bool check) {
    if  (check)
    {
        value = skip(value, "method");
        size_t pos = value.find("GET");
        if (pos != std::string::npos)
        {
            this->method.push_back("GET");
            value.erase(pos, 3);
        }
        pos = value.find("POST");
        if (pos != std::string::npos)
        {
            this->method.push_back("POST");
            value.erase(pos, 4);
        }

        pos = value.find("DELETE");
        if (pos != std::string::npos)
        {
            this->method.push_back("DELETE");
            value.erase(pos, 6);
        }
        value = skipWhitespaceBeginAnd(value);
    }
    else
        this->method.push_back(value);
}

const std::string& Location::getRoot() const {
    return root;
}

void Location::setRoot(std::string& value) {
    value = skip(value, "root");
    if (value[value.length() - 1] != '/')
        parse_error("root");
    root = value;
}

const std::string& Location::getUpload() const {
    return this->upload;
}

void Location::setUpload(std::string& value) {
    value = skip(value, "upload");
    if (value[value.length() - 1] != '/')
        parse_error("upload");
    this->upload = value;
}

const std::string& Location::getAutoindex() const {
    return this->autoindex;
}

void Location::setAutoindex(std::string& value) {
    value = skip(value, "autoindex");
    if (value != "on" && value != "off")
        parse_error("autoindex");
    this->autoindex = value;
}

const std::string& Location::getIndex() const { return this->index; }

void Location::setIndex(std::string& value) {
    value = skip(value, "index");
    this->index = value;
}

void    Location::clear()
{
    this->location_path.clear();
    this->method.clear();
    this->root.clear();
    this->upload.clear();
    this->autoindex.clear();
    this->index.clear();
    this->redirection.clear();
    this->cgi.clear();
}

const std::string& Location::getRedirection() const {
    return this->redirection;
}

void Location::setRedirection(std::string& value) {
    value = skip(value, "redirection");
    this->redirection = value;
}

const std::string& Location::getLocation() const {
    return this->location_path;
}

void Location::setLocation(std::string& value){
    if (value[0] != '/')
        parse_error("location");
    this->location_path = value;
}

void    Location::setCgi(std::string & value)
{
    value = skip(value, "cgi");
    if (value != "on" && value != "off")
        parse_error("cgi");
    this->cgi = value;
}

const std::string&    Location::getCgi() const { return this->cgi; }

void Location::printLocationInfo(){
    std::cout << "LOCATION        (" << this->getLocation() << ") \n";

    for (size_t i = 0; i < this->getMethod().size(); i++)
        std::cout << "METHOD          (" << this->getMethod()[i] << ") \n";
    
    std::cout << "ROOT            (" << this->getRoot() << ") \n";
    std::cout << "UPLOAD          (" << this->getUpload() << ")\n";
    std::cout << "AUTOINDEX       (" << this->getAutoindex() << ") \n";
    std::cout << "CGI             (" << this->getCgi() << ") \n";
    std::cout << "INDEX           (" << this->getIndex() << ") \n";
    std::cout << "REDIRECTION     (" << this->getRedirection() << ") \n";
}