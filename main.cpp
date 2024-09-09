# include "inc/Server.hpp"

int main(int ac, char **av)
{
	Server ser;
	std::cout << "---- SERVER ----" << std::endl;
	try
	{
		signal(SIGINT, Server::SignalHandler); //-> catch the signal (ctrl + c)
		signal(SIGQUIT, Server::SignalHandler); //-> catch the signal (ctrl + \)
		ser.init(std::atoi(av[1]), av[2]); //-> initialize the server
	}
	catch(const std::exception& e)
	{
		ser.CloseFds(); //-> close the file descriptors
		std::cerr << e.what() << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
}