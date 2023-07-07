#include "ServerParser.hpp"

ServerParser::ServerParser()
	: port(), host() , serverName(), errorPages() , clientMaxBodySize() , locations()
{
}

ServerParser::ServerParser(const ServerParser& other)
{
	*this = other;
}

ServerParser& ServerParser::operator=(const ServerParser& other)
{
	if (this != &other)
	{
		this->port = other.port;
		this->host = other.host;
		this->serverName = other.serverName;
		this->errorPages = other.errorPages;
		this->clientMaxBodySize = other.clientMaxBodySize;
		this->locations = other.locations;
	}
	return *this;
}

ServerParser::~ServerParser()
{
	this->clear();
}

void	ServerParser::clear()
{
	this->port.clear();
	this->host.clear();
	this->serverName.clear();
	this->errorPages.clear();
	this->clientMaxBodySize = 0;
	this->locations.clear();
}

// ----------------------------- Getters -----------------------------------

std::string							ServerParser::getPort() const { return this->port; }

const std::string&					ServerParser::getHost() const { return this->host; }

const std::string&					ServerParser::getServerName() const { return this->serverName; }

const std::map<short, std::string>&	ServerParser::getErrorPages() const { return this->errorPages; }

unsigned long						ServerParser::getClientMaxBodySize() const { return this->clientMaxBodySize; }

std::vector<Location>				ServerParser::getLocations() const { return this->locations; }

// ----------------------------- Setters -----------------------------------

void	ServerParser::setPort(std::string& value)
{
	value = skip(value, "port");

	for (size_t i = 0; i < value.length(); i++)
	{
		if (!std::isdigit(value[i]))
			parseError("port");
	}

	this->port = value;
}

void	ServerParser::setHost(std::string& value)
{
	value = skip(value, "host");

	std::regex ip_regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

	if (!std::regex_match(value, ip_regex))
		parseError("host");

	this->host = value;
}

void	ServerParser::setServerName(std::string& value)
{
	value = skip(value, "server_name");
	this->serverName = value;
}

void	ServerParser::setErrorPages(std::string& value)
{
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
		parseError("error page");
	}

	value = skipWhitespaceBeginEnd(value);

	if (codeStatus.empty() || value.empty())
		parseError("page_error");

	for (size_t i = 0; i < codeStatus.length(); i++)
	{
		if (!isdigit(codeStatus[i]))
			parseError("(error page) code status");
	}

	short code = std::stoi(codeStatus);
	if (!isValidHTTPStatusCode(code))
		parseError("(error page) code status");

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
		parseError("(error page) code status"); 
	}

	int fd = open(value.c_str(), O_RDONLY);

	if (fd < 0)
		parseError("error page");

	close(fd);

	this->errorPages[code] = value;
}

void	ServerParser::setClientMaxBodySize(std::string& value)
{
	value = skip(value, "client_max_body_size");

	for (size_t i = 0; i < value.size(); i++)
	{
		if (!std::isdigit(value[i]))
			parseError("client max body size");
	}

	try
	{
		this->clientMaxBodySize = std::stoul(value);
	}
	catch(const std::exception)
	{
		parseError("client max body size");
	}
}

void	ServerParser::setLocations(Location& location)
{
	this->locations.push_back(location);
}

// ----------------------------- Methodes -----------------------------------

std::vector<ServerParser>	ServerParser::parseServer(std::string& filename)
{
	ServerParser				s;
	Location					loc;
	std::string 				key;
	std::string					value;
	std::vector<ServerParser>	vecServers;
	std::string					line;
	std::ifstream				infile(filename);

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
				else if (key == "location" && !value.empty() && line[line.length() - 1] == '[')
				{
					loc = Location();
					loc.setLocationPath(value);
					while (std::getline(infile, line))
					{
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
								parseError("config file");
							s.setLocations(loc);
							break;
						}
						else if (!key.empty())
							parseError("config file");
					}
					key.clear();
					value.clear();
				}
				else if (!key.empty())
					parseError("config file");
			}
		}
		else if (!key.empty())
			parseError("config file");
	}

	checkServersAndLocations(vecServers);

	return vecServers;
}

void	ServerParser::checkServersAndLocations(std::vector<ServerParser>& vecServers)
{
	for (size_t i = 0; i < vecServers.size(); i++)
	{
		if (vecServers[i].getPort().empty() || vecServers[i].getHost().empty() || vecServers[i].getServerName().empty())
			parseError("config file");
		if (vecServers[i].getLocations().empty())
			parseError("config file");
		for (size_t j = 0; j < vecServers[i].getLocations().size(); j++)
		{
			if (vecServers[i].getLocations()[j].getRoot().empty())
				parseError("config file");
		}
	}
}

void	ServerParser::printServerInfo()
{
	std::cout << "HOST (" << this->getHost() << ") \n";
	std::cout << "PORT (" << this->getPort() << ") \n";
	std::cout << "SERVER NAME (" << this->getServerName() << ")\n";
	if (this->getClientMaxBodySize() == 0)
		std::cout << "THE CLIENT MAX BODY SIZE (" << this->getClientMaxBodySize() << ") \n";
	std::cout << "\n\n";
	for (size_t i = 0; i < this->getLocations().size(); i++)
	{
		std::cout << "|||||||||||||||| START PRINTING LOCATION NUMBER [" << i + 1 <<  "] |||||||||||||||||||" << std::endl;
		this->getLocations()[i].printLocationInfo();
		std::cout << "|||||||||||||||| END PRINTING LOCATION NUMBER [" << i + 1 <<  "] |||||||||||||||||||||" << std::endl;
	}
	std::cout << "\n";   
}
