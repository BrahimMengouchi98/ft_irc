#include "../inc/Server.hpp"

/* PASS COMMAND */
void Server::clientAuth(int fd, std::string cmd)
{
	Client *cli = getClient(fd);
	cmd = cmd.substr(4);
	size_t pos = cmd.find_first_not_of("\t\v ");
	if(pos < cmd.size())
		cmd = cmd.substr(pos);

	if(pos == std::string::npos || cmd.empty()) 
		// _sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		std::cout << "not enough params !!\n";
	else if(!cli->getIsRegistered())
	{
		std::string pass = cmd;
		if(pass == pwd)
		{
			cli->setIsRegistered(true);
			//std::cout << "registered successefuly !!\n";
		}
		else
            //_sendResponse(ERR_INCORPASS(std::string("*")), fd);
			std::cout << "Incorrect PASS\n";
	}
	else
        // _sendResponse(ERR_ALREADYREGISTERED(GetClient(fd)->GetNickName()), fd);
		std::cout << "Already Registered: " << cli->getNickname() << "\n";
}

/* NICK COMMAND */
bool Server::isValidNickname(std::string& nickname)
{
		
	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
		return false;
	for(size_t i = 1; i < nickname.size(); i++)
	{
		if(!std::isalnum(nickname[i]) && nickname[i] != '_')
			return false;
	}
	return true;
}

bool Server::isNicknameInUse(std::string& nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].getNickname() == nickname)
			return true;
	}
	return false;
}


void Server::setNickname(std::string cmd, int fd)
{
	std::string inuse;
	cmd = cmd.substr(4);
	size_t pos = cmd.find_first_not_of("\t\v ");
	if(pos < cmd.size())
		cmd = cmd.substr(pos);
	Client *cli = getClient(fd);
	if(pos == std::string::npos || cmd.empty())
	{
		//_sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd); 
		std::cout << "not enough params\n";
		return;
	}
	if (isNicknameInUse(cmd) && cli->getNickname() != cmd)
	{
		inuse = "*";
		if(cli->getNickname().empty())
			cli->setNickname(inuse);
	    //_sendResponse(ERR_NICKINUSE(std::string(cmd)), fd); 
		std::cout << "nick name already inuse!!\n";
		return;
	}
	if(!isValidNickname(cmd)) 
	{
		//_sendResponse(ERR_ERRONEUSNICK(std::string(cmd)), fd);
		std::cout << "not valid nickname !!\n";
		return;
	}
	else
	{
		if(cli && cli->getIsRegistered())
		{
			std::string oldnick = cli->getNickname();
			cli->setNickname(cmd);
			if(!oldnick.empty() && oldnick != cmd)
			{
				if(oldnick == "*" && !cli->getUsername().empty())
				{
					cli->setIsLogedin(true);
					// _sendResponse(RPL_CONNECTED(cli->GetNickName()), fd);
					// _sendResponse(RPL_NICKCHANGE(cli->GetNickName(),cmd), fd);
				}
				// else
				// 	_sendResponse(RPL_NICKCHANGE(oldnick,cmd), fd);
				return;
			}
			
		}
		else if (cli && !cli->getIsRegistered())
			// _sendResponse(ERR_NOTREGISTERED(cmd), fd);
			std::cout << "not registered !!\n";
	}
	if(cli && cli->getIsRegistered() && !cli->getUsername().empty() && !cli->getNickname().empty() && cli->getNickname() != "*" && !cli->getIsLogedIn())
	{
		cli->setIsLogedin(true);
		// _sendResponse(RPL_CONNECTED(cli->getNickname()), fd);
		std::cout << "user conneted !! " << cli->getNickname() << "\n";
	}
}


/* USER COMMAND */
void	Server::setUsername(std::string& cmd, int fd)
{
	std::vector<std::string> tokens = extractTokens(cmd);

	Client *cli = getClient(fd); 
	if((cli && tokens.size() < 5))
	{
		//_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd); 
		std::cout << "not enough params !!\n";
		return; 
	}
	if(!cli  || !cli->getIsRegistered())
	{
		// _sendResponse(ERR_NOTREGISTERED(std::string("*")), fd);
		std::cout << "not registered !!\n";
	}
	else if (cli && !cli->getUsername().empty())
	{
		//_sendResponse(ERR_ALREADYREGISTERED(cli->GetNickName()), fd); 
		std::cout << "already regitered !\n";
		return;
	}
	else
		cli->setUsername(tokens[1]);
	if(cli && cli->getIsRegistered() && !cli->getUsername().empty() && !cli->getNickname().empty() && cli->getNickname() != "*"  && !cli->getIsLogedIn())
	{
		cli->setIsLogedin(true);
		// _sendResponse(RPL_CONNECTED(cli->GetNickName()), fd);
		std::cout << "user connected !!\n";
	}
}