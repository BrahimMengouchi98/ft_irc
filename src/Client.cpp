# include "../inc/Client.hpp"

Client::Client()
{
	this->_fd = -1;
	this->_ipadd = "";
	this->nickname = "";
	this->username = "";
	this->isOperator= false;
	this->isRegistered = false;
	this->_buffer = "";
	this->_ipadd = "";
	this->isLogedin = false;
}

Client::~Client(){}

Client::Client(Client const &src) {*this = src;}

Client &Client::operator=(Client const &src)
{
	if (this != &src)
	{
		this->_fd = src._fd;
		this->_ipadd = src._ipadd;
		this->nickname = src.nickname;
		this->username = src.username;
		this->_buffer = src._buffer;
		this->isRegistered = src.isRegistered;
		this->isLogedin = src.isLogedin;
		this->channelsInvite = src.channelsInvite;
	}
	return *this;
}

// Getters
int Client::getFd() {return _fd;}

bool Client::getIsRegistered(){return isRegistered;}

std::string Client::getIpAdd() {return _ipadd;}

std::string Client::getBuffer() {return _buffer;}

std::string Client::getNickname(){return this->nickname;}

bool Client::getIsLogedIn(){return this->isLogedin;}

std::string Client::getUsername(){return this->username;}

bool Client::getInviteChannel(std::string &ChName)
{
	for (size_t i = 0; i < this->channelsInvite.size(); i++)
	{
		if (this->channelsInvite[i] == ChName)
			return true;
	}
	return false;
}

std::string Client::getHostname()
{
	std::string hostname = this->getNickname() + "!" + this->getUsername();
	return hostname;
}

// Setters
void Client::setFd(int fd) 
{
	_fd = fd;
}

void Client::setIpAdd(std::string ipadd) 
{
	_ipadd = ipadd;
}

void Client::setBuffer(std::string buffer) { _buffer += buffer;}

void Client::setNickname(std::string& nickName) {this->nickname = nickName;}

void Client::setIsLogedin(bool value) {this->isLogedin = value;}

void Client::setUsername(std::string& username) {this->username = username;}

void Client::setIsRegistered(bool value) {this->isRegistered = value;}

//---------------//Methods
void Client::clearBuffer()
{
	_buffer.clear();
}

void Client::addChannelInvite(std::string &chname)
{
	channelsInvite.push_back(chname);
}

void Client::removeChannelInvite(std::string &chname)
{
	for (size_t i = 0; i < this->channelsInvite.size(); i++)
	{
		if (this->channelsInvite[i] == chname)
		{
			this->channelsInvite.erase(this->channelsInvite.begin() + i); 
			return;
		}
	}
}