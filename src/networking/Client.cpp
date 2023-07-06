# include "Client.hpp"

Client::Client() 
    : clientSocket(), lastMsgTime(time(NULL)), request(), server(), response()
{
}

Client::~Client()
{
    this->clear();
}

Client::Client(const ConfigServer& server)
    : clientSocket(), lastMsgTime(time(NULL)), request(), server(), response()
{ 
    this->server = server;
    this->lastMsgTime = time(NULL);
}

void                        Client::buildResponse()
{
    this->response.setConfigServer(this->server);
    this->response.setRequest(this->request);
    this->response.buildResponse();
}

//! ----------------------------- getters -----------------------------------

int                     Client::getClientSocket() const { return this->clientSocket; }

time_t                  Client::getLastMsgTime() const { return this->lastMsgTime; }

//! ----------------------------- setters -----------------------------------

void                    Client::setClientSocket(int socket) { this->clientSocket = socket; }

void                    Client::setLastMsgTime(time_t time) { this->lastMsgTime = time; }

// ? Methodes ----------------------------------------------------------------

void                    Client::updateTime() { this->setLastMsgTime(time(NULL)); }

void                    Client::clear()
{
    this->server.clear();
    this->request.clear();
    this->response.clear();
}
