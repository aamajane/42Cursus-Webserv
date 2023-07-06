#include "../Response.hpp"

void	Response::handlePostMethod()
{
	try
	{
		// check if the location supports upload
		if (!this->location.getUpload().empty())
		{
			// check if the upload path exists
			DIR *dir = opendir(this->location.getUpload().c_str());
			if (!dir)
			{
				this->statusCode = 404;
				throw std::exception();
			}

			if (closedir(dir) == -1)
			{
				this->statusCode = 500;
				throw std::exception();
			}

			// get the file size
			struct stat st;
			const char *filename = this->request.getNameFileBody().c_str();
			stat(filename, &st);
			off_t size = st.st_size;

			if (size == 0)
			{
				this->statusCode = 411;
				throw std::exception();
			}

			// check if the file size is bigger than the max body size
			if ((unsigned long)size > this->server.getClientMaxBodySize())
			{
				this->statusCode = 413;
				throw std::exception();
			}

			// check if the location has CGI
			if (this->location.getCgi() == "on")
			{
				this->handleCGI();
			}
			else
			{
				this->handlePostFile();
			}
		}
		else
		{
			this->statusCode = 403;
			throw std::exception();
		}
	}
	catch (const std::exception& e)
	{
		throw;
	}
}

void	Response::handlePostFile()
{
	std::string path_to_upload_file = this->location.getUpload();

	// check if the file type is supported
	if (mime_type.getExeFile(skipWhitespaceBeginAnd(this->request.getHeader("Content-Type"))).empty())
	{
		this->statusCode = 415;
		throw std::exception();
	}

	// get the start position of the file name
	std::string::size_type pos =  this->request.getNameFileBody().rfind("/") + 1;

	// join the file name with the upload path
	path_to_upload_file += this->request.getNameFileBody().substr(pos, this->request.getNameFileBody().length());

	// copy the file to the upload path
	std::ifstream in(this->request.getNameFileBody(), std::ios::in | std::ios::binary);
	std::ofstream out(path_to_upload_file, std::ios::out | std::ios::binary);

	out << in.rdbuf();

	out.close();
	in.close();

	this->statusCode = 201;
	throw std::exception();
}
