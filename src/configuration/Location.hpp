#ifndef LOCATION_HPP
#define LOCATION_HPP
 
#include "../webserv.hpp"

class Location
{    
    public:
        Location();
        ~Location();
        Location (const Location& other);
        Location& operator=(const Location& other);

        //getters
        const std::vector<std::string>& getMethod() const;
        const std::string&  getRoot() const;
        const std::string&  getUpload() const;
        const std::string&  getAutoindex() const;
        const std::string&  getIndex() const;
        const std::string&  getRedirection() const;
        const std::string&  getLocation() const;
        const std::string&  getCgi() const;

        //setters
        void setMethod(std::string , bool );
        void setRoot(std::string& );
        void setUpload(std::string& );
        void setIndex(std::string& );
        void setAutoindex(std::string& );
        void setRedirection(std::string& );
        void setLocation(std::string& );
        void setCgi(std::string & );

        void    clear();

        void printLocationInfo();

        private:
            /* data */
            std::string location_path;
            std::vector<std::string> method;
            std::string root;
            std::string upload;
            std::string autoindex;
            std::string index;
            std::string redirection;
            std::string cgi;
    };

#endif