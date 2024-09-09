#ifndef SERVER_HPP
#define SERVER_HPP

#include "./irc.hpp"
#include "./Client.hpp"

class Client;

class Server //-> class for server
{
	private:
		int port; //-> server port
		std::string pwd;
		int server_fdsocket; //-> server socket file descriptor
		static bool Signal; //-> static boolean for signal
		std::vector<Client> clients; //-> vector of clients
		std::vector<struct pollfd> fds; //-> vector of pollfd
	
		struct sockaddr_in add;
		struct sockaddr_in cliadd;
	public:
		Server();
		~Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		// Getters
		int 		getFd();
		int 		getPort();
		std::string getPassword();
		Client 		*getClient(int fd);

		void init(int port, std::string pwd); //-> server initialization
		void SerSocket(); //-> server socket creation
		void AcceptNewClient(); //-> accept new client
		void ReceiveNewData(int fd); //-> receive new data from a registered client

		static void SignalHandler(int signum); //-> signal handler
	
		void CloseFds(); //-> close file descriptors
		void ClearClients(int fd); //-> clear clients

		std::vector<std::string> splitBuffer(std::string str);
		std::vector<std::string> extractTokens(std::string& cmd);
		void execCmd(std::string &cmd, int fd);
};

#endif