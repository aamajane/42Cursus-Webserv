#pragma once

# define CLIENT_MAX_BODY_SIZE 3000000

# define MAX_URI_LENGTH 2048

# define TIMEOUT_CONNECTION 60

# define MSG_BUF 40000
# define BUF_SIZE 1024

# include <iostream>
# include <fstream>
# include <fcntl.h>
# include <cstring>
# include <string> 
# include <unistd.h>
# include <dirent.h>
# include <sstream>

# include <cstdlib>
# include <fstream>
# include <sstream>
# include <cctype>
# include <ctime>
# include <cstdarg>

/* STL Containers */
# include <map>
# include <vector>
# include <algorithm>

/* System */
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <unistd.h>
# include <signal.h>

/* Network */
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

# include <regex>
# include <unordered_set>

# include "response/MimeTypes.hpp"

# include "configuration/Location.hpp"
# include "configuration/ServerParser.hpp"
# include "configuration/ConfigServer.hpp"

// UTILS FUNCTIONS

std::string		skipWhitespaceBeginAnd(std::string);
std::string		skip(std::string, std::string);
bool			errorPath(std::string);
bool			checkUriCharacters(u_int8_t);
bool			checkIsToken(u_int8_t);
std::string		statusCodeString(short);
std::string		getPageError(short);
std::string		getDateFormat();
std::string		getResponsePage(short, bool, std::string);
bool			errorCharQuery(char);
bool			checkStringIsEmpty(std::string);
std::string		generateRandomFileName();
bool			checkFileExists(const std::string);
bool			isDirectory(const std::string);
std::string		getContentType(const std::string);
void			parse_error(const std::string);
bool			isValidHTTPStatusCode(short);
bool			hasPercentEncoded(char*, int, int);
char			decodePercentEncodedChar(const std::string);
std::string		getFileSize(const std::string);
std::string		getLastModified(const std::string);
