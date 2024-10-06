# include "../inc/Server.hpp"

// QUIT CMD
void	Server::QUIT(std::vector<std::string> tokens, int fd)
{
	std::string reason;
	if (tokens.size() > 1)
		reason = tokens[1];
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i].getClient(fd))
		{
			channels[i].removeClient(fd);
			if (channels[i].getClientsNumber() == 0)
				channels.erase(channels.begin() + i);
			else
			{
				std::string rpl = ":" + getClient(fd)->getNickname() + "!~" + getClient(fd)->getUsername() + "@localhost QUIT " + reason + "\r\n";
				channels[i].sendToAll(rpl);
			}
		}
		else if (channels[i].getAdmin(fd))
		{
			channels[i].removeAdmin(fd);
			if (channels[i].getClientsNumber() == 0)
				channels.erase(channels.begin() + i);
			else
			{
				std::string rpl = ":" + getClient(fd)->getNickname() + "!~" + getClient(fd)->getUsername() + "@localhost QUIT " + reason + "\r\n";
				channels[i].sendToAll(rpl);
			}
		}
	}
	std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
	removeFd(fd);
	close(fd);
}
