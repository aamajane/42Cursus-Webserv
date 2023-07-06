#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
    : Port(), Host(), serverName(), errorPages(), clientMaxBodySize(CLIENT_MAX_BODY_SIZE), locationList()
{ }

ConfigServer::ConfigServer(std::string port, std::string host, std::string ServerName, std::vector<Location > _locationList, std::map<short, std::string> errorPages, unsigned long clientMaxBodySize)
    : Port(), Host(), serverName(), errorPages(), clientMaxBodySize(), locationList()
{
    this->setPort(port);
    this->setHost(host);
    this->setServerName(ServerName);
    this->locationList = _locationList;
    this->errorPages = errorPages;
    this->clientMaxBodySize = clientMaxBodySize;
    this->setFd(0);
}

ConfigServer::ConfigServer(const ConfigServer & other) {*this = other; }


ConfigServer & ConfigServer::operator=(const ConfigServer & other)
{
    if (this != &other)
	{
        this->Port = other.Port;
        this->Host = other.Host;
        this->serverName = other.serverName;
        this->clientMaxBodySize = other.clientMaxBodySize;
        this->errorPages = other.errorPages;
        this->Fd = other.Fd;
        this->serverAddress = other.serverAddress;
        this->locationList = other.locationList;
    }
    return *this;
}

ConfigServer::~ConfigServer() { clear(); }

void        ConfigServer::clear()
{
    this->Port = 0;
    this->Host.clear();
    this->serverName.clear();
    this->clientMaxBodySize = 0;
    this->errorPages.clear();
    for (size_t i = 0; i < this->locationList.size(); i++)
        this->locationList[i].clear();
    
}

//! ----------------------------- getters -----------------------------------

uint16_t                            ConfigServer::getPort() const { return this->Port; }

std::string                         ConfigServer::getHost() const { return this->Host; }

std::string                         ConfigServer::getServerName() const { return this->serverName; }

std::map<short, std::string>        ConfigServer::getErrorPages() const { return this->errorPages; }

unsigned long                       ConfigServer::getClientMaxBodySize() const { return this->clientMaxBodySize; }

std::vector<Location >              ConfigServer::getLocationList() const { return this->locationList; }

int                                 ConfigServer::getFd() const { return this->Fd; }

//! ----------------------------- setters -----------------------------------

void                ConfigServer::setPort(std::string port) { this->Port = static_cast<uint16_t>(std::stoul(port)); }

void                ConfigServer::setHost(std::string host) { 
    this->Host = host;
}

void                ConfigServer::setServerName(std::string ServerName) { this->serverName = ServerName; }

void                ConfigServer::setErrorPages(std::map<short, std::string> ErrorPages) { this->errorPages = ErrorPages; }

void                ConfigServer::setClientMaxBodySize(unsigned long size) { 

    if (!size)
        this->clientMaxBodySize = size;
    else
        this->clientMaxBodySize = CLIENT_MAX_BODY_SIZE;
}

void                ConfigServer::setLocationList(std::vector<Location > _locationList) { this->locationList = _locationList; }

void                ConfigServer::setFd(int fd) { this->Fd = fd; }

void                                ConfigServer::setupServer()
{
    // ! Create a fd socket 
    // ! The AF_INET is responsible for IPv4 connection 
    // ! And SOCK_STREAM is responsible for TCP connection
    this->Fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->Fd < 0)
    {
        std::cerr << "PORT [" << this->getPort() << "] SERVER NAME [" << this->getServerName() << "]" << std::endl;
        std::cerr << "webserver: socket error [" << strerror(errno) << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    int option_value = 1;
    
    // ! Here set the address local to reusable
    if (setsockopt(this->Fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) == -1)
    {
        std::cerr << "PORT [" << this->getPort() << "] SERVER NAME [" << this->getServerName() << "]" << std::endl;
        std::cerr << "webserver: setsockopt error [" << strerror(errno) << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&this->serverAddress, 0, sizeof(this->serverAddress));
    
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_addr.s_addr = inet_addr(this->getHost().c_str());
    this->serverAddress.sin_port = htons(this->getPort());
    
    // ! Here bind address with the port 
    if (bind(this->Fd, (struct sockaddr *) &this->serverAddress, sizeof(this->serverAddress)) < 0)
    {
        std::cerr << "PORT [" << this->getPort() << "] SERVER NAME [" << this->getServerName() << "]" << std::endl;
        std::cerr << "webserver: bind error [" << strerror(errno) << "]" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void                ConfigServer::printServerInfo()
{
    std::cout << "HOST (" << this->getHost() << ") \n";
    std::cout << "PORT (" << this->getPort() << ") \n";
    std::cout << "SERVER NAME (" << this->getServerName() << ")\n";
    std::cout << "THE CLIENT MAX BODY SIZE (" << this->getClientMaxBodySize() << ") \n";
    std::cout << "\n\n";
    for (size_t i = 0; i < this->getLocationList().size(); i++)
    {
        std::cout << "|||||||||||||||| START PRINTING LOCATION NUMBER [" << i + 1 <<  "] |||||||||||||||||||" << std::endl;
        this->getLocationList()[i].printLocationInfo();
        std::cout << "|||||||||||||||| END PRINTING LOCATION NUMBER [" << i + 1 <<  "] |||||||||||||||||||||" << std::endl;
    }
    std::cout << "\n";
}