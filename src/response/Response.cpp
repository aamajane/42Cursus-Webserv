#include "Response.hpp"

MimeTypes Response::mime_type;

Response::Response() :
	responseContent(""),
	statusCode(0),
	body(""),
	method(""),
	fullPath(""),
	fd(0),
	readBytes(0),
	readStatus(false),
	connectionStatus(false),
	isExecuted(false),
	pid(0),
	status(0)
{
}

Response::~Response()
{
    this->clear();
}

void	Response::clear()
{
    this->server.clear();
    this->request.clear();
	this->responseContent.clear();
	this->statusCode = 0;
	this->body.clear();
	this->location.clear();
	this->method.clear();
	this->fullPath.clear();
	if (this->fd > 0)
		close(this->fd);
	this->fd = 0;
	this->readBytes = 0;
	this->readStatus = false;
	this->connectionStatus = false;
	this->isExecuted = false;
	this->pid = 0;
	this->status = 0;
}

// ----------------------------- Getters -----------------------------------

const ConfigServer&	Response::getConfigServer() const { return this->server; }

const Request&		Response::getRequest() const { return this->request; }

const std::string&	Response::getResponseContent() const { return this->responseContent; }

short				Response::getStatusCode() const { return this->statusCode; }

const std::string&	Response::getBody() const { return this->body; }

const Location&		Response::getLocation() const { return this->location; }

const std::string&	Response::getMethod() const { return this->method; }

const std::string&	Response::getFullPath() const { return this->fullPath; }

int					Response::getFd() const { return this->fd; }

int					Response::getReadBytes() const { return this->readBytes; }

bool				Response::getReadStatus() const { return this->readStatus; }

bool				Response::getConnectionStatus() const { return this->connectionStatus; }

bool				Response::getIsExecuted() const { return this->isExecuted; }

pid_t				Response::getPid() const { return this->pid; }

int					Response::getStatus() const { return this->status; }

// ----------------------------- Setters -----------------------------------

void	Response::setConfigServer(const ConfigServer& server) { this->server = server; }

void	Response::setRequest(const Request& request) { this->request = request; }

void	Response::setResponseContent(const std::string& responseContent) { this->responseContent = responseContent; }

void	Response::setStatusCode(const int statusCode) { this->statusCode = statusCode; }

void	Response::setBody(const std::string& body) { this->body = body; }

void	Response::setLocation(const Location& location) { this->location = location; }

void	Response::setMethod(const std::string& method) { this->method = method; }

void	Response::setFullPath(const std::string& fullPath) { this->fullPath = fullPath; }

void	Response::setFd(const int fd) { this->fd = fd; }

void	Response::setReadBytes(const int readBytes) { this->readBytes = readBytes; }

void	Response::setReadStatus(const bool readStatus) { this->readStatus = readStatus; }

void	Response::setConnectionStatus(const bool connectionStatus) { this->connectionStatus = connectionStatus; }

void	Response::setIsExecuted(const bool isExecuted) { this->isExecuted = isExecuted; }

void	Response::setPid(const pid_t pid) { this->pid = pid; }

void	Response::setStatus(const int status) { this->status = status; }

// ----------------------------- Methodes -----------------------------------

void	Response::buildResponse()
{
	try
	{
		// check if the requested file is still being read from the previous loop
		if (this->statusCode == 200)
		{
			this->handleGetFile();
			return;
		}

		// check if the request was parsed successfully
		if (this->request.getCodeError())
		{
			this->statusCode = this->request.getCodeError();
			this->request.setCodeError(0);
			throw std::exception();
		}

		// check if the server has a location that matches the requested location
		this->isLocationMatched();

		// check if the location has a redirection
		this->isRedirectionExist();

		// check if the requested method is allowed in the matched location
		this->isMethodAllowed();

		// check if the requested resource exists
		this->isResourceExist();

		// handle the request based on the method type (GET, POST, DELETE)
		if (this->method == "GET")
		{
			this->handleGetMethod();
		}  
		else if (this->method == "POST")
		{
			this->handlePostMethod();
		}
		else if (this->method == "DELETE")
		{
			this->handleDeleteMethod();
		}
	}
	catch(const std::exception& e)
	{
		this->connectionStatus = true;

		if (this->statusCode == 301)
		{
			this->responseContent = getResponsePage(this->statusCode, false, "");
			this->responseContent.append("Location: ");
			this->responseContent.append(this->fullPath);
			this->responseContent.append("\r\n");
		}
		else
		{
			std::string errorPage = this->server.getErrorPages().find(this->statusCode)->second;
			this->responseContent = getResponsePage(this->statusCode, true, errorPage);
		}
	}
}

