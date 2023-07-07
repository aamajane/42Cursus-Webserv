#pragma once

#include "../webserv.hpp"

class Location;

class ServerParser
{
	private:
		std::string						port;
		std::string						host;
		std::string						serverName;
		std::map<short, std::string>	errorPages;
		unsigned long					clientMaxBodySize;
		std::vector<Location>			locations;

	public:
		ServerParser();
		ServerParser(const ServerParser& other);
		ServerParser& operator=(const ServerParser& other);
		~ServerParser();

		void	clear();

		// ----------------------------- Getters -----------------------------------

		std::string							getPort() const;
		const std::string&					getHost() const;
		const std::string&					getServerName() const;
		const std::map<short, std::string>&	getErrorPages() const;
		unsigned long						getClientMaxBodySize() const;
		std::vector<Location>				getLocations() const;

		// ----------------------------- Setters -----------------------------------

		void	setPort(std::string&);
		void	setHost(std::string&);
		void	setServerName(std::string&);
		void	setErrorPages(std::string&);
		void	setClientMaxBodySize(std::string&);
		void	setLocations(Location&);

		// ----------------------------- Methodes -----------------------------------

		std::vector<ServerParser>	parseServer(std::string&);
		void						checkServersAndLocations(std::vector<ServerParser>&);
		void						printServerInfo();
};
