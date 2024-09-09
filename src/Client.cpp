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

// Setters
void Client::setFd(int fd) {_fd = fd;}

void Client::setIpAdd(std::string ipadd) {_ipadd = ipadd;}

void Client::setBuffer(std::string buffer) { _buffer += buffer;}

void Client::setNickname(std::string& nickName) {this->nickname = nickName;}

void Client::setIsLogedin(bool value) {this->isLogedin = value;}

void Client::setUsername(std::string& username) {this->username = username;}

void Client::setIsRegistered(bool value) {this->isRegistered = value;}

