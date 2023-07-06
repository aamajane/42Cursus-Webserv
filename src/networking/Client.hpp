# pragma once

# include "../webserv.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"

class Client
{
    private:
        int                         clientSocket;
        time_t                      lastMsgTime;

    public:
        Client();

        Client(const ConfigServer &);
        
        ~Client();
        
        Request         request;
        ConfigServer    server;
        Response        response;
        
        void                        buildResponse();
        
        // ? ----------------------------- getters -----------------------------------

        int                     getClientSocket() const;

        time_t                  getLastMsgTime() const;

        // ? ----------------------------- setters -----------------------------------

        void                    setClientSocket(int);

        void                    setLastMsgTime(time_t);

        // ? Methodes ----------------------------------------------------------------

        void                    updateTime();

        void                    clear();
};