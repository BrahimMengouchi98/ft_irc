# include "../inc/Server.hpp"

bool Server::Signal = false; //-> initialize the static boolean

Server::Server() {server_fdsocket = -1;}

Server::~Server(){}

Server::Server(Server const &src){*this = src;}

Server &Server::operator=(Server const &src)
{
	if (this != &src)
	{
		this->port = src.port;
		this->server_fdsocket = src.server_fdsocket;
		this->pwd = src.pwd;
		this->clients = src.clients;
		this->channels = src.channels;
		this->fds = src.fds;
	}
	return *this;
}

//---------------//Getters
int Server::getPort() {return this->port;}

int Server::getFd() {return this->server_fdsocket;}

Client *Server::getClient(int fd)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].getFd() == fd)
			return &this->clients[i];
	}
	return NULL;
}

Client 		*Server::getClientByNickname(std::string nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].getNickname() == nickname)
			return &this->clients[i];
	}
	return NULL;
}

Channel *Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->channels.size(); i++)
	{
		if (this->channels[i].getName() == name)
			return &channels[i];
	}
	return NULL;
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true; //-> set the static boolean to true to stop the server
}

void Server::CloseFds()
{
	for(size_t i = 0; i < clients.size(); i++)
	{ //-> close all the clients
		std::cout << RED << "Client <" << clients[i].getFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].getFd());
	}
	if (server_fdsocket != -1)
	{ 
		//-> close the server socket
		std::cout << RED << "Server <" << server_fdsocket << "> Disconnected" << WHI << std::endl;
		close(server_fdsocket);
	}
}

