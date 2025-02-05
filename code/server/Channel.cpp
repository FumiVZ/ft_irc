#include <Channel.hpp>

Channel::Channel(std::string name, Client &owner) : name(name), topic("No topic is set")
{
	this->ops.push_back(owner);
	this->clients.push_back(owner);
	this->modes.push_back('t');
	this->password = "";
	this->limit = 0;
}
Channel::~Channel()
{
}

std::string Channel::getName() { return this->name; };
std::vector<Client> &Channel::getClients() { return this->clients; };
std::vector<Client> &Channel::getOps() { return this->ops; };
void Channel::addOp(Client &op) { this->ops.push_back(op); };
void Channel::removeOp(Client &op)
{
	for (std::vector<Client>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (*it == op)
		{
			ops.erase(it);
			break;
		}
	}
}

std::string Channel::getModes()
{
	std::string result;
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
	{
		result += *it;
		result += " ";
	}
	return result;
}

bool Channel::isOp(Client &op)
{
	for (std::vector<Client>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (*it == op)
			return true;
	}
	return false;
}
bool Channel::isClient(Client &c)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == c)
			return true;
	}
	return false;
}


void Channel::removeClient(Client &c)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == c)
		{
			clients.erase(it);
			break;
		}
	}
}

void Channel::broadcast(Client &c, std::string msg)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == c)
			continue;
		it->forwardMessage(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHostname() + msg + "\r\n");
	}
}

// MODE <channel>  <mode> <+/-> <~nickname>
/*— t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
rateurs de canaux
— k : Définir/supprimer la clé du canal (mot de passe)
— o : Donner/retirer le privilège de l’opérateur de canal
— l : Définir/supprimer la limite d’utilisateurs pour le canal*/
void Channel::addClient(Client &c)
{
	if (clients.size() < limit  || limit == 0)
		clients.push_back(c);
	else
		c.sendReply("471", "Channel is full");
}
std::string Channel::getTopic() { return this->topic; }

void Channel::setPasswd(std::string passwd) { this->password = passwd; }
std::string Channel::getPasswd() { return this->password; }
void Channel::setTopic(std::string topic) { this->topic = topic; }

void Channel::setLimit(size_t limit){ this->limit = limit; }
size_t Channel::getLimit(){ return this->limit; }
void Channel::addMode(char mode)
{
	this->modes.push_back(mode);
}

void Channel::removeMode(char mode)
{
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
	{
		if (*it == mode)
		{
			modes.erase(it);
			break;
		}
	}
}

void Channel::ChannelIsTrue(char mode)
{
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
	{
		if (*it == mode)
			return;
	}
}


bool Channel::getTopicIsTrue()
{
	return this->topicIsTrue;
}
