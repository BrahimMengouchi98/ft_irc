# include "inc/Server.hpp"

int main(int ac, char **av)
{
	Server ser;
	std::cout << "---- SERVER ----" << std::endl;
	if (ac != 3)
	{
		std::cout << "Usage: " << av[0] << " <port number> <password>" << std::endl; 
		return 1;
	}
	try
	{
		//-> catch the signal (ctrl + c)
		signal(SIGINT, Server::signalHandler); 
		if(!ser.isPortValid(av[1]) || !*av[2] || std::strlen(av[2]) > 20)
		{
			std::cout << "invalid Port number / Password!" << std::endl; 
			return 1;
		}
		ser.init(std::atoi(av[1]), av[2]); //-> initialize the server
	}
	catch(const std::exception& e)
	{
		ser.closeFds(); //-> close the file descriptors
		std::cerr << e.what() << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
}
