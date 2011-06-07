/* Register set of machines in my cluster. */

#pragma once

#include <vector>
#include <remote/remote.h>
#include <10util/library.h>

namespace cluster {

/** Client machines in cluster. A machine may be both a client and server machine */
extern std::vector<remote::Host> clients;

/** Server machines in cluster. A machine may be both a client and server machine */
extern std::vector<remote::Host> servers;

/** All machines in cluster (clients and servers), removing duplicates */
std::set<remote::Host> machines();

/** Broadcast the set of machines in the cluster to all the machines so they know about each other */
void members (std::vector<remote::Host> clients, std::vector<remote::Host> servers);

/** Tell all machines in cluster to load given library */
void load (library::Libname libname);

/** Tell all machines in cluster to reset its random number generator with given seed */
void seedRandom (int seed);

/** Return the next cluster server in cycle */
remote::Host someServer();

/** Return the next N cluster servers in cycle */
std::vector<remote::Host> someServers (unsigned n);

/** Return the next cluster client in cycle */
remote::Host someClient();

/** Return the next N cluster clients in cycle */
std::vector<remote::Host> someClients (unsigned n);

}

namespace _cluster {

extern remote::Module module;
void setMembers (std::vector<remote::Host> clients, std::vector<remote::Host> servers);
void load (library::Libname);
void setRandomSeed (int seed);

}
