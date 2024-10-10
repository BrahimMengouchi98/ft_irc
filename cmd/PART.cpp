#include "../inc/Server.hpp"

void	splitChannels(std::vector<std::string> &token, std::string tokens)
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


void	Server::PART(std::vector<std::string> tokens, int fd)
{
    std::vector<std::string> channelsName;
		
    splitChannels(channelsName, tokens[1]);
	
	// // ERR_NEEDMOREPARAMS (461) // if the channel name is empty
	if (tokens.size() < 2)
	{
		sendError(461, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :Not enough parameters\r\n"); 
		return;
	}
	for (size_t i = 0; i < channelsName.size(); i++)
	{
		if (channelsName[i][0] == '#')
			channelsName[i] = channelsName[i].substr(1);
	}
	
	std::string reason;
	if (tokens.size() > 2)
	{
		for (size_t i = 2; i < tokens.size(); i++)
		{
			reason += tokens[i];
			if (i < tokens.size() - 1)
				reason += " ";
		}	
	}
	
	// search for the channel
	for (size_t i = 0; i < channelsName.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++)
		{ 
			// check if the channel exist
			if (this->channels[j].getName() == channelsName[i])
			{ 
				flag = true;
				// ERR_NOTONCHANNEL (442) // if the client is not in the channel
				if (!channels[j].getClient(fd) && !channels[j].getAdmin(fd)) 
				{
					sendError(442, getClient(fd)->getNickname(), "#" + channelsName[i], getClient(fd)->getFd(), " :You're not on that channel\r\n"); 
					continue;
				}
				std::stringstream ss;
				ss << ":" << getClient(fd)->getNickname() << "!~" << getClient(fd)->getUsername() << "@" << "localhost" << " PART #" << channelsName[i];
				if (!reason.empty())
					ss << " :" << reason << "\r\n";
				else 
					ss << "\r\n";
				channels[j].sendToAll(ss.str());
				if (channels[j].getAdmin(channels[j].getClientInChannel(getClient(fd)->getNickname())->getFd()))
					channels[j].removeAdmin(channels[j].getClientInChannel(getClient(fd)->getNickname())->getFd());
				else
					channels[j].removeClient(channels[j].getClientInChannel(getClient(fd)->getNickname())->getFd());
				if (channels[j].getClientsNumber() == 0)
					removeChannel(channels[j].getName());
					// channels.erase(channels.begin() + i);
			}
		}
		// ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
		if (!flag) 
			sendError(403, getClient(fd)->getNickname(), channelsName[i], getClient(fd)->getFd(), " :No such channel\r\n");
	}
}
