#ifndef SERVER_HPP
#define SERVER_HPP

#include "./irc.hpp"

class Client;
class Channel;

class Server
{
	private:
		int port; //-> server port
		std::string pwd;
		int server_fdsocket; //-> server socket file descriptor
		static bool Signal; //-> static boolean for signal
		std::vector<Client> clients; //-> vector of clients
		std::vector<Channel> channels; //-> vector of channels
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
		Client 		*getClientByNickname(std::string nickname);
		Channel 	*getChannel(std::string name);

		// Setters
		void setNickname(std::string cmd, int fd);
		void setUsername(std::string& username, int fd);

		void addClient(Client newClient);
		void addChannel(Channel newChannel);

		// Server Methods
		void init(int port, std::string pwd); //-> server initialization
		void SerSocket(); //-> server socket creation
		void AcceptNewClient(); //-> accept new client
		void ReceiveNewData(int fd); //-> receive new data from a registered client

		//---------------//remove Methods
		void removeClient(int fd);
		void removeChannel(std::string name);
		void removeFd(int fd);
		void removeChannels(int fd);

		//-> signal handler
		static void SignalHandler(int signum); 
	
		void CloseFds(); //-> close file descriptors
		void ClearClients(int fd); //-> clear clients

		// for extracting data from client
		std::vector<std::string> 	splitBuffer(std::string str);
		std::vector<std::string>	extractTokens(std::string& cmd);
		void 						execCmd(std::string &cmd, int fd);

		// for Authenticate User
		bool isValidNickname(std::string& nickname);
		bool isNicknameInUse(std::string& nickname);
		void clientAuth(int fd, std::string cmd);
		bool isRegistered(int fd);

		// SEND Methods
		void sendResponse(std::string response, int fd);
		void sendError(int code, std::string clientname, int fd, std::string msg);
		void sendError(int code, std::string clientname, std::string channelname, int fd, std::string msg);
		
		// JOIN CMD
		void 	JOIN(std::vector<std::string> tokens, int fd);
		int  	fillJoin(std::vector<std::pair<std::string, std::string> > &token, std::vector<std::string> tokens, int fd);
		void	channelNotExist(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
		int		searchForClientInChannels(std::string nickname);
		void 	channelExist(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
		
		// PRIVMSG CMD
		//void 	PRIVMSG(std::vector<std::string> tokens, int fd);
		void	PRIVMSG(std::string cmd, int fd);
		void	CheckExistingChannelsAndClients(std::vector<std::string> &tmp, int fd);
		
		
		// MODE cmd
		// void		MODE(std::string& cmd, int fd);
		void		MODE(std::vector<std::string> tokens, int fd);
		bool 		password_mode(std::string token, Channel *channel, char opera, int fd);
		bool 		operator_mode(std::string token, Channel *channel, int fd, char opera);
		bool		limit_mode(std::string token,  Channel *channel, char opera, int fd);
		void		topic_restriction_mode(Channel *channel , char opera);
		
		
		// QUIT CMD
};

#endif