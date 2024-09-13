#include "../inc/Server.hpp"

// std::vector<std::string> Server::splitParams(std::string params)
// {
// 	if(!params.empty() && params[0] == ':')
// 		params.erase(params.begin());
// 	std::vector<std::string> tokens;
// 	std::string param;
// 	std::istringstream stm(params);
// 	while (std::getline(stm, param, ','))
// 	{
// 		tokens.push_back(param);
// 		param.clear();
// 	}
// 	return tokens;
// }

// std::string Server::topic_restriction(Channel *channel ,char opera, std::string chain)
// {
// 	std::string param;
// 	param.clear();
// 	if(opera == '+' && !channel->getModeAtIndex(1))
// 	{
// 		channel->setModeAtIndex(1, true);
// 		channel->set_topicRestriction(true);
// 		param =  mode_toAppend(chain, opera, 't');
// 	}
// 	else if (opera == '-' && channel->getModeAtIndex(1))
// 	{
// 		channel->setModeAtIndex(1, false);
// 		channel->set_topicRestriction(false);
// 		param =  mode_toAppend(chain, opera, 't');
// 	}	
// 	return param;
// }


// bool Server::isvalid_limit(std::string& limit)
// {
// 	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
// }

// std::string Server::channel_limit(std::vector<std::string> tokens,  Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments)
// {
// 	std::string limit;
// 	std::string param;

// 	param.clear();
// 	limit.clear();
// 	if(opera == '+')
// 	{
// 		if(tokens.size() > pos )
// 		{
// 			limit = tokens[pos++];
// 			if(!isvalid_limit(limit))
// 			{
// 				_sendResponse(ERR_INVALIDMODEPARM(channel->GetName(),"(l)"), fd);
// 			}
// 			else
// 			{
// 				channel->setModeAtIndex(4, true);
// 				channel->setLimit(std::atoi(limit.c_str()));
// 				if(!arguments.empty())
// 					arguments += " ";
// 				arguments += limit;
// 				param =  mode_toAppend(chain, opera, 'l');
// 			}
// 		}
// 		else
// 			_sendResponse(ERR_NEEDMODEPARM(channel->GetName(),"(l)"),fd);
// 	}
// 	else if (opera == '-' && channel->getModeAtIndex(4))
// 	{
// 		channel->setModeAtIndex(4, false);
// 		channel->setLimit(0);
// 		param  = mode_toAppend(chain, opera, 'l');
// 	}
// 	return param;
// }


bool validPassword(std::string password)
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

void	Server::password_mode(std::string token, Channel *channel, char opera, int fd)
{
	std::string pass;
	
	pass.clear();
	if (token.empty())
		sendResponse(ERR_NEEDMODEPARM(channel->getName(), std::string("(k)")), fd);
	
	pass = token;
	std::cout << "pass: " << pass << "\n";
	if(!validPassword(pass))
		sendResponse(ERR_INVALIDMODEPARM(channel->getName(), std::string("(k)")), fd);
	
	if(opera == '+')
	{
		channel->setModeAtIndex(2, true);
		channel->setPassword(pass);
	}
	else if (opera == '-' && channel->getModeAtIndex(2))
	{
		if(pass == channel->getPassword())
		{		
			channel->setModeAtIndex(2, false);
			channel->setPassword("");
		}
		else
			sendResponse(ERR_KEYSET(channel->getName()),fd);
	}
}


void	invite_only(Channel *channel, char opera)
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

void getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params)
{
	std::istringstream stm(cmd);
	stm >> name;
	stm >> modeset;
	stm >> params;
	// size_t found = cmd.find_first_not_of(name + modeset + " \t\v");
	// if(found != std::string::npos)
	// 	params = cmd.substr(found);
}

void	Server::operator_privilege(std::string token, Channel *channel, int fd, char opera)
{
	std::string user;

	user.clear();
	if(token.empty())
		sendResponse(ERR_NEEDMODEPARM(channel->getName(),"(o)"), fd);

	user = token;
	if(!channel->isClientInChannel(user))
		sendResponse(ERR_NOSUCHNICK(channel->getName(), user), fd);

	if(opera == '+')
	{
		channel->setModeAtIndex(3, true);
		if(channel->changeClientToAdmin(user))
		{
			// if(!arguments.empty())
			// 	arguments += " ";
			// arguments += user;
		}
	}
	else if (opera == '-')
	{
		channel->setModeAtIndex(3,false);
		if(channel->changeAdminToClient(user))
		{
			// 	if(!arguments.empty())
			// 		arguments += " ";
			// arguments += user;
		}
	}
}

// void Server::MODE(std::string& cmd, int fd)
// {
// 	std::string channelName, params, modeset;

// 	std::string arguments = ":";
	
// 	Channel *channel;
	
// 	char opera = '\0';

// 	arguments.clear();
// 	Client *cli = getClient(fd);
// 	size_t found = cmd.find_first_not_of("MODEmode \t\v");
// 	if(found != std::string::npos)
// 		cmd = cmd.substr(found);
// 	else
// 	{
// 		sendResponse(ERR_NOTENOUGHPARAM(cli->getNickname()), fd); 
// 		return ;
// 	}
// 	getCmdArgs(cmd ,channelName, modeset , params);

