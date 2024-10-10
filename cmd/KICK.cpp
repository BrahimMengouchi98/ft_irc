#include "../inc/Server.hpp"

void	splitParamsByComma(std::vector<std::string> &token, std::string tokens)
{
	std::string buff;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == ',')
		{
			token.push_back(buff); 
			buff.clear();
		}
		else 
			buff += tokens[i];
	}
	// After the loop, check if there's a final chunk in 'buff'
    if (!buff.empty()) 
    {
        token.push_back(buff);  // Push the last chunk into the vector
    }
}

// kick #ch1,#ch2 user1,user2
void	Server::KICK(std::vector<std::string> tokens, int fd)
{
    std::vector<std::string> channelsName;
    std::vector<std::string> usersName;

	// // ERR_NEEDMOREPARAMS (461) // if the channel name is empty
	if (tokens.size() < 3)
	{
		sendError(461, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :Not enough parameters\r\n"); 
		return;
	}

    splitParamsByComma(channelsName, tokens[1]);
    splitParamsByComma(usersName, tokens[2]);
	for (size_t i = 0; i < channelsName.size(); i++)
	{
		if (channelsName[i][0] == '#')
			channelsName[i] = channelsName[i].substr(1);
	}
	
	std::string reason;
	if (tokens.size() > 3)
	{
		for (size_t i = 3; i < tokens.size(); i++)
		{
			reason += tokens[i];
			if (i < tokens.size() - 1)
				reason += " ";
		}	
	}
	
	// search for the channel
	for (size_t i = 0; i < channelsName.size(); i++)
	{
		for (size_t j = 0; j < this->channels.size(); j++)
    	{ 
			// check if the channel exist
			if (this->channels[j].getName() == channelsName[i])
			{ 
                Channel *ch = getChannel(this->channels[j].getName());
				// ERR_NOTONCHANNEL (442) // if the client is not in the channel
				if (!channels[j].getClient(fd) && !channels[j].getAdmin(fd)) 
				{
					sendError(442, getClient(fd)->getNickname(), "#" + channelsName[i], getClient(fd)->getFd(), " :You're not on that channel\r\n"); 
					continue;
				}
                // check if the client is admin
                if(ch->getAdmin(fd))
                {
                    for (size_t k = 0; k < usersName.size(); k++)
                    {
                        // check if the client to kick is in the channel
                        if (ch->getClientInChannel(usersName[k]))
                        { 
                            std::stringstream ss;
                            ss << ":" << getClient(fd)->getNickname() << "!~" << getClient(fd)->getUsername() << "@" << "localhost" << " KICK #" << channelsName[i] << " " << usersName[k];
                            if (!reason.empty())
                                ss << " :" << reason << "\r\n";
                            else ss << "\r\n";
                            ch->sendToAll(ss.str());
                            if (ch->getAdmin(ch->getClientInChannel(usersName[k])->getFd()))
                                ch->removeAdmin(ch->getClientInChannel(usersName[k])->getFd());
                            else
                                ch->removeClient(ch->getClientInChannel(usersName[k])->getFd());
                            if (ch->getClientsNumber() == 0)
                                channels.erase(channels.begin() + i);
                        }
                        else // if the client to kick is not in the channel
					    {
                            sendError(441, getClient(fd)->getNickname(), "#" + channelsName[i], getClient(fd)->getFd(), " :He is not on that channel\r\n"); 
                            continue;
                        }
                    }
                }
        		else // if the client is not admin
				{
                    sendError(482, getClient(fd)->getNickname(), "#" + channelsName[i], getClient(fd)->getFd(), " :You're not channel operator\r\n");
                    continue;
                }
			}
            else // if the channel doesn't exist
			    sendError(403, getClient(fd)->getNickname(), "#" + channelsName[i], getClient(fd)->getFd(), " :No such channel\r\n");
		}
	}
}
