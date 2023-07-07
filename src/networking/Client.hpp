#pragma once

#include "../webserv.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

class Client
{
	private:
		int		clientSocket;
		time_t	lastMsgTime;

	public:
		ConfigServer	server;
		Request			request;
		Response		response;

		Client();
		Client(const ConfigServer&);
		~Client();

		void	clear();

		// ----------------------------- Getters -----------------------------------

		int		getClientSocket() const;
		time_t	getLastMsgTime() const;

		// ----------------------------- Setters -----------------------------------

		void	setClientSocket(int);
		void	setLastMsgTime(time_t);

		// ----------------------------- Methodes -----------------------------------

		void	buildResponse();
		void	updateTime();
};