void	Response::buildResponseContent()
{
	this->responseContent = getResponsePage(this->statusCode, false, "");
	this->responseContent.append("Content-Type: ");
	this->responseContent.append(getContentType(this->fullPath));
	this->responseContent.append("\r\n");
	this->responseContent.append("Content-Length: ");

	if (!this->body.empty())
	{
		this->responseContent.append(std::to_string(this->body.length()));
		this->responseContent.append("\r\n\r\n");
		this->responseContent.append(this->body);
	}
	else
	{
		struct stat st;
		const char *filename = this->fullPath.c_str();
		stat(filename, &st);
		off_t size = st.st_size;

		this->responseContent.append(std::to_string(size));
		this->responseContent.append("\r\n\r\n");
	}
}

void	Response::isLocationMatched()
{
	std::vector<Location >	locations = this->server.getLocationList();
	std::string				requestedLocation = this->request.getPath();
	bool					isMatched = false;

	// if the URI has multiple slashes, erase the part after the first slash to get the requested location
	// example: /test/test2/test3 -> /test
	size_t pos = requestedLocation.find_first_of('/', 1);
	if (pos != std::string::npos)
	{
		requestedLocation.erase(pos);
	}

	// check if the requested location is matched with a location in the server
	for (std::vector<Location >::iterator it = locations.begin(); it != locations.end(); it++)
	{
		// if the requested location is matched, set it to the response location
		if (it->getLocation() == requestedLocation)
		{
			isMatched = true;
			this->location = *it;

			// check if the requested location ends with a slash
			if (pos == std::string::npos && this->location.getLocation() != "/")
			{
				this->statusCode = 301;
				this->fullPath = this->location.getLocation();
				this->fullPath.append("/");
				throw std::exception();
			}

			break;
		}
	}

	if (!isMatched)
	{
		// if no location is matched check if the root location '/' is matched
		for (std::vector<Location >::iterator it = locations.begin(); it != locations.end(); it++)
		{
			// if the root location '/' is matched, set it to the response location
			if (it->getLocation() == "/")
			{
				isMatched = true;
				this->location = *it;

				break;
			}
		}
	}

	// if the requested location is not matched, set the status code to 404
	if (!isMatched)
	{
		this->statusCode = 404;
		throw std::exception();
	}
}

void	Response::isRedirectionExist()
{
	// check if the location has a redirection
	if (!location.getRedirection().empty())
	{
		// if it does, redirect the client to the redirection path
		this->statusCode = 301;
		this->fullPath = location.getRedirection();
		throw std::exception();
	}
}

void	Response::isMethodAllowed()
{
	std::vector<std::string>	allowedMethods = this->location.getMethod();
	std::string					requestedMethod = this->request.getMethodsString();
	bool						isAllowed = false;

	// check if the requested method is allowed
	for (std::vector<std::string>::iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++)
	{
		// if the requested method is allowed, set it to the response method
		if (*it == requestedMethod)
		{
			isAllowed = true;
			this->method = *it;
		}
	}

	// if the requested method is not allowed, set the status code to 405
	if (!isAllowed)
	{
		this->statusCode = 405;
		throw std::exception();
	}
}

void	Response::isResourceExist()
{
	// set the full path to the requested path and replace the location path with the root path
	this->fullPath = this->request.getPath();
	this->fullPath.replace(0, this->location.getLocation().length(), this->location.getRoot());

	// check if the requested resource exist
	if (access(this->fullPath.c_str(), F_OK) == -1)
	{
		this->statusCode = 404;
		throw std::exception();
	}
}
