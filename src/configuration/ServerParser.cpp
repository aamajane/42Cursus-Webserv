#include "ServerParser.hpp"

void ServerParser::setPort(std::string& value) {
    value = skip(value, "port");

    for (size_t i = 0; i < value.length(); i++)
    {
        if (!std::isdigit(value[i]))
            parse_error("port");
    }
    port = value;
}

const std::string& ServerParser::getHost() const {
    return host;
}

void ServerParser::setHost(std::string& value) {
    
    value = skip(value, "host");

    std::regex ip_regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    
    if (!std::regex_match(value, ip_regex))
        parse_error("host");

    this->host = value;
}

const std::string& ServerParser::getServerName() const {
    return server_name;
}

void ServerParser::setServerName(std::string& value) {
    value = skip(value, "server_name");

    server_name = value;
}

const std::map<short, std::string>& ServerParser::getErrorPages() const {
    return error_pages;
}

std::string ServerParser::getPort() const { return port; }

void ServerParser::setErrorPages(std::string & value) {

    value = skip(value, "error_page");
    std::string codeStatus;
    
    for (size_t i = 0; i < 3; i++)
    {
        codeStatus += value[i];
    }

    try
    {
        value.erase(0, 3);
    }
    catch(const std::exception& e)
    {
        parse_error("error page");
    }
    
    value = skipWhitespaceBeginAnd(value);

    if (codeStatus.empty() || value.empty())
        parse_error("page_error");

    for (size_t i = 0; i < codeStatus.length(); i++)
    {
        if (!isdigit(codeStatus[i]))
            parse_error("(error page) code status");
    }
    short code = std::stoi(codeStatus);
    if (!isValidHTTPStatusCode(code))
        parse_error("(error page) code status");
    
    try
    {
        std::string::size_type pos = value.find_last_of('.');
        if (pos == std::string::npos)
            throw std::exception();
        std::string result = value.substr(pos, value.length());
        if (result != ".html")
            throw std::exception();
    }
    catch(const std::exception)
    {
        parse_error("(error page) code status"); 
    }

    int fd = open(value.c_str(), O_RDONLY);

    if (fd < 0)
        parse_error("error page");
    close(fd);
    error_pages[code] = value;
}

unsigned long ServerParser::getClientMaxBodySize() const {
    return client_max_body_size;
}

void ServerParser::setClientMaxBodySize(std::string& value) {

    value = skip(value, "client_max_body_size");

    for (size_t i = 0; i < value.size(); i++)
    {
        if (!std::isdigit(value[i]))
            parse_error("client max body size");
    }

    try
    {
        client_max_body_size = std::stoul(value);
    }
    catch(const std::exception)
    {
        parse_error("client max body size");
    }
    
}

std::vector<Location> ServerParser::get_locations() const {
    return _location;
}

void ServerParser::set_locations(Location new_locations) { _location.push_back(new_locations); }

ServerParser::ServerParser(const ServerParser& other)
{
    *this = other;
}

ServerParser& ServerParser::operator=(const ServerParser& other) {
    if (this != &other) {
        port = other.port;
        host = other.host;
        server_name = other.server_name;
        error_pages = other.error_pages;
        client_max_body_size = other.client_max_body_size;
        _location = other._location;
        this->error_pages = other.error_pages;
    }
    return *this;
}

void        ServerParser::clear()
{
    this->port.clear();
    this->host.clear();
    this->server_name.clear();
    this->error_pages.clear();
    this->client_max_body_size = 0;
    this->_location.clear();
}

ServerParser::ServerParser()
    : port(), host() , server_name(), error_pages() , client_max_body_size() , _location()
{

}
ServerParser::~ServerParser(){
    this->clear();
}

