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
		static bool signal; //-> static boolean for signal
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
		void createSocket(); //-> server socket creation
		void acceptNewClient(); //-> accept new client
		void receiveNewData(int fd); //-> receive new data from a registered client

		//---------------//remove Methods
		void removeClient(int fd);
		void removeChannel(std::string name);
		void removeFd(int fd);
		void removeChannels(int fd);

		//-> signal handler
		static void signalHandler(int signum); 
	
		void closeFds(); //-> close file descriptors
		void clearClients(int fd); //-> clear clients

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
		//void sendError(std::string msg, int fd);

		// JOIN CMD
		void 	JOIN(std::vector<std::string> tokens, int fd);
		int  	fillJoin(std::vector<std::pair<std::string, std::string> > &token, std::vector<std::string> tokens, int fd);
		void	channelNotExist(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
		int		searchForClientInChannels(std::string nickname);
		void 	channelExist(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
		
		// PRIVMSG CMD
		void	PRIVMSG(std::string cmd, int fd);
		void	CheckExistingChannelsAndClients(std::vector<std::string> &tmp, int fd);
		
		
		// MODE CMD
		void		MODE(std::vector<std::string> tokens, int fd);
		bool 		password_mode(std::string token, Channel *channel, char opera, int fd);
		bool 		operator_mode(std::string token, Channel *channel, int fd, char opera);
		bool		limit_mode(std::string token,  Channel *channel, char opera, int fd);
		void		topic_restriction_mode(Channel *channel , char opera);
		
		// PART CMD
		void		PART(std::vector<std::string> tokens, int fd);
		
		// KICK CMD
		void		KICK(std::vector<std::string> tokens, int fd);

		// TOPIC CMD
		void		TOPIC(std::vector<std::string> tokens, int fd);

		// INVITE CMD
		void		INVITE(std::vector<std::string> tokens, int fd);
		
		// QUIT CMD
		void		QUIT(std::vector<std::string> tokens, int fd);

		// check if port valid
		bool 		isPortValid(std::string port);
};

#endif