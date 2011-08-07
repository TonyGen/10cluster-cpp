/* Register set of machines in my cluster. */

#pragma once

#include <vector>
#include <10remote/remote.h>

namespace cluster {

enum Role {CLIENT, SERVER, BOTH};

struct Member {
	remote::Host host;
	Role role;
	Member (remote::Host host, Role role) : host(host), role(role) {}
	Member () {} // for serialization
	bool isClient () {return role == CLIENT || role == BOTH;}
	bool isServer () {return role == SERVER || role == BOTH;}
};

/** All machines in cluster including self. Each machine may have a client and/or server role. */
extern std::vector<Member> members;

/** Join cluster of machines where given host is one of them. Join as given role. I must already be listening (remote::listen) */
void join (Role, remote::Host);

/** Join cluster where I am the first one or others have already joined to me so I know who to tell. I must already be listening (remote::listen) */
void join (Role);

/** Join cluster of machines where given host is one of them. Join as BOTH client and server. I must already be listening (remote::listen) */
inline void join (remote::Host host) {join (BOTH, host);}

/** Join cluster where I am the first one or others have already joined to me so I know who to tell. Join as BOTH client and server. I must already be listening (remote::listen) */
inline void join () {join (BOTH);}

/** Remove self from cluster of machines */
void leave ();

/** Tell all machines in cluster to reset its random number generator with given seed */
void seedRandom (int seed);

/** Return all hosts in cluster whether client or server or both */
std::vector<remote::Host> hosts ();

/** Return all clients in cluster */
std::vector<remote::Host> clients ();

/** Return all servers in cluster */
std::vector<remote::Host> servers ();

/** Return the next client in cluster cycle */
remote::Host someClient ();

/** Return the next N clients in cluster cycle */
std::vector<remote::Host> someClients (unsigned n);

/** Return the next server in cluster cycle */
remote::Host someServer ();

/** Return the next N servers in cluster cycle */
std::vector<remote::Host> someServers (unsigned n);

}

inline std::ostream& operator<< (std::ostream& out, const cluster::Member& x) {
	out << x.host;
	if (x.role == cluster::CLIENT) out << "/c";
	if (x.role == cluster::SERVER) out << "/s";
	return out;
}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, cluster::Member & x, const unsigned version) {
	ar & x.host;
	ar & x.role;
}

}}

namespace _cluster {

extern module::Module module;
void newMember (cluster::Member);
void addMember (cluster::Member);
void removeMember (remote::Host);
void setRandomSeed (int seed);

}