void Server::SerSocket()
{
	struct pollfd NewPoll;
	add.sin_family = AF_INET; //-> set the address family to ipv4
	add.sin_port = htons(this->port); //-> convert the port to network byte order (big endian)
	add.sin_addr.s_addr = INADDR_ANY; //-> set the address to any local machine address

	server_fdsocket = socket(AF_INET, SOCK_STREAM, 0); //-> create the server socket
	if(server_fdsocket == -1) //-> check if the socket is created
		throw(std::runtime_error("faild to create socket"));

	int en = 1;
	if(setsockopt(server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(server_fdsocket, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	if (bind(server_fdsocket, (struct sockaddr *)&add, sizeof(add)) == -1) //-> bind the socket to the address
		throw(std::runtime_error("faild to bind socket"));
	if (listen(server_fdsocket, SOMAXCONN) == -1) //-> listen for incoming connections and making the socket a passive socket
		throw(std::runtime_error("listen() faild"));

	NewPoll.fd = server_fdsocket; //-> add the server socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0; //-> set the revents to 0
	fds.push_back(NewPoll); //-> add the server socket to the pollfd
}

void Server::init(int port, std::string pwd)
{
	this->port = port;
	this->pwd = pwd;

	SerSocket(); //-> create the server socket

	std::cout << GRE << "Server <" << server_fdsocket << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::Signal == false) //-> run the server until the signal is received
	{
		if((poll(&fds[0], fds.size(), -1) == -1) && Server::Signal == false) //-> wait for an event
			throw(std::runtime_error("poll() faild"));

		for (size_t i = 0; i < fds.size(); i++) //-> check all file descriptors
		{
			if (fds[i].revents & POLLIN)//-> check if there is data to read
			{
				if (fds[i].fd == server_fdsocket)
					AcceptNewClient(); //-> accept new client
				else
					ReceiveNewData(fds[i].fd); //-> receive new data from a registered client
			}
		}
	}
	CloseFds(); //-> close the file descriptors when the server stops
}

void Server::AcceptNewClient()
{
	Client cli; //-> create a new client
	
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(server_fdsocket, (sockaddr *)&(cliadd), &len); //-> accept the new client
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		{std::cout << "fcntl() failed" << std::endl; return;}

	NewPoll.fd = incofd; //-> add the client socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0; //-> set the revents to 0

	cli.setFd(incofd); //-> set the client file descriptor
	cli.setIpAdd(inet_ntoa((cliadd.sin_addr))); //-> convert the ip address to string and set it
	clients.push_back(cli); //-> add the client to the vector of clients
	fds.push_back(NewPoll); //-> add the client socket to the pollfd

	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
}

void Server::ReceiveNewData(int fd)
{
	std::vector<std::string> cmd;
	char buff[1024]; //-> buffer for the received data
	memset(buff, 0, sizeof(buff)); //-> clear the buffer
	Client *client = getClient(fd);
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0); //-> receive the data

	//-> check if the client disconnected
	if(bytes <= 0)
	{ 
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		ClearClients(fd); //-> clear the client
		close(fd); //-> close the client socket
	}

	else
	{ //-> print the received data
		buff[bytes] = '\0';
		client->setBuffer(buff);

		// split buffer line by line
		cmd = splitBuffer(client->getBuffer());
		//std::cout << this->server_fdsocket << " : " << fd << "\n";
		for(size_t i = 0; i < cmd.size(); i++)
		{
			execCmd(cmd[i], fd);
			//std::cout << cmd[i] << "\n";
			std::cout << "------------------\n";
			//std::cout << "line: " << cmd[i] << "\n";
			
		}
		if(getClient(fd))
			getClient(fd)->clearBuffer();
		//std::cout << "bytes: " << bytes << "\n";
		//std::cout << YEL << "Client <" << fd << "> Data: " << WHI << buff;
		//here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
	}
}
// split buffer by \r\n
std::vector<std::string> Server::splitBuffer(std::string str)
{
	std::vector<std::string> vec;
	std::istringstream stm(str);
	std::string line;
	while(std::getline(stm, line))
	{
		size_t pos = line.find_first_of("\r\n");
		if(pos != std::string::npos)
			line = line.substr(0, pos);
		vec.push_back(line);
	}
	return vec;
}

std::vector<std::string> Server::extractTokens(std::string& cmd)
{
	std::vector<std::string> vec;
	std::istringstream stm(cmd);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}

bool Server::isRegistered(int fd)
{
	if (!getClient(fd) || getClient(fd)->getNickname().empty() 
		|| getClient(fd)->getUsername().empty() 
		|| getClient(fd)->getNickname() == "*"  
		|| !getClient(fd)->getIsLogedIn())
		return false;
	return true;
}

void Server::execCmd(std::string &cmd, int fd)
{
	std::vector<std::string> tokens = extractTokens(cmd);
	size_t found = cmd.find_first_not_of(" \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
	if (tokens.size())
	{
		std::cout << "token: " << tokens[0] << "\n";
		if (tokens[0] == "PASS" || tokens[0] == "pass")
		{ 
			std::cout << "pass\n";
			// authenticate user
			clientAuth(fd, cmd);
		}
		else if (tokens[0] == "NICK" || tokens[0] == "nick")
		{
			// set a nickname
			setNickname(cmd, fd);
			//std::cout << "nickname: " << getClient(fd)->getNickname();
			//std::cout << "nick\n";
		}
		else if(tokens[0] == "USER" || tokens[0] == "user")
		{
			// set username
			//std::cout << "user\n";
			setUsername(cmd, fd);
		}
		else if (tokens[0] == "QUIT" || tokens[0] == "quit")
		{
			// quit
			std::cout << "quit\n";
		}
		else if (isRegistered(fd))
		{

			if (tokens[0] == "KICK" || tokens[0] == "kick")
			{
				//KICK(cmd, fd);

			}
			else if (tokens[0] == "JOIN" || tokens[0] == "join")
			{
				JOIN(tokens, fd);
				//std::cout << "join\n";

			}
			else if (tokens[0] == "TOPIC" || tokens[0] == "topic")
			{
				//Topic(cmd, fd);

			}
			else if (tokens[0] == "MODE" || tokens[0] == "mode")
			{
				// MODE(cmd, fd);
				MODE(tokens, fd);
			}
			else if (tokens[0] == "PART" || tokens[0] == "part")
			{

				//PART(cmd, fd);
			}
			else if (tokens[0] == "PRIVMSG" || tokens[0] == "privmsg")
			{
				std::cout << "privmsg!!\n";
				PRIVMSG(cmd, fd);

			}
			else if (tokens[0] == "INVITE" || tokens[0] == "invite")
			{
				std::cout << "invite\n";
				//Invite(cmd,fd);

			}
			else
				sendResponse(ERR_CMDNOTFOUND(getClient(fd)->getNickname(), tokens[0]), fd);
		}
		else if (!isRegistered(fd))
			sendResponse(ERR_NOTREGISTERED(getClient(fd)->getNickname()), fd);
	}
		//std::cout << "registered !!\n";
	// for(size_t i = 0; i < tokens.size(); ++i)
	// 	std::cout << "token: " << tokens[i] << "\n";
}
//-> clear the clients
void Server::ClearClients(int fd)
{ 
	//-> remove the client from the pollfd
	for(size_t i = 0; i < fds.size(); i++)
	{ 
		if (fds[i].fd == fd)
		{
			fds.erase(fds.begin() + i); 
			break;
		}
	}
	//-> remove the client from the vector of clients
	for(size_t i = 0; i < clients.size(); i++)
	{ 
		if (clients[i].getFd() == fd)
		{
			clients.erase(clients.begin() + i); 
			break;
		}
	}
}

//---------------//Send Methods

void Server::sendError(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() faild" << std::endl;
}

void Server::sendError(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() faild" << std::endl;
}

void Server::sendResponse(std::string response, int fd)
{
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Response send() faild" << std::endl;
}

//---------------//remove Methods
void Server::removeClient(int fd)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].getFd() == fd)
		{
			this->clients.erase(this->clients.begin() + i); 
			return;
		}
	}
}

void Server::removeChannel(std::string name)
{
	for (size_t i = 0; i < this->channels.size(); i++)
	{
		if (this->channels[i].getName() == name)
		{
			this->channels.erase(this->channels.begin() + i); 
			return;
		}
	}
}

void Server::removeFd(int fd)
{
	for (size_t i = 0; i < this->fds.size(); i++)
	{
		if (this->fds[i].fd == fd)
		{
			this->fds.erase(this->fds.begin() + i); 
			return;
		}
	}
}