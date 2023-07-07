#include "Client.hpp"

Client::Client() 
	: clientSocket(), lastMsgTime(time(NULL)), server(), request(), response()
{
}

Client::Client(const ConfigServer& server)
	: clientSocket(), lastMsgTime(time(NULL)), server(), request(), response()
{ 
	this->server = server;
	this->lastMsgTime = time(NULL);
}

Client::~Client()
{
	this->clear();
}

void	Client::clear()
{
	this->server.clear();
	this->request.clear();
	this->response.clear();
}

// ----------------------------- Getters -----------------------------------

int		Client::getClientSocket() const { return this->clientSocket; }

time_t	Client::getLastMsgTime() const { return this->lastMsgTime; }

// ----------------------------- Setters -----------------------------------

void	Client::setClientSocket(int socket) { this->clientSocket = socket; }

void	Client::setLastMsgTime(time_t time) { this->lastMsgTime = time; }

// ----------------------------- Methodes -----------------------------------

void	Client::buildResponse()
{
	this->response.setConfigServer(this->server);
	this->response.setRequest(this->request);
	this->response.buildResponse();
}

void	Client::updateTime()
{
	this->setLastMsgTime(time(NULL));
}
