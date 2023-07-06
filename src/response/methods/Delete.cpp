#include "../Response.hpp"

void	Response::handleDeleteMethod()
{
	try
	{
		// check if the location has CGI
		if (this->location.getCgi() == "on")
		{
			this->statusCode = 501;
			throw std::exception();
		}

		// check if the requested resource is a directory or a file
		if (isDirectory(this->fullPath))
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

			this->handleDeleteDirectoryContent();
			this->handleDeleteDirectory();
		}
		else
		{
			this->handleDeleteFile();
		}
	}
	catch (const std::exception& e)
	{
		throw;
	}

	// if the requested resource is deleted successfully, set the status code to 204
	this->statusCode = 204;
	throw std::exception();
}

void	Response::handleDeleteDirectoryContent()
{
	// save the original path
	std::string originalPath = this->fullPath;

	// open the directory and check if it's open
	DIR* dir = opendir(this->fullPath.c_str());
	if (dir == NULL)
	{
		this->statusCode = 500;
		throw std::exception();
	}

	try
	{
		struct dirent* ent;

		// loop through the directory content and delete subdirectories and files
		while ((ent = readdir(dir)) != NULL)
		{
			std::string fileName = ent->d_name;
			if (fileName != "." && fileName != "..")
			{
				// check if the entry is a directory or a file
				if (ent->d_type == DT_DIR)
				{
					// append the directory name to the full path and add a slash at the end
					this->fullPath.append(fileName + "/");

					// recursively delete the directory content then delete the directory
					this->handleDeleteDirectoryContent();
					this->handleDeleteDirectory();
				}
				else
				{
					// append the file name to the full path and delete it
					this->fullPath.append(fileName);
					this->handleDeleteFile();
				}
			}

			// restore the original path
			this->fullPath = originalPath;
		}

		if (closedir(dir) == -1)
		{
			this->statusCode = 500;
			throw std::exception();
		}
	}
	catch (const std::exception& e)
	{
		throw;
	}
}

void	Response::handleDeleteDirectory()
{
	// check if the directory has write and read permission
	if (access(this->fullPath.c_str(), W_OK | R_OK) == 0)
	{
		// check if the directory has been deleted successfully
		if (rmdir(this->fullPath.c_str()) != 0)
		{
			this->statusCode = 500;
			throw std::exception();
		}
	}
	else
	{
		this->statusCode = 403;
		throw std::exception();
	}
}

void	Response::handleDeleteFile()
{
	// check if the file has write permission
	if (access(this->fullPath.c_str(), W_OK) == 0)
	{
		// check if the file has been deleted successfully
		if (unlink(this->fullPath.c_str()) != 0)
		{
			this->statusCode = 500;
			throw std::exception();
		}
	}
	else
	{
		this->statusCode = 403;
		throw std::exception();
	}
}
