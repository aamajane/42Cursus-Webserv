#include "webserv.hpp"

std::string skipWhitespaceBeginAnd(std::string line)
{
    line.erase(line.begin(), std::find_if_not(line.begin(), line.end(), isspace));
    line.erase(std::find_if_not(line.rbegin(), line.rend(), isspace).base(), line.end());

    return line;
}

std::string skip(std::string value, std::string removeString)
{
    if (value[value.length() - 1] != ';')
        parse_error(removeString);

    value.erase(value.length() - 1);
    value = skipWhitespaceBeginAnd(value);
    value.erase(0 , removeString.length());
    value = skipWhitespaceBeginAnd(value);

    return value;
}

bool    errorPath(std::string path)
{
    std::string t(path);
    char *r = strtok((char*)t.c_str(), "/");
    int p = 0;

    while (r != NULL)
    {
        if (!strcmp(r, ".."))
            p--;
        else
            p++;
        if (p < 0)
            return true;
        r = strtok(NULL, "/");
    }

    return false;
}

bool    hasPercentEncoded(char * buffer, int startIndex, int bufferSize)
{
    std::string input = "";

    for (int i = startIndex; (i < bufferSize && i < (startIndex + 3) ); i++)
        input += buffer[i];
    
    if (input.length() != 3 ) {
        bool isValidHexDigit1 = isxdigit(static_cast<unsigned char>(input[1]));
        bool isValidHexDigit2 = isxdigit(static_cast<unsigned char>(input[2]));
        if (isValidHexDigit1 && isValidHexDigit2) {
            return true;
        }
    }

    return false;
}

char    decodePercentEncodedChar(const std::string percentEncoded)
{
    std::istringstream hexDigits(percentEncoded);
    int hexValue = 0;

    hexDigits >> std::hex >> hexValue;

    return static_cast<char>(hexValue);
}

void    parse_error(const std::string message)
{
    std::cerr << "[ERROR]: Invalid (" << message << ")" << std::endl;
    exit(EXIT_FAILURE);
}

bool    checkUriCharacters(u_int8_t c)
{
    const std::string forbiddenChars = "<>\"%{}|\\^~[]`();?:@=&$,#";

    return forbiddenChars.find(c) != std::string::npos;
}

bool    errorCharQuery(char c)
{
    const std::string forbiddenChars = " \"<>{}|\\^~[]`();/?:@$,#";

    return forbiddenChars.find(c) != std::string::npos;
}

bool    checkIsToken(u_int8_t c)
{
    if (c == '!' || (c >= '#' && c <= '\'') || c == '*'|| c == '+' || c == '-'  || c == '.' || \
       (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= '^' && c <= '`') || \
       (c >= 'a' && c <= 'z') || c == '|')
        return (true);

    return (false);
}

std::string statusCodeString(short statusCode)
{
    switch (statusCode)
    {
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocol";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 300:
            return "Multiple Choice";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Moved Temporarily";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Requested Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot";
        case 421:
            return "Misdirected Request";
        case 425:
            return "Too Early";
        case 426:
            return "Upgrade Required";
        case 428:
            return "Precondition Required";
        case 429:
            return "Too Many Requests";
        case 431:
            return "Request Header Fields Too Large";
        case 451:
            return "Unavailable for Legal Reasons";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";
        case 506:
            return "Variant Also Negotiates";
        case 507:
            return "Insufficient Storage";
        case 510:
            return "Not Extended";
        case 511:
            return "Network Authentication Required";
        default:
            return "";
    }
}

std::string getPageError(short statusCode)
{
    std::string page = "";
    std::string s_code = std::to_string(statusCode);
    std::string s_code_string = statusCodeString(statusCode);

    page.append("<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Jinx Nginx - Error Page</title>\r\n");
    page.append("<style>\r\n");
    page.append("body {\r\n");
    page.append("  background-color: #1a1a1a;\r\n");
    page.append("  color: #ffffff;\r\n");
    page.append("  font-family: 'Arial', sans-serif;\r\n");
    page.append("  margin: 0;\r\n");
    page.append("  padding: 0;\r\n");
    page.append("}\r\n");
    page.append("h1, h2 {\r\n");
    page.append("  text-align: center;\r\n");
    page.append("}\r\n");
    page.append(".container {\r\n");
    page.append("  max-width: 35em;\r\n");
    page.append("  margin: 0 auto;\r\n");
    page.append("  padding: 2em;\r\n");
    page.append("}\r\n");
    page.append("</style>\r\n");
    page.append("</head>\r\n");
    page.append("<body>\r\n");
    page.append("<div class='container'>\r\n");
    page.append("<h1>Welcome to Jinx Nginx</h1>\r\n");
    page.append("<h2>" + s_code + " - " + s_code_string + "</h2>\r\n");
    page.append("</div>\r\n");
    page.append("</body>\r\n");
    page.append("</html>");

    return page;
}