std::vector<ServerParser> ServerParser::get_server(std::string filename){
    ServerParser        s;
    Location            loc;

    std::string key , value;
    std::vector<ServerParser>    vecServers;


    std::string line;
    std::ifstream infile(filename);
    if (!infile.is_open()) 
    {
        std::cerr << "webserv  error : could not open config file " << std::endl;
        exit(EXIT_FAILURE);
    }

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        iss >> key >> value;
        if (!key.empty() && key[0] == '#')
        {
            key.clear();
            value.clear();
        }
        else if (key == "server" && value == "{")
        {
            s = ServerParser();
            while (std::getline(infile, line))
            {
                std::istringstream iss_loc(line);
                iss_loc >> key >> value;
                if (!key.empty() && key[0] == '#')
                {
                    key.clear();
                    value.clear();
                }
                else if (key == "port" && !value.empty())
                {
                    s.setPort(line);
                    key.clear();
                    value.clear();
                }
                else if (key == "host" && !value.empty())
                {
                    s.setHost(line);
                    key.clear();
                    value.clear();
                }
                else if (key == "server_name" && !value.empty())
                {
                    s.setServerName(line);
                    key.clear();
                    value.clear();
                }
                else if (key == "error_page" && !value.empty())
                {
                    s.setErrorPages(line);
                    key.clear();
                    value.clear();
                }
                else if (key == "client_max_body_size" && !value.empty())
                {
                    s.setClientMaxBodySize(line);
                    key.clear();
                    value.clear();
                }
                else if (key == "}" && value.empty())
                {
                    vecServers.push_back(s);
                    key.clear();
                    value.clear();
                    break;
                }
                else if (key == "location" && !value.empty() && line[line.length() - 1] == '[') {
                    loc = Location();
                    loc.setLocation(value);
                    while (std::getline(infile, line)) {
                        std::istringstream iss_loc_key_val(line);
                        iss_loc_key_val >> key >> value;
                        if (!key.empty() && key[0] == '#')
                        {
                            key.clear();
                            value.clear();
                        }
                        else if (key == "method" && !value.empty())
                            loc.setMethod(line, true);
                        else if (key == "root" && !value.empty())
                        {
                            loc.setRoot(line);
                            key.clear();
                            value.clear();
                        }
                        else if (key == "cgi" && !value.empty())
                        {
                            loc.setCgi(line);
                            key.clear();
                            value.clear();
                        }
                        else if (key == "upload" && !value.empty())
                        {
                            loc.setUpload(line);
                            key.clear();
                            value.clear();
                        }   
                        else if (key == "autoindex" && !value.empty())
                        {
                            loc.setAutoindex(line);
                            key.clear();
                            value.clear();
                        }   
                        else if (key == "index" && !value.empty())
                        {
                            loc.setIndex(line);
                            key.clear();
                            value.clear();
                        }   
                        else if (key == "redirection" && !value.empty())
                        {
                            loc.setRedirection(line);
                            key.clear();
                            value.clear();
                        }
                        else if (key == "]")
                        {
                            if (loc.getMethod().empty())
                                loc.setMethod("GET", false);
                            if  (loc.getRoot().empty())
                                parse_error("config file");
                            s.set_locations(loc);
                            break;
                        }
                        else if (!key.empty())
                            parse_error("config file");
                    }
                    key.clear();
                    value.clear();
                }
                else if (!key.empty())
                    parse_error("config file");
            }
            
        }
        else if (!key.empty())
            parse_error("config file");
    }
    
    checkServersAndLocations(vecServers);

    return vecServers;
}

void    ServerParser::checkServersAndLocations(std::vector<ServerParser > vecServers)
{
    for (size_t i = 0; i < vecServers.size(); i++){
        if (vecServers[i].getPort().empty() || vecServers[i].getHost().empty() || vecServers[i].getServerName().empty())
            parse_error("config file");
        if (vecServers[i].get_locations().empty())
            parse_error("config file");
        for (size_t j = 0; j < vecServers[i].get_locations().size(); j++){
            if (vecServers[i].get_locations()[j].getRoot().empty())
                parse_error("config file");
        }
    }
}

void    ServerParser::printTheServerInfo()
{
    std::cout << "HOST (" << this->getHost() << ") \n";
    std::cout << "PORT (" << this->getPort() << ") \n";
    std::cout << "SERVER NAME (" << this->getServerName() << ")\n";
    if (this->getClientMaxBodySize() == 0)
        std::cout << "THE CLIENT MAX BODY SIZE (" << this->getClientMaxBodySize() << ") \n";
    std::cout << "\n\n";
    for (size_t i = 0; i < this->get_locations().size(); i++)
    {
        std::cout << "|||||||||||||||| START PRINTING LOCATION NUMBER [" << i + 1 <<  "] |||||||||||||||||||" << std::endl;
        this->get_locations()[i].printLocationInfo();
        std::cout << "|||||||||||||||| END PRINTING LOCATION NUMBER [" << i + 1 <<  "] |||||||||||||||||||||" << std::endl;
    }
    std::cout << "\n";   
}