# pragma once

# include "../webserv.hpp"
# include "../networking/Client.hpp"

class ManageServers {
    private :
        std::vector<ConfigServer>           Servers;
        std::map<int, ConfigServer>         serversMap;
        fd_set                              readFd;
        fd_set                              writeFd;
        int                                 biggestFd;
        std::map<int, Client>               clientsMap;

    public : 
        ManageServers();

        ~ManageServers();

        void                                clear();

        //! ----------------------------- getters -----------------------------------

        const std::vector<ConfigServer>&            getServers() const;

        const std::map<int, ConfigServer>&          getServersMap() const;

        fd_set                                      getreadFd() const;

        fd_set                                      getWriteFd() const;

        const std::map<int, Client>&                getClient() const;

        //! ----------------------------- setters -----------------------------------

        void                            setServers(std::vector<ConfigServer>);

        void                            setServersMap(std::map<int, ConfigServer>);

        void                            setreadFd(fd_set);

        void                            setWriteFd(fd_set);

        void                            setClient(std::map<int, Client>);

        //! Methods  ----------------------------------------------------------------
        
        void                                acceptClientConnection(int);

        void                                timeoutCheck();

        void                                initSets();

        void                                readRequest(const int &, Client &);

        void                                closeConnectionClient(const int );

        void                                addToSet(const int , fd_set &);

        void                                removeFromSet(const int , fd_set &);
        
        void                                assignServerToClient(Client &);

        std::vector<ConfigServer>           getInfoServer(const std::vector<ServerParser >& );

        void                                setupServers(const std::vector<ConfigServer>& );

        void                                startServers();

        void                                sendResponse(const int &, Client &);
};