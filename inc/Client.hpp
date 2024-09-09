#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Server.hpp"

class Client
{
	private:
		int 		_fd; //-> client file descriptor
		std::string _ipadd; //-> client ip address
		std::string _buffer;
		bool 		isOperator;
		bool 		isRegistered;
		std::string nickname;
		bool 		isLogedin;
		std::string username;

	public:
		Client();
		~Client();
		Client(Client const &src);
		Client &operator=(Client const &src);

		// Getters
		int 		getFd();
		std::string getIpAdd();
		std::string getBuffer();
		bool 		getIsRegistered();
		std::string getNickname();
		bool 		getIsLogedIn();
		std::string getUsername();

		// Setters
		void setFd(int fd); //-> setter for fd
		void setIpAdd(std::string ipadd); //-> setter for ipadd
		void setBuffer(std::string buffer);
		void setNickname(std::string& nickName);
		void setIsLogedin(bool value);
		void setUsername(std::string& username);
		void setIsRegistered(bool value);
};

#endif