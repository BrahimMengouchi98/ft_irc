#ifndef SERVER_HPP
#define SERVER_HPP

#include "./irc.hpp"
#include "./Client.hpp"

class Client;

class Server
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


		// Setters
		void setNickname(std::string cmd, int fd);
		void setUsername(std::string& username, int fd);

		void init(int port, std::string pwd); //-> server initialization
		void SerSocket(); //-> server socket creation
		void AcceptNewClient(); //-> accept new client
		void ReceiveNewData(int fd); //-> receive new data from a registered client

		//-> signal handler
		static void SignalHandler(int signum); 
	
		void CloseFds(); //-> close file descriptors
		void ClearClients(int fd); //-> clear clients

		// for extracting data from client
		std::vector<std::string> splitBuffer(std::string str);
		std::vector<std::string> extractTokens(std::string& cmd);
		void execCmd(std::string &cmd, int fd);

		// for Authenticate User
		bool isValidNickname(std::string& nickname);
		bool isNicknameInUse(std::string& nickname);
		void clientAuth(int fd, std::string cmd);
		bool isRegistered(int fd);
};

#endif