// 	std::vector<std::string> tokens;
// 	// std::vector<std::string> tokens = splitParams(params);
// 	std::cout << "modeset: " << modeset << "\n";
// 	std::cout << "params: " << params << "\n";
// 	return;
// 	if(channelName[0] != '#' || !(channel = getChannel(channelName.substr(1))))
// 	{
// 		sendResponse(ERR_CHANNELNOTFOUND(cli->getUsername(),channelName), fd);
// 		return ;
// 	}
// 	else if (!channel->getClient(fd) && !channel->getAdmin(fd))
// 	{
// 		sendError(442, getClient(fd)->getNickname(), channelName, getClient(fd)->getFd(), " :You're not on that channel\r\n"); 
// 		return;
// 	}
// 	// response with the channel modes (MODE #channel)
// 	else if (modeset.empty()) 
// 	{
// 		sendResponse(RPL_CHANNELMODES(cli->getNickname(), channel->getName(), channel->getModes()) + \
// 		RPL_CREATIONTIME(cli->getNickname(), channel->getName(), channel->getCreationAt()), fd);
// 		return ;
// 	}
// 	// client is not channel operator
// 	else if (!channel->getAdmin(fd)) 
// 	{
// 		sendResponse(ERR_NOTOPERATOR(channel->getName()), fd);
// 		return ;
// 	}
// 	else if(channel)
// 	{
// 		size_t pos = 0;
// 		for(size_t i = 0; i < modeset.size(); i++)
// 		{
// 			if(modeset[i] == '+' || modeset[i] == '-')
// 				opera = modeset[i];
// 			else
// 			{
// 				//invite mode
// 				if(modeset[i] == 'i')
// 				{
					
// 					invite_only(channel , opera);
// 					//std::cout << "mode_chain: " << mode_chain.str() << "\n";
// 				}
// 				// else if (modeset[i] == 't') //topic restriction mode
// 				// 	mode_chain << topic_restriction(channel, opera, mode_chain.str());
// 				//password set/remove
// 				else if (modeset[i] == 'k')
// 				{
// 					password_mode(params, channel, opera, fd);
// 				}
// 				//set/remove user operator privilege
// 				else if (modeset[i] == 'o')
// 				{
// 					operator_privilege(params, channel, fd, opera, arguments);
// 					//std::cout << "mode_chain: " << mode_chain.str() << "\n";
// 				}
// 				// else if (modeset[i] == 'l') //set/remove channel limits
// 				// 	mode_chain << channel_limit(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
// 				// else
// 				// 	_sendResponse(ERR_UNKNOWNMODE(cli->GetNickName(), channel->GetName(),modeset[i]),fd);
// 			}
// 		}
// 	}
// 	// std::string chain = mode_chain.str();
// 	// if(chain.empty())
// 	// 	return ;
//  	channel->sendToAll(RPL_CHANGEMODE(cli->getHostname(), channel->getName(), modeset, params));
// }


void	Server::MODE(std::vector<std::string> tokens, int fd)
{
	std::string channelName, params, modeset;
	for (int i = 0; i < tokens.size(); i++)
	{
		std::cout << "token: " << tokens[i] << "\n";
	}

	// std::string arguments = ":";
	
	Channel *channel;
	
	char opera = '\0';

	// arguments.clear();
	Client *cli = getClient(fd);
	if (tokens.size() < 2)
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
	// // response with the channel modes (MODE #channel)
	else if (modeset.empty()) 
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
		size_t pos = 0;
		for(size_t i = 0; i < modeset.size(); i++)
		{
			if(modeset[i] == '+' || modeset[i] == '-')
				opera = modeset[i];
			else
			{
				//invite mode
				if(modeset[i] == 'i')
				{
					
					invite_only(channel , opera);
					//std::cout << "mode_chain: " << mode_chain.str() << "\n";
				}
				// else if (modeset[i] == 't') //topic restriction mode
				// 	mode_chain << topic_restriction(channel, opera, mode_chain.str());
				//password set/remove
				else if (modeset[i] == 'k')
				{
					password_mode(params, channel, opera, fd);
				}
				//set/remove user operator privilege
				else if (modeset[i] == 'o')
				{
					operator_privilege(params, channel, fd, opera);
					//std::cout << "mode_chain: " << mode_chain.str() << "\n";
				}
				// else if (modeset[i] == 'l') //set/remove channel limits
				// 	mode_chain << channel_limit(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				// else
				// 	_sendResponse(ERR_UNKNOWNMODE(cli->GetNickName(), channel->GetName(),modeset[i]),fd);
			}
		}
	}
	// // std::string chain = mode_chain.str();
	// // if(chain.empty())
	// // 	return ;
 	channel->sendToAll(RPL_CHANGEMODE(cli->getHostname(), channel->getName(), modeset, params));
}