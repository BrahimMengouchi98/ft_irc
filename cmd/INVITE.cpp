# include "../inc/Server.hpp"

// INVITE CMD
void		Server::INVITE(std::vector<std::string> tokens, int fd)
{
	std::string channelName, nickName;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::cout << "tokens: " << tokens[i] << "\n";
	}

	// // ERR_NEEDMOREPARAMS (461) // if the channel name is empty
	if (tokens.size() < 3)
	{
		sendError(461, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :Not enough parameters\r\n"); 
		return;
	}
	nickName = tokens[1];
	channelName = tokens[2];
	if (channelName[0] == '#')
		channelName = channelName.substr(1);
	
	// ERR_NOSUCHCHANNEL (403) if the given channel does not exist
	if (!getChannel(channelName))
	{
		sendError(403, "#" + channelName, fd, " :No such channel\r\n"); 
		return;
	}
	
	// ERR_NOTONCHANNEL (442) // if the client is not in the channel
	if (!getChannel(channelName)->getClient(fd) && !getChannel(channelName)->getAdmin(fd)) 
	{
		sendError(442, getClient(fd)->getNickname(), "#" + channelName, getClient(fd)->getFd(), " :You're not on that channel\r\n"); 
	}
	
	// ERR_USERONCHANNEL (443) if the given nickname is already on the channel
	if (getChannel(channelName)->getClientInChannel(nickName))
	{
		sendError(443, getClient(fd)->getNickname(), channelName, fd, " :is already on channel\r\n"); 
		return;
	}

	Client *clt = getClientByNickname(nickName);
	// ERR_NOSUCHNICK (401) if the given nickname is not found
	if (!clt)
	{
		sendError(401, nickName, fd, " :No such nick\r\n");
		return;
	}
	// ERR_INVITEONLYCHAN (473) if the channel is invite-only
	if (getChannel(channelName)->getInvitOnly() && !getChannel(channelName)->getAdmin(fd))
	{
		sendError(482, getChannel(channelName)->getClient(fd)->getNickname(), nickName, fd," :You're not channel operator\r\n"); 
		return;
	}
	// ERR_CHANNELISFULL (471) if the channel is full
	if (getChannel(channelName)->getLimit() && getChannel(channelName)->getClientsNumber() >= getChannel(channelName)->getLimit())
	{
		sendError(473, getChannel(channelName)->getClient(fd)->getNickname(), channelName, fd," :Cannot invit to channel (+i)\r\n"); 
		return;
	}
	
	// RPL_INVITING (341) if the invite was successfully sent
	clt->addChannelInvite(channelName);
	sendResponse(RPL_INVITING(getClient(fd)->getNickname(), clt->getNickname(), channelName), fd);
	std::string rep2 = ":"+ clt->getHostname() + " INVITE " + clt->getNickname() + " #" + channelName + "\r\n";
	sendResponse(rep2, clt->getFd());	

}
