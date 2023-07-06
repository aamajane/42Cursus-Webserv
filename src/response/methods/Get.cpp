#include "../Response.hpp"

void	Response::handleGetMethod()
{
	try
	{
		// check if the requested resource is a directory or a file
		if (isDirectory(this->fullPath))
		{
			this->handleGetDirectory();
		}
		else
		{
			// check if the requested file has read permission
			if (access(this->fullPath.c_str(), R_OK) == -1)
			{
				this->statusCode = 403;
				throw std::exception();
			}

			// check if the location has CGI
			if (this->location.getCgi() == "on")
			{
				this->handleCGI();
			}
			else
			{
				this->handleGetFile();
			}
		}
	}
	catch (const std::exception& e)
	{
		throw;
	}
}

void	Response::handleGetDirectory()
{
	// check if the directory path ends with a slash
	if (this->fullPath.at(this->fullPath.length() - 1) != '/')
	{
		// if it doesn't, redirect the client to the same requested path with a slash at the end
		this->statusCode = 301;
		this->fullPath.replace(0, this->location.getRoot().length(), this->location.getLocation());
		this->fullPath.append("/");
		throw std::exception();
	}

	// open the directory and check if it's open
	DIR* dir = opendir(this->fullPath.c_str());
	if (dir == NULL)
	{
		this->statusCode = 500;
		throw std::exception();
	}

	struct dirent* ent;
	bool hasIndex = false;

	// loop through the directory content and check if it has the index file
	while ((ent = readdir(dir)) != NULL)
	{
		// if the directory has the index file, append it to the full path
		std::string fileName = ent->d_name;
		if (fileName == this->location.getIndex())
		{
			this->fullPath.append(fileName);
			hasIndex = true;
			break;
		}
	}

	if (closedir(dir) == -1)
	{
		this->statusCode = 500;
		throw std::exception();
	}

	try
	{
		// if the directory has the index file, handle it as a file, otherwise check if it has autoindex
		if (hasIndex)
		{
			// check if the requested file has read permission
			if (access(this->fullPath.c_str(), R_OK) == -1)
			{
				this->statusCode = 403;
				throw std::exception();
			}

			// check if the location has CGI
			if (this->location.getCgi() == "on")
			{
				this->handleCGI();
			}
			else
			{
				this->handleGetFile();
			}
		}
		else
		{
			this->handleGetAutoindex();
		}
	}
	catch (const std::exception& e)
	{
		throw;
	}
}

void	Response::handleGetFile()
{
	char buffer[BUF_SIZE];

	if (!this->readStatus)
	{
		// open the requested file and check if it's open
		if ((this->fd = open(this->fullPath.c_str(), O_RDONLY)) == -1)
		{
			this->statusCode = 500;
			throw std::exception();
		}

		// set the response headers
		this->statusCode = 200;
		this->buildResponseContent();

		// set the read status to true to read the file content
		this->readStatus = true;
	}
	else
	{
		// clear the response content that was sent in the previous loop
		this->responseContent.clear();

		// read the file content by BUF_SIZE bytes
		if ((this->readBytes = read(this->fd, buffer, BUF_SIZE)) == -1)
		{
			this->statusCode = 500;
			throw std::exception();
		}

		if (this->readBytes > 0)
		{
			this->responseContent = std::string(buffer, this->readBytes);
		}
		else
		{
			this->connectionStatus = true;
		}
	}
}

void	Response::handleGetAutoindex()
{
	// check if the directory has autoindex
	if (this->location.getAutoindex() == "on")
	{
		// open the directory and check if it's open
		DIR* dir = opendir(this->fullPath.c_str());
		if (dir == NULL)
		{
			this->statusCode = 500;
			throw std::exception();
		}

		this->body = "<html><head><title>Index of " + this->request.getPath();
		this->body += "</title></head><body><h1>Index of " + this->request.getPath() + "</h1>";
		this->body += "<style>@import url('https://fonts.googleapis.com/css?family=Poppins:400,500,600,700&display=swap');\
						*{box-sizing: border-box;font-family: 'Poppins', Arial, Helvetica, sans-serif;margin: 0;padding: 0;}\
						body{padding: 3%;background: #222222;}\
						table{background: #333333;max-width: 1366px;margin: 0 auto;border:none;border-collapse:collapse;table-layout: fixed;}\
						table th {font-size: 18px;color: #fff;line-height: 1.4;text-transform: uppercase;background-color: #111111;padding-top: 24px;padding-bottom: 20px;padding-left: 20px;text-align: left;font-weight: 600;}\
						table td {padding-top: 18px;padding-bottom: 14px;padding-left: 15px;}\
						table tr:nth-child(even) {background-color: #444444;}\
						h1{text-align: center;padding-bottom: 50px;color: #fff;margin-bottom: 0;}\
						table a{color: #fff;font-size: 16px;text-decoration: none;}\
						table a i{padding-right: 5px;}\
						table tr:nth-child(even) a{color: #fff;}\
						table a:hover, table tr:nth-child(even) a:hover{color: #ffcc66; font-weight: 500;}\
						table a:hover i.fa, table a:hover i.fa:before{background-color: #ffcc66}\
						table a:hover i.fa-file-o, table a:hover i.fa-file-o:before{background-color: #ffffff; border-color: #ffcc66}\
						table tr:hover{background-color: #ffcc662e; cursor: pointer;}\
						i.fa.fa-folder:before {content: '';width: 50%;height: 2px;border-radius: 0 10px 0 0;background-color: #fff;position: absolute;top: -2px;left: 0px;}\
						i.fa.fa-folder {width: 20px;height: 14px;margin: 0 10px -2px 0;position: relative;background-color: #fff;border-radius: 0 3px 3px 3px;display: inline-block;}\
						i.fa.fa-file-o {display: inline-block;width: 15px;height: 20px;background: #ffffff;border-radius: 2px;margin: 0 10px -4px 0;border-top-right-radius: 7px;border:1px solid #fff}\
						</style>";
		this->body += "<table style=\"width:100%\"> <tr> <th>Name</th> <th>Size</th> <th>Last Modified</th> </tr>";

		// loop through the directory content and build the response body
		struct dirent* ent;
		while ((ent = readdir(dir)) != NULL)
		{
			std::string fileName = ent->d_name;
			if (fileName != "." && fileName != "..")
			{
				std::string filePath = this->fullPath + fileName;
				std::string icon;

				if (isDirectory(filePath))
				{
					icon = "<i class=\"fa fa-folder\" aria-hidden=\"true\"></i>";
				}
				else
				{
					icon = "<i class=\"fa fa-file-o\" aria-hidden=\"true\"></i>";
				}

				std::string fileSize = getFileSize(filePath);
				std::string lastModified = getLastModified(filePath);

				this->body += "<tr> <td><a href=\"" + fileName + "\">" + icon + fileName + "</a></td> <td>" + fileSize + "</td> " + lastModified + "</tr>";
			}
		}

		if (closedir(dir) == -1)
		{
			this->statusCode = 500;
			throw std::exception();
		}

		// build the response content
		this->connectionStatus = true;
		this->statusCode = 200;
		this->fullPath.append("index.html");
		this->buildResponseContent();
	}
	else
	{
		this->statusCode = 403;
		throw std::exception();
	}
}
