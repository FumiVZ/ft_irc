#include <Commands.hpp>

bool isOperator(Server &server, int clientSocket, const std::string &channel_name)
{
	return server.getChannel(channel_name)->isOp(server.getClient(clientSocket));
}

bool isNumeric(const std::string &str)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	if (str.length() > 3)
		return false;
	return true;
}

/*
 MODE <channel>  <mode> <+/-> <~nickname>
— t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
rateurs de canaux
— k : Définir/supprimer la clé du canal (mot de passe)
— o : Donner/retirer le privilège de l’opérateur de canal
— l : Définir/supprimer la limite d’utilisateurs pour le canal
*/
void mode(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() == 1)
	{
		Channel *channel = server.getChannel(message.getParameters()[0]);
		if (channel == NULL)
		{
			client.sendReply("403",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
			return;
		}
		client.sendReply("324", "RPL_CHANNELMODEIS " + channel->getName() + " +" + channel->getModes());
		return;
	}
	if (message.getParameters().size() < 2)
	{
		client.sendReply("461",(client.getNickname().empty() ? "* " : client.getUsername()) + " MODE :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	upcase(channel_name);
	std::string mode_op = message.getParameters()[1];
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		client.sendReply("403",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (mode_op != "+i" && mode_op != "-i" && mode_op != "+t" && mode_op != "-t" && mode_op != "+k" && mode_op != "-k" && mode_op != "+o" && mode_op != "-o" && mode_op != "+l" && mode_op != "-l")
	{
		client.sendReply("472",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + mode_op + " :" + ERR_UNKNOWNMODE);
		return;
	}
	if (mode_op == "+t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " +t");
		channel->addMode('t');
		return ;
	}
	else if (mode_op == "-t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " -t");
		channel->removeMode('t');
		return ;
	}
	else if (mode_op == "-i")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " -i");
		channel->removeMode('i');
	}
	else if (mode_op == "+i")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->addMode('i');
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " +i");
		return ;
	}
	else if (mode_op == "-k")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " -k");
		channel->setPasswd("");
		return ;
	}
	else if (mode_op == "-l")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " -l");
		channel->setLimit(0);
		return ;
	}
	if (message.getParameters().size() < 3)
	{
		client.sendReply("461",(client.getNickname().empty() ? "* " : client.getUsername()) + " MODE :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string argument = message.getParameters()[2];
	if (mode_op == "+k")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " +k " + argument);
		channel->setPasswd(argument);
	}
	else if (mode_op == "+o")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		try
		{
			Client &op = server.getClient(argument);
			channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " +o " + argument);
			channel->addOp(op);
		}
		catch (const std::exception &e)
		{
			client.sendReply("401",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + argument + " :" + ERR_NOSUCHNICK);
		}
	}
	else if (mode_op == "-o")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		try
		{
			Client &op = server.getClient(argument);
			channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " -o " + argument);
			channel->removeOp(op);
		}
		catch (const std::exception &e)
		{
			client.sendReply("401",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + argument + " :" + ERR_NOSUCHNICK);
		}
	}
	else if (mode_op == "+l")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		if (!isNumeric(argument))
		{
			client.sendReply("461",(client.getNickname().empty() ? "* " : client.getUsername()) + " MODE :" + ERR_WRONGPARAMCOUNT);
			return;
		}
		int limit;
		std::istringstream(argument) >> limit;
		if (limit <= 0)
		{
			client.sendReply("461",(client.getNickname().empty() ? "* " : client.getUsername()) + " MODE :" + ERR_WRONGPARAMCOUNT);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), " MODE " + channel->getName() + " +l " + argument);
		channel->setLimit(limit);
	}
}