std::string getDateFormat()
{
	time_t	currentTime = time(NULL);
	tm*		utcTime = gmtime(&currentTime);

	std::string monthNames[] = {
	    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	std::string monthName = monthNames[utcTime->tm_mon];

	std::string dayNames[] = {
	    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	std::string dayName = dayNames[utcTime->tm_wday];

	std::string date = "Date: " + dayName + ", " +
	std::to_string(utcTime->tm_mday) + " " + monthName + " " +
	std::to_string(utcTime->tm_year + 1900) + " " +
	std::to_string(utcTime->tm_hour) + ":" +
	std::to_string(utcTime->tm_min) + ":" +
	std::to_string(utcTime->tm_sec) + " UTC";
    date.append("\r\n");

	return (date);
}

static std::string  getContentFileFromPathFile(std::string path)
{
    std::ifstream   file(path);
    std::string     content;

    if (file.is_open())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();

        file.close();
    }

    return content;
}

std::string getResponsePage(short codeStatus, bool needBody, std::string pathErrorFile)
{
    std::string response;

    response.append("HTTP/1.1 ");
	response.append(std::to_string(codeStatus));
	response.append(" ");
	response.append(statusCodeString(codeStatus));
	response.append("\r\n");

    if (codeStatus == 101)
        response.append("Upgrade: HTTP/1.1\r\nConnection: Upgrade\r\n");

    response.append("Server: Jinx Nginx\r\n");
	response.append(getDateFormat());

    if (needBody)
    {
        std::string body = getContentFileFromPathFile(pathErrorFile);
        if (body.empty())
            body = getPageError(codeStatus);

        response.append("Content-Type: text/html\r\n");
        response.append("Content-Length: ");
        response.append(std::to_string(body.length()));
        response.append("\r\n\r\n");
        response.append(body);
    }

    return response;
}

bool    isValidHTTPStatusCode(short statusCode)
{
    static const int validStatusCodesArr[] = {
        100, 101, 102, 103, 122,
        200, 201, 202, 203, 204, 205, 206, 207, 208, 226,
        300, 301, 302, 303, 304, 305, 307, 308,
        400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 421, 422, 423, 424, 425, 426, 428, 429, 431, 451,
        500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511
    };

    static const std::unordered_set<int> validStatusCodes(validStatusCodesArr, validStatusCodesArr + sizeof(validStatusCodesArr) / sizeof(int));

    return validStatusCodes.count(statusCode) > 0;
}

bool    checkStringIsEmpty(std::string str)
{
    if (str.empty())
        return true;

    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isspace(static_cast<unsigned char>(*it)))
            return false;
    }

    return true;
}

std::string generateRandomFileName()
{
    srand(time(NULL));

    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::string randomName;
    for (int i = 0; i < 10; ++i) {
        int randomIndex = rand() % characters.size();
        randomName += characters[randomIndex];
    }

    return randomName;
}

bool    checkFileExists(const std::string path_filename)
{
    std::ifstream file(path_filename.c_str());

    return (file.good());
}

bool    isDirectory(const std::string path)
{
    DIR* dir = opendir(path.c_str());

    if (dir)
    {
        closedir(dir);
        return true;
    }

    return false;
}

std::string getContentType(const std::string path)
{
    std::string type = path.substr(path.find_last_of(".") + 1);
    MimeTypes mimeTypes;

    std::string contentType = mimeTypes.getMimeType(type);
    if (contentType.empty())
        contentType = "application/octet-stream";

    return contentType;
}

std::string getFileSize(const std::string path)
{
    struct stat st;
    std::string size = "-";

    stat(path.c_str(), &st);
    if ((st.st_size / 1000000000) > 0)
    {
        size = std::to_string(st.st_size / 10000000000) + " Gb";
    }
    else if ((st.st_size / 1000000) > 0)
    {
        size = std::to_string(st.st_size / 1000000) + " Mb";
    }
    else if ((st.st_size / 1000) > 0)
    {
        size = std::to_string(st.st_size / 1000) + " Kb";
    }
    else
    {
        size = std::to_string(st.st_size) + " Bytes";
    }

    return size;
}

std::string getLastModified(const std::string path)
{
    struct stat st;

    stat(path.c_str(), &st);
    std::string lastModified = "<td>" + std::string(ctime(&st.st_mtime)) + "</td>";

    return lastModified;
}
