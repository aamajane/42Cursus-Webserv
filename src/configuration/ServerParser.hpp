#ifndef SERVER_HPP
#define SERVER_HPP

#include "../webserv.hpp"

class Location;

class ServerParser
{
    private:
        /* data */
        std::string                     port;
        std::string                     host;
        std::string                     server_name;
        std::map<short, std::string>    error_pages;
        unsigned long                   client_max_body_size;
        std::vector<Location>           _location;
    public:
        //constructors
        ServerParser();
        ~ServerParser();
        ServerParser(const ServerParser& other);
        ServerParser& operator=(const ServerParser& other);

        //getters
        std::string getPort() const;
        const std::string& getHost() const;
        const std::string& getServerName() const;
        const std::map<short, std::string>& getErrorPages() const;
        unsigned long getClientMaxBodySize() const;
        std::vector<Location> get_locations() const;
        
        //setters
        void setPort(std::string& );
        void setHost(std::string& );
        void setServerName(std::string& );
        void setErrorPages(std::string & );
        void setClientMaxBodySize(std::string& );
        void set_locations(Location new_locations);

        //functions
        void                            printTheServerInfo(void);
        std::vector<ServerParser >      get_server(std::string filename);
        void                            checkServersAndLocations(std::vector<ServerParser >);

        void        clear();
};

#endif