#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Server.hpp"

class Client;
class Channel
{
	private:
		int invit_only;
		int topic;
		int key;
		int limit;
		bool topic_restriction;
		std::string name;
		std::string time_creation;
		std::string password;
		std::string created_at;
		std::string topic_name;
		std::vector<Client> clients;
		std::vector<Client> admins;
		std::vector<std::pair<char, bool> > modes;
	public:
		Channel();
		~Channel();
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);


		//---------------//setters
	void setInvitOnly(int invit_only);
	void setTopic(int topic);
	void setKey(int key);
	void setLimit(int limit);
	void setTopicName(std::string topic_name);
	void setPassword(std::string password);
	void setName(std::string name);
	void setCreateAt();
	void setModeAtIndex(size_t index, bool mode);
	void setTime();
	void setTopicRestriction(bool value);


	//---------------//Getters
	int getInvitOnly();
	int getTopic();
	int getKey();
	int getLimit();
	int getClientsNumber();

	size_t	getAdmins();

	bool getModeAtIndex(size_t index);
	bool isClientInChannel(std::string &nickname);
	bool getTopicRestriction();

	std::string getTopicname();
	std::string getPassword();
	std::string getName();
	std::string getTime();
	std::string getCreationAt();
	std::string getClientsInChannel();
	std::string getModes();
	std::string getFirstNickname();
	
	Client *getClient(int fd);
	Client *getAdmin(int fd);

	Client* getClientInChannel(std::string nickname);
	
	//---------------//Methods
	void addClient(Client newClient);
	void addAdmin(Client newClient);
	void removeClient(int fd);
	void removeAdmin(int fd);
	bool changeClientToAdmin(std::string& nickname);
	bool changeAdminToClient(std::string& nickname);

	//---------------//SendToAll
	void sendToAll(std::string rpl1);
	void sendToAll(std::string rpl1, int fd);

};

#endif
