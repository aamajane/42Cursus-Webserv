# include "ManageServers.hpp"

ManageServers::ManageServers()
    : Servers(), serversMap(), readFd(), writeFd(), biggestFd(), clientsMap()
{ }
        
ManageServers::~ManageServers()
{
    for (size_t i = 0; i < this->Servers.size(); i++)
        close(this->Servers[i].getFd());
    this->clear();
}

void ManageServers::clear()
{
    if (!this->Servers.empty())
    {
        for (size_t i = 0; i < Servers.size(); i++)
            close(Servers[i].getFd());
        this->Servers.clear();
    }
    if (!this->serversMap.empty())
    {
        for (std::map<int, ConfigServer>::iterator it = this->serversMap.begin(); it != this->serversMap.end(); ++it)
            close (it->second.getFd());
        this->serversMap.clear();
    }

    if (!this->clientsMap.empty()) 
        this->clientsMap.clear();
    this->biggestFd = 0;
}

//! ----------------------------- getters -----------------------------------

const std::vector<ConfigServer>&    ManageServers::getServers() const { return this->Servers; }

const std::map<int, ConfigServer>&  ManageServers::getServersMap() const { return this->serversMap; }

fd_set                              ManageServers::getreadFd() const { return this->readFd; }

fd_set                              ManageServers::getWriteFd() const { return this->writeFd; }

const std::map<int, Client>&        ManageServers::getClient() const { return this->clientsMap; }

//! ----------------------------- setters -----------------------------------

void                            ManageServers::setServers(std::vector<ConfigServer> servers) { this->Servers = servers; }

void                            ManageServers::setServersMap(std::map<int, ConfigServer> ServersMap) { this->serversMap = ServersMap; }

void                            ManageServers::setreadFd(fd_set Recv) { this->readFd = Recv; }

void                            ManageServers::setWriteFd(fd_set Write) { this->writeFd = Write; }

void                            ManageServers::setClient(std::map<int, Client> Clients) { this->clientsMap = Clients; }

//! Methods ----------------------------------------------------------------


