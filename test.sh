#!/bin/bash
# Connexion au serveur IRC
{
    printf "PASS password\n"
	printf "\rNICK user\n"
	printf "\rUSER user 0 * :realname\n"

} | nc localhost 6667