# include "../inc/Server.hpp"

int Server::fillJoin(std::vector<std::pair<std::string, std::string> > &token, std::vector<std::string> tokens, int fd)
{
	std::vector<std::string> tmp;
	std::string ch, pwd, buff;

	if (tokens.size() < 2)
	{
		sendResponse(ERR_NEEDMOREPARAMS(getClient(fd)->getNickname()), fd);
		return 0;
	}
	for (int i = 1; i < tokens.size(); i++)
		tmp.push_back(tokens[i]);
	ch = tmp[0]; 
	tmp.erase(tmp.begin());
	// check if their is passwords ?
	if (!tmp.empty()) 
	{
		pwd = tmp[0]; 
		tmp.clear();
	}
	for (size_t i = 0; i < ch.size(); i++)
	{
		if (ch[i] == ',')
		{
			token.push_back(std::make_pair(buff, "")); 
			buff.clear();
		}
		else 
			buff += ch[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!pwd.empty())
	{
		size_t j = 0; 
		buff.clear();
		for (size_t i = 0; i < pwd.size(); i++)
		{
			if (pwd[i] == ',')
			{
				token[j++].second = buff;
				buff.clear();
			}	
			else 
				buff += pwd[i];
		}
		// make sure that passwords equals to their channel not exceed it
		if (j < token.size())
			token[j].second =  buff;
	}
	
	for (size_t i = 0; i < token.size(); i++) 
	{
		std::string channel = token[i].first;
		std::string password = token[i].second;
	}
	//erase the empty channel names
	for (size_t i = 0; i < token.size(); i++)
	{
		if (token[i].first.empty())
			token.erase(token.begin() + i--);
	}
    // if the channel doesn't exist
	for (size_t i = 0; i < token.size(); i++)
	{
		if (token[i].first.begin()[0] != '#')
		{
			sendResponse(ERR_NOSUCHCHANNEL(token[i].first), fd);
			//sendError(403, GetClient(fd)->getNickname(), token[i].first, GetClient(fd)->GetFd(), " :No such channel\r\n"); 
			token.erase(token.begin() + i--);
		}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

// check if the client is already in max channels
int Server::searchForClientInChannels(std::string nickname)
{
	int count = 0;
	for (size_t i = 0; i < this->channels.size(); i++)
	{
		if (this->channels[i].getClientInChannel(nickname))
			count++;
	}
	return count;
}

void Server::channelNotExist(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	// check if the client is already in max channels
	if (searchForClientInChannels(getClient(fd)->getNickname()) >= MAX_CHANNELS_PER_CLIENT)
	{
		sendError(405, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :You have joined too many channels\r\n"); 
		return;
	}
	Channel newChannel;
	newChannel.setName(token[i].first);
	newChannel.addAdmin(*getClient(fd));
	newChannel.setCreateAt();
	this->channels.push_back(newChannel);
	
	// notifiy thet the client joined the channel
    sendResponse(RPL_JOINMSG(getClient(fd)->getHostname(),getClient(fd)->getIpAdd(),newChannel.getName()) + \
        RPL_NAMREPLY(getClient(fd)->getNickname(), newChannel.getName(), newChannel.getClientsInChannel()), fd);
	//std::cout << getClient(fd)->getNickname() << " joined to " << newChannel.getName() << "\n";
}

bool IsInvited(Client *cli, std::string ChName, int flag)
{
	if(cli->getInviteChannel(ChName))
	{
		if (flag == 1)
			cli->removeChannelInvite(ChName);
		return true;
	}
	return false;
}

void Server::channelExist(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{	
	// if the client is already registered
	if (this->channels[j].getClientInChannel(getClient(fd)->getNickname()))
		return;

	// if the client is already in max channels
	if (searchForClientInChannels(getClient(fd)->getNickname()) >= MAX_CHANNELS_PER_CLIENT)
	{
		sendError(405, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :You have joined too many channels\r\n"); 
		return;
	}
	// if the password is incorrect
	if (!this->channels[j].getPassword().empty() && this->channels[j].getPassword() != token[i].second)
	{
		if (!IsInvited(getClient(fd), token[i].first, 0))
		{
			sendError(475, getClient(fd)->getNickname(), "#" + token[i].first, getClient(fd)->getFd(), " :Cannot join channel (+k) - bad key\r\n"); 
			return;
		}
	}
	// if the channel is invit only
	if (this->channels[j].getInvitOnly())
	{
		if (!IsInvited(getClient(fd), token[i].first, 1))
		{
			sendError(473, getClient(fd)->getNickname(), "#" + token[i].first, getClient(fd)->getFd(), " :Cannot join channel (+i)\r\n"); 
			return;
		}
	}
	// if the channel reached the limit of number of clients
	if (this->channels[j].getLimit() && this->channels[j].getClientsNumber() >= this->channels[j].getLimit())
	{
		sendError(471, getClient(fd)->getNickname(), "#" + token[i].first, getClient(fd)->getFd(), " :Cannot join channel (+l)\r\n"); 
		return;
	}
	
	//add the client to the channel
	Client *cli = getClient(fd);
	this->channels[j].addClient(*cli);
	if(channels[j].getTopicname().empty())
		sendResponse(RPL_JOINMSG(getClient(fd)->getHostname(),getClient(fd)->getIpAdd(),token[i].first) + \
			RPL_NAMREPLY(getClient(fd)->getNickname(),channels[j].getName(),channels[j].getClientsInChannel()), fd);
	else
		sendResponse(RPL_JOINMSG(getClient(fd)->getHostname(),getClient(fd)->getIpAdd(),token[i].first) + \
			RPL_TOPICIS(getClient(fd)->getNickname(),channels[j].getName(),channels[j].getTopicname()) + \
			RPL_NAMREPLY(getClient(fd)->getNickname(),channels[j].getName(),channels[j].getClientsInChannel()), fd);
    channels[j].sendToAll(RPL_JOINMSG(getClient(fd)->getHostname(), getClient(fd)->getIpAdd(),token[i].first), fd);
}

// JOIN #ch1,#ch2,#ch3 k1,k2,k3
void Server::JOIN(std::vector<std::string> tokens, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	// check if the channel name is empty
	if (!fillJoin(token, tokens, fd))
		return ;
	for (size_t i = 0; i < token.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++)
		{
			if (this->channels[j].getName() == token[i].first)
			{
				channelExist(token, i, j, fd);
				flag = true; 
				break;
			}
		}
		if (!flag)
			channelNotExist(token, i, fd);
	}
}