void                            ManageServers::acceptClientConnection(int fd)
{
    struct  sockaddr_in         clientAddress;
    long                        clientAddressSize = sizeof(clientAddress);
    int                         clientSock;
    Client                      client;

    // ! Accept the connection
    clientSock = accept(fd, (struct sockaddr *)&clientAddress, (socklen_t*)&clientAddressSize);
    if (clientSock == -1)
    {
        std::cerr << "Failed to accept " << std::endl;
        // 406
        return;
    }
    // ! Adding the client Socket to the readable list of Sets
    this->addToSet(clientSock, this->readFd);
    
    // ! Here like before the fcntl function will help us to be able to read and write from the socket
    if (fcntl(clientSock, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "webserver: fcntl error [" << strerror(errno) << "]" << std::endl;
        removeFromSet(clientSock, this->readFd);
        close(clientSock);
        return ;
    }

    client.setClientSocket(clientSock);
    if (this->clientsMap.count(clientSock) != 0)
        this->clientsMap.erase(clientSock);
    this->clientsMap.insert(std::make_pair(clientSock, client));
}

void                            ManageServers::readRequest(const int & i, Client & client)
{
    char    buf[MSG_BUF];
    int     r_recv = 0;

    r_recv = recv(i, buf, MSG_BUF, 0);
    if (r_recv == 0)
    {
        std::cerr << "webserv: Client " << i << " Closed Connection" << std::endl;
        this->closeConnectionClient(i);
        return ;
    }
    else if (r_recv < 0)
    {
        std::cerr << "webserv: Fd " << i << " read error " << strerror(errno) << std::endl;
        this->closeConnectionClient(i);
        return ;
    }
    else if (r_recv != 0)
    {
        client.updateTime();
        client.request.readBufferFromReq(buf, r_recv);
        memset(buf, 0, sizeof(buf));
    }
    if (client.request.getState() == Parsing_Done || client.request.getCodeError())
    {
        // ! Here if the the parsing is done successfully
        // ! We should assign the Server to the client
        this->assignServerToClient(client);

        // ! Remove fd from recv fd+
        this->removeFromSet(i, this->readFd);

        // ! And add it to the write fd to send response
        this->addToSet(i, this->writeFd);
    }
}

void                            ManageServers::assignServerToClient(Client & client)
{
    client.server.clear();
    for (std::vector<ConfigServer>::iterator it = this->Servers.begin(); it != this->Servers.end(); ++it)
    {
        if ( \
            (client.request.getHost() == it->getHost() || \
            (client.request.getHost() == "localhost" && it->getHost() == "127.0.0.1" )
        ) && client.request.getPort() == it->getPort())
        {
            client.server = *it;
            return ;
        }
    }
}

void                            ManageServers::initSets()
{
    FD_ZERO(&this->readFd);
    FD_ZERO(&this->writeFd);

    for(std::vector<ConfigServer>::iterator it = this->Servers.begin(); it != this->Servers.end(); ++it)
    {
        // ! Here the listen function is responsible for setting the fd to be accepting incoming connections
        // ! The maximum number of connections that can be set for macOS in the listen function is 128
        if (listen(it->getFd(), 128) == -1)
        {
            std::cerr << "webserv: listen error: [" << strerror(errno) << "] Closing" << std::endl;
            exit(EXIT_FAILURE);
        }
        // ! Fcntl functionality is used for the fd socket fd to able to be readable and writable
        if (fcntl(it->getFd(), F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << "webserv: fcntl error: [" << strerror(errno) << "] Closing" << std::endl;
            exit(EXIT_FAILURE);
        }
        // ! add the socket fd to the read file descriptor
        addToSet(it->getFd(), this->readFd);
        this->serversMap.insert(std::make_pair(it->getFd(), *it));
    }
    // ! Gitting the max of file discriptor
    this->biggestFd = this->Servers.back().getFd();
}

void                            ManageServers::setupServers(const std::vector<ConfigServer>& servers)
{
    bool            checkDoubleServers;

    this->Servers = servers;
    // ! If there is two servers with the same port and the same host.
    // ! The program will use the first fd socket for the both theme
    for (std::vector<ConfigServer>::iterator it = this->Servers.begin(); it != this->Servers.end(); ++it)
    {
        checkDoubleServers = false;
        for (std::vector<ConfigServer>::iterator it2 = this->Servers.begin(); it2 != it; ++it2)
        {
            if (it2->getHost() == it->getHost() && it2->getPort() == it->getPort())
            {
                it->setFd(it2->getFd());
                checkDoubleServers = true;
            }
        }
        if (!checkDoubleServers)
            it->setupServer();
    }
}

void                            ManageServers::timeoutCheck()
{
    for(std::map<int, Client>::iterator it = this->clientsMap.begin() ; it != this->clientsMap.end(); ++it)
    {
        if (time(NULL) - it->second.getLastMsgTime() > TIMEOUT_CONNECTION)
        {
            std::cerr << "Client [" << it->first << "] Timeout Connection, Closing ..." << std::endl;
            this->closeConnectionClient(it->first);
            return ;
        }
    }
}

void                            ManageServers::closeConnectionClient(const int i)
{
    if (FD_ISSET(i, &this->writeFd))
        this->removeFromSet(i, this->writeFd);
    if (FD_ISSET(i, &this->readFd))
        this->removeFromSet(i, this->readFd);
    close(i);
    this->clientsMap.erase(i);
}

void                            ManageServers::removeFromSet(const int i, fd_set & set)
{
    FD_CLR(i, &set);
    if (i == this->biggestFd)
        this->biggestFd--;
}

void                            ManageServers::addToSet(const int i, fd_set & set)
{
    FD_SET(i, &set);
    if (i > this->biggestFd)
        biggestFd = i;
}

void                            ManageServers::startServers()
{
    fd_set  readCpy;
    fd_set  writeCpy;
    int     r_select;

    this->biggestFd = 0;
    // ! initialized the sets for all the fd's
    this->initSets();
    struct timeval timer;

    std::cout << "--------------------------- STARTING --------------------------------" << std::endl;
    
    while (true)
    {
        timer.tv_sec = 1;
        timer.tv_usec = 0;
        readCpy = this->readFd;
        writeCpy = this->writeFd;

        // ! Here select will help us with the fd's that are ready to be read and write
        // ! The last parameter is for the how much the select will wait for every fd
        if ( (r_select = select(this->biggestFd + 1, &readCpy, &writeCpy, NULL, &timer)) < 0 )
        {
            std::cerr << "Webserv error :: select [" << strerror(errno) << "] Closing !" << std::endl;
            exit(1);
        }
        for (int i = 0; i <= this->biggestFd; ++i)
        {
            // ! Here checking if the fd is already accepted or not 
            if (FD_ISSET(i, &readCpy) && this->serversMap.count(i))
                this->acceptClientConnection(i);
            else if (FD_ISSET(i, &readCpy) && this->clientsMap.count(i))
            {
                // ! Here start reading the request client
                this->readRequest(i, this->clientsMap[i]);

                // this->clientsMap[i].request.printRequest();
            }
            else if (FD_ISSET(i, &writeCpy))
            {
                // ! Here building the response
                this->clientsMap[i].buildResponse();

                // ! Here sending the response
                this->sendResponse(i, this->clientsMap[i]);
            }
        }
        // ! Here we check the time if the client write a request with more than one minute 
        // ! The connection will be closed 
        // [Info] One minute is used by nginx you can update the time out the webserv header 
        this->timeoutCheck();
    }
}

void                            ManageServers::sendResponse(const int & i, Client & client)
{
    std::string response = client.response.getResponseContent();

    // std::cout << response << std::endl;

    if (send(i, response.c_str(), response.size(), 0) < 0)
    {
        std::cerr << "sendResponse(): error sending : " << strerror(errno) << std::endl;
        this->closeConnectionClient(i);
        return ;
    }

    if (client.response.getConnectionStatus())
    {
        if (client.request.keepAlive() == false || client.response.getStatusCode())
        {
            std::cerr << "Client [" << i << "] Connection Closed" << std::endl;
            this->closeConnectionClient(i);
        }
        else
        {
            this->removeFromSet(i, this->writeFd);
            this->addToSet(i, this->readFd);
            client.clear();
        }
    }
    else
        client.updateTime();
}

std::vector<ConfigServer>       ManageServers::getInfoServer(const std::vector<ServerParser >& servers)
{
    std::vector<ConfigServer> vecServers;
    ConfigServer              server;

    for (size_t i = 0; i < servers.size(); i++)
    {
        server = ConfigServer(servers[i].getPort(), servers[i].getHost(), servers[i].getServerName(), servers[i].get_locations(), servers[i].getErrorPages(), servers[i].getClientMaxBodySize());
        vecServers.push_back(server);
    }

    return vecServers;
}
