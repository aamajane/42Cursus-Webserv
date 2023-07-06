#pragma once

#include "../webserv.hpp"

class Location;

class ConfigServer {
    private :
        uint16_t						Port;
	    std::string						Host; 
		std::string						serverName;
		std::map<short, std::string>    errorPages;
		unsigned long					clientMaxBodySize;
        std::vector<Location >          locationList;
        int     						Fd;
        struct sockaddr_in 				serverAddress;

    public : 
        ConfigServer();
        
        ConfigServer(std::string , std::string , std::string , std::vector<Location > , std::map<short, std::string>, unsigned long );

        ConfigServer(const ConfigServer &);

        ConfigServer & operator=(const ConfigServer &);

        ~ConfigServer();

        void        clear();

        //! ----------------------------- getters -----------------------------------

        uint16_t                            getPort() const;
        
        std::string                         getHost() const;

        std::string                         getServerName() const;

        std::map<short, std::string>        getErrorPages() const;

        unsigned long                       getClientMaxBodySize() const;

		std::vector<Location >			    getLocationList() const;

        int                                 getFd() const;

        //! ----------------------------- setters -----------------------------------

        void                                setPort(std::string);
        
        void                                setHost(std::string);

        void                                setServerName(std::string);

        void                                setErrorPages(std::map<short, std::string>);

        void                                setClientMaxBodySize(unsigned long );

		void                                setLocationList(std::vector<Location >);

        void                                setFd(int);

        // ! Methods ----------------------------------------------------------------

        void                                setupServer();

        void                                printServerInfo();
};