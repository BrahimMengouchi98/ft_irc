# include "../inc/Server.hpp"

void extractMessage(std::string cmd, std::string tofind, std::string &str)
{
	size_t i = 0;
	for (; i < cmd.size(); i++)
	{
		if (cmd[i] != ' ')
		{
			std::string tmp;
			for (; i < cmd.size() && cmd[i] != ' '; i++)
				tmp += cmd[i];
			if (tmp == tofind) 
				break;
			else tmp.clear();
		}
	}
	if (i < cmd.size()) 
		str = cmd.substr(i);
	i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	str = str.substr(i);
}

std::string SplitCmdPM(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, msg;
	int count = 2;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 2) 
		return std::string("");
	//std::cout << "tmp[1]: " << tmp[1] << "\n";
	extractMessage(cmd, tmp[1], msg);
	return msg;
}

std::string SplitCmdPrivmsg(std::string cmd, std::vector<std::string> &tmp)
{
	std::string str = SplitCmdPM(cmd, tmp);
	if (tmp.size() != 2) 
	{
		tmp.clear(); 
		return std::string("");
	}
	tmp.erase(tmp.begin());
	std::string str1 = tmp[0]; 
	std::string str2; 
	tmp.clear();
	for (size_t i = 0; i < str1.size(); i++){//split the first string by ',' to get the channels names
		if (str1[i] == ',')
			{tmp.push_back(str2); str2.clear();}
		else str2 += str1[i];
	}
	tmp.push_back(str2);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	if (str[0] == ':') str.erase(str.begin());
	else //shrink to the first space
		{for (size_t i = 0; i < str.size(); i++){if (str[i] == ' '){str = str.substr(0, i);break;}}}
	return  str;
	
}

void 	Server::PRIVMSG(std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string message = SplitCmdPrivmsg(cmd, tmp);
	for (int i = 0; i < tmp.size(); i++)
	{
		std::cout << "aa: " << tmp[i] << "\n";
	}
	std::cout << "msg: " << message << "\n";
	// if the client doesn't specify the recipient
	if (!tmp.size())//ERR_NORECIPIENT (411) 
	{
		std::cout << "size = 0\n";
		// sendError(411, getClient(fd)->getNickname(), getClient(fd)->getFd(), " :No recipient given (PRIVMSG)\r\n"); 
		sendResponse(ERR_NORECIPIENT(getClient(fd)->getNickname()), fd);
		return;
	}
	// if (message.empty())//ERR_NOTEXTTOSEND (412) // if the client doesn't specify the message
	// 	{senderror(412, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No text to send\r\n"); return;}
	// if (tmp.size() > 10) //ERR_TOOMANYTARGETS (407) // if the client send the message to more than 10 clients
	// 	{senderror(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many recipients\r\n"); return;}
	// CheckForChannels_Clients(tmp, fd); // check if the channels and clients exist
	// for (size_t i = 0; i < tmp.size(); i++){// send the message to the clients and channels
	// 	if (tmp[i][0] == '#'){
	// 		tmp[i].erase(tmp[i].begin());
	// 		std::string resp = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost PRIVMSG #" + tmp[i] + " :" + message + "\r\n";
	// 		GetChannel(tmp[i])->sendTo_all(resp, fd);
	// 	}
	// 	else{
	// 		std::string resp = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost PRIVMSG " + tmp[i] + " :" + message + "\r\n";
	// 		_sendResponse(resp, GetClientNick(tmp[i])->GetFd());
	// 	}
	// }
}