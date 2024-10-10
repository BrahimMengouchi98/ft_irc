# include "../inc/Server.hpp"

// TOPIC CMD
void		Server::TOPIC(std::vector<std::string> tokens, int fd)
{
	std::string channelName, topic;

	// // ERR_NEEDMOREPARAMS (461) // if the channel name is empty
	if (tokens.size() < 2)
	{
		sendError(461, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :Not enough parameters\r\n"); 
		return;
	}
	channelName = tokens[1];
	if (channelName[0] == '#')
		channelName = channelName.substr(1);
	if (tokens.size() > 2)
	{
		for (size_t i = 2; i < tokens.size(); i++)
		{
			topic += tokens[i];
			if (i < tokens.size() - 1)
				topic += " ";
		}
	}
	if (topic[0] == ':')
		topic = topic.substr(1);
	
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
	
	// see topic name
	if (tokens.size() == 2)
	{
		// RPL_NOTOPIC (331) if no topic is set
		if (getChannel(channelName)->getTopicname() == "")
		{
			sendResponse(": 331 " + getClient(fd)->getNickname() + " " + "#" + channelName + " :No topic is set\r\n", fd);
			return;
		}
		else
		{
			// RPL_TOPIC (332) if the topic is set
			sendResponse(RPL_TOPICIS(getClient(fd)->getNickname(), channelName, getChannel(channelName)->getTopicname()), fd);
			// RPL_TOPICWHOTIME (333) if the topic is set
			sendResponse(": 333 " + getClient(fd)->getNickname() + " " + "#" + channelName + " " + getClient(fd)->getNickname() + " " + getChannel(channelName)->getTime() + "\r\n", fd);
			return;	
		}
	}
	// end see topic name

	// ERR_CHANOPRIVSNEEDED (482) if the client is not a channel operator
	if (getChannel(channelName)->getTopicRestriction() && getChannel(channelName)->getClient(fd))
	{
		sendError(482, "#" + channelName, fd, " :You're Not a channel operator\r\n");
		return;
	}
	
	else if (getChannel(channelName)->getTopicRestriction() && getChannel(channelName)->getAdmin(fd))
	{
		getChannel(channelName)->setTime();
		getChannel(channelName)->setTopicName(topic);
		std::string rpl;
		// RPL_TOPIC (332) if the topic is set
		rpl = ":" + getClient(fd)->getNickname() + "!" + getClient(fd)->getUsername() + "@localhost TOPIC #" + channelName + " :" + getChannel(channelName)->getTopicname() + "\r\n";
		getChannel(channelName)->sendToAll(rpl);
	}
	else if (!getChannel(channelName)->getTopicRestriction())
	{
		getChannel(channelName)->setTime();
		getChannel(channelName)->setTopicName(topic);
		std::string rpl;
		// RPL_TOPIC (332) if the topic is set
		rpl = ":" + getClient(fd)->getNickname() + "!" + getClient(fd)->getUsername() + "@localhost TOPIC #" + channelName + " :" + getChannel(channelName)->getTopicname() + "\r\n";
		getChannel(channelName)->sendToAll(rpl);	
	}
}
