#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../webserv.hpp"
#include "../request/Request.hpp"

class Response
{
    private:
		ConfigServer	server;
        Request			request;
		std::string		responseContent;
		short			statusCode;
		std::string		body;
		Location		location;
		std::string		method;
		std::string		fullPath;
		int				fd;
		int				readBytes;
		bool			readStatus;
		bool 			connectionStatus;
		bool			isExecuted;
		pid_t			pid;
		int				status;

    public:
		static MimeTypes mime_type;
        Response();
        ~Response();

        void	clear();

		// ----------------------------- Getters -----------------------------------

		const ConfigServer&	getConfigServer() const;
		const Request&		getRequest() const;
		const std::string&	getResponseContent() const;
		short				getStatusCode() const;
		const std::string&	getBody() const;
		const Location&		getLocation() const;
		const std::string&	getMethod() const;
		const std::string&	getFullPath() const;
		int					getFd() const;
		int					getReadBytes() const;
		bool				getReadStatus() const;
		bool				getConnectionStatus() const;
		bool				getIsExecuted() const;
		pid_t				getPid() const;
		int					getStatus() const;

        // ----------------------------- Setters -----------------------------------

		void	setConfigServer(const ConfigServer&);
		void	setRequest(const Request&);
		void	setResponseContent(const std::string&);
		void	setStatusCode(const int);
		void	setBody(const std::string&);
		void	setLocation(const Location&);
		void	setMethod(const std::string&);
		void	setFullPath(const std::string&);
		void	setFd(const int);
		void	setReadBytes(const int);
		void	setReadStatus(const bool);
		void	setConnectionStatus(const bool);
		void	setIsExecuted(const bool);
		void	setPid(const pid_t);
		void	setStatus(const int);

        // ----------------------------- Methodes -----------------------------------

		void	buildResponse();
		void	buildResponseContent();
		void	isLocationMatched();
		void	isRedirectionExist();
		void	isMethodAllowed();
		void	isResourceExist();

		// Get
		void	handleGetMethod();
		void	handleGetDirectory();
		void	handleGetFile();
		void	handleGetAutoindex();

		// Post
		void	handlePostMethod();
		void	handlePostFile();

		// Delete
		void	handleDeleteMethod();
		void	handleDeleteDirectoryContent();
		void	handleDeleteDirectory();
		void	handleDeleteFile();

		// CGI
		void	handleCGI();
		void	handleCGIStatusCode();
};

#endif
