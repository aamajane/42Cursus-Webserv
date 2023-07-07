#pragma once

#include "../webserv.hpp"

class Location
{
	private:
		std::string					location_path;
		std::vector<std::string>	method;
		std::string					root;
		std::string					upload;
		std::string					index;
		std::string					autoindex;
		std::string					cgi;
		std::string					redirection;

	public:
		Location();
		Location(const Location& other);
		Location& operator=(const Location& other);
		~Location();

		void	clear();

		// ----------------------------- Getters -----------------------------------

		const std::string&				getLocation() const;
		const std::vector<std::string>&	getMethod() const;
		const std::string&				getRoot() const;
		const std::string&				getUpload() const;
		const std::string&				getIndex() const;
		const std::string&				getAutoindex() const;
		const std::string&				getCgi() const;
		const std::string&				getRedirection() const;

		// ----------------------------- Setters -----------------------------------

		void	setLocation(std::string&);
		void	setMethod(std::string, bool);
		void	setRoot(std::string&);
		void	setUpload(std::string&);
		void	setIndex(std::string&);
		void	setAutoindex(std::string&);
		void	setCgi(std::string&);
		void	setRedirection(std::string&);

		// ----------------------------- Methodes -----------------------------------

		void	printLocationInfo();
};
