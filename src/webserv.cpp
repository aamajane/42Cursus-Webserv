# include "webserv.hpp"
# include "networking/ManageServers.hpp"

int main(int ac, char **av)
{
    std::string                     filename;
    ServerParser                    serversFromConfigFile;
    std::vector<ServerParser >     servers;
    ManageServers master;

    // ignore SIGPIPE signal
    // (broken pipe) when trying to write to a socket that has been closed
    signal(SIGPIPE, SIG_IGN);

    filename = (ac == 2) ? av[1] : "config/webserv.conf";
    servers = serversFromConfigFile.get_server(filename);

    master.setupServers(master.getInfoServer(servers));
    master.startServers();
   
    return 0;
}
