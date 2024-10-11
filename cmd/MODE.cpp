#include "../inc/Server.hpp"

void	Server::topic_restriction_mode(Channel *channel , char opera)
{
	if(opera == '+' && !channel->getModeAtIndex(1))
	{
		channel->setModeAtIndex(1, true);
		channel->setTopicRestriction(true);
	}
	else if (opera == '-' && channel->getModeAtIndex(1))
	{
		channel->setModeAtIndex(1, false);
		channel->setTopicRestriction(false);
	}
}

bool	isValidLimit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789") != std::string::npos) && std::atoi(limit.c_str()) > 0);
}

bool	Server::limit_mode(std::string token,  Channel *channel, char opera, int fd)
{
	std::string limit;
	if (opera == '+' && token.empty())
	{
		sendResponse(ERR_NEEDMODEPARM(channel->getName(),"(l)"), fd);
		return false;
	}

	if(opera == '+')
	{
		limit = token;
		if(!isValidLimit(limit))
		{
			sendResponse(ERR_INVALIDMODEPARM(channel->getName(),"(l)"), fd);
			return false;
		}
		else
		{
			channel->setModeAtIndex(4, true);
			channel->setLimit(std::atoi(limit.c_str()));
		}
	}
	else if (opera == '-' && channel->getModeAtIndex(4))
	{
		channel->setModeAtIndex(4, false);
		channel->setLimit(0);
	}
	return true;
}

bool	isValidPassword(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}

bool	Server::password_mode(std::string token, Channel *channel, char opera, int fd)
{
	std::string pass;
	
	if (opera == '+' && token.empty())
	{
		sendResponse(ERR_NEEDMODEPARM(channel->getName(), std::string("(k)")), fd);
		return false;
	}
	
	if(opera == '+')
	{
		pass = token;
		if(!isValidPassword(pass))
		{
			sendResponse(ERR_INVALIDMODEPARM(channel->getName(), std::string("(k)")), fd);
			return false;
		}
		channel->setModeAtIndex(2, true);
		channel->setPassword(pass);
	}
	else if (opera == '-' && channel->getModeAtIndex(2))
	{		
		channel->setModeAtIndex(2, false);
		channel->setPassword("");
		sendResponse(RPL_KEYREMOVED(channel->getName()), fd);
	}
	return true;
}

void	invite_only_mode(Channel *channel, char opera)
{
	if(opera == '+' && !channel->getModeAtIndex(0))
	{
		channel->setModeAtIndex(0, true);
		channel->setInvitOnly(1);
	}
	else if (opera == '-' && channel->getModeAtIndex(0))
	{
		channel->setModeAtIndex(0, false);
		channel->setInvitOnly(0);
	}
}

bool	Server::operator_mode(std::string token, Channel *channel, int fd, char opera)
{
	std::string user;

	if(token.empty())
	{
		sendResponse(ERR_NEEDMODEPARM(channel->getName(),"(o)"), fd);
		return false;
	}

	user = token;
	if(!channel->isClientInChannel(user))
	{
		sendResponse(ERR_NOSUCHNICK(channel->getName(), user), fd);
		return false;
	}

	if(opera == '+')
	{
		channel->setModeAtIndex(3, true);
		channel->changeClientToAdmin(user);
	}
	else if (opera == '-')
	{
		channel->setModeAtIndex(3,false);
		channel->changeAdminToClient(user);
	}
	return true;
}

void	Server::MODE(std::vector<std::string> tokens, int fd)
{
	std::string channelName, params, modeset;
	Channel *channel;
	bool is_print = true;
	char opera;

	// for (int i = 0; i < tokens.size(); i++)
	// {
	// 	std::cout << "token: " << tokens[i] << "\n";
	// }
	Client *cli = getClient(fd);
	if (tokens.size() < 3)
	{
		sendResponse(ERR_NOTENOUGHPARAM(cli->getNickname()), fd); 
		return ;
	}
	channelName = tokens[1];
	modeset = tokens[2];
	if (tokens.size() > 3)
		params = tokens[3];
	if(channelName[0] != '#' || !(channel = getChannel(channelName.substr(1))))
	{
		sendResponse(ERR_CHANNELNOTFOUND(cli->getUsername(),channelName), fd);
		return ;
	}
	else if (!channel->getClient(fd) && !channel->getAdmin(fd))
	{
		sendError(442, getClient(fd)->getNickname(), channelName, getClient(fd)->getFd(), " :You're not on that channel\r\n"); 
		return;
	}
	// response with the channel modes (MODE #channel)
	else if (modeset.empty() || modeset.size() != 2)
	{
		sendResponse(RPL_CHANNELMODES(cli->getNickname(), channel->getName(), channel->getModes()) + \
		RPL_CREATIONTIME(cli->getNickname(), channel->getName(), channel->getCreationAt()), fd);
		return ;
	}
	// client is not channel operator
	else if (!channel->getAdmin(fd)) 
	{
		sendResponse(ERR_NOTOPERATOR(channel->getName()), fd);
		return ;
	}
	else if(channel)
	{
		opera = modeset[0];
		if (opera == '-' || opera == '+')
		{
			//invite mode
			if(modeset[1] == 'i')
				invite_only_mode(channel , opera);
			
			//topic restriction mode
			else if (modeset[1] == 't')
			{
				topic_restriction_mode(channel, opera);
				params = tokens[2];
			} 
			
			// password
			else if (modeset[1] == 'k')
				is_print = password_mode(params, channel, opera, fd);
			
			// user operator privilege
			else if (modeset[1] == 'o')
				is_print = operator_mode(params, channel, fd, opera);

			// channel limits
			else if (modeset[1] == 'l') 
				is_print = limit_mode(params, channel, opera, fd);
			else
			{
				sendResponse(ERR_UNKNOWNMODE(cli->getNickname(), channel->getName(), modeset[1]), fd);
				return;
			}
		}
		else
		{
			sendResponse(ERR_UNKNOWNMODE(cli->getNickname(), channel->getName(), modeset), fd);
			return;
		}
	}
	if(is_print)
 		channel->sendToAll(RPL_CHANGEMODE(cli->getHostname(), channel->getName(), modeset, params));
}
