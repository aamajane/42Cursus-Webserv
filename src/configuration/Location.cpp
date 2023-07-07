#include "Location.hpp"

Location::Location()
	: locationPath(), method(), root(), upload(), index(), autoindex(), cgi(), redirection()
{
}

Location::Location(const Location& other)
{
	*this = other;
}

Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		this->locationPath = other.locationPath;
		this->method = other.method;
		this->root = other.root;
		this->upload = other.upload;
		this->index = other.index;
		this->autoindex = other.autoindex;
		this->cgi = other.cgi;
		this->redirection = other.redirection;
	}
	return *this;
}

Location::~Location()
{
	this->clear();
}

void    Location::clear()
{
	this->locationPath.clear();
	this->method.clear();
	this->root.clear();
	this->upload.clear();
	this->index.clear();
	this->autoindex.clear();
	this->cgi.clear();
	this->redirection.clear();
}

// ----------------------------- Getters -----------------------------------

const std::string&				Location::getLocationPath() const { return this->locationPath; }

const std::vector<std::string>&	Location::getMethod() const { return this->method; }

const std::string&				Location::getRoot() const { return root; }

const std::string&				Location::getUpload() const { return this->upload; }

const std::string&				Location::getIndex() const { return this->index; }

const std::string&				Location::getAutoindex() const {	return this->autoindex; }

const std::string&				Location::getCgi() const { return this->cgi; }

const std::string&				Location::getRedirection() const { return this->redirection; }

// ----------------------------- Setters -----------------------------------


void Location::setLocationPath(std::string& value)
{
	if (value[0] != '/')
		parseError("location");

	this->locationPath = value;
}

void	Location::setMethod(std::string value, bool check)
{
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

		value = skipWhitespaceBeginEnd(value);
	}
	else
		this->method.push_back(value);
}

void Location::setRoot(std::string& value)
{
	value = skip(value, "root");

	if (value[value.length() - 1] != '/')
		parseError("root");

	this->root = value;
}

void Location::setUpload(std::string& value)
{
	value = skip(value, "upload");

	if (value[value.length() - 1] != '/')
		parseError("upload");

	this->upload = value;
}

void Location::setIndex(std::string& value)
{
	value = skip(value, "index");
	this->index = value;
}

void Location::setAutoindex(std::string& value)
{
	value = skip(value, "autoindex");

	if (value != "on" && value != "off")
		parseError("autoindex");

	this->autoindex = value;
}

void    Location::setCgi(std::string & value)
{
	value = skip(value, "cgi");

	if (value != "on" && value != "off")
		parseError("cgi");

	this->cgi = value;
}

void Location::setRedirection(std::string& value)
{
	value = skip(value, "redirection");
	this->redirection = value;
}

// ----------------------------- Methodes -----------------------------------

void	Location::printLocationInfo()
{
	std::cout << "LOCATION        (" << this->getLocationPath() << ") \n";

	for (size_t i = 0; i < this->getMethod().size(); i++)
		std::cout << "METHOD          (" << this->getMethod()[i] << ") \n";
	
	std::cout << "ROOT            (" << this->getRoot() << ") \n";
	std::cout << "UPLOAD          (" << this->getUpload() << ")\n";
	std::cout << "INDEX           (" << this->getIndex() << ") \n";
	std::cout << "AUTOINDEX       (" << this->getAutoindex() << ") \n";
	std::cout << "CGI             (" << this->getCgi() << ") \n";
	std::cout << "REDIRECTION     (" << this->getRedirection() << ") \n";
}
