/** A set of machines running a routine.  */

#include <unistd.h>  // sleep
#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include <util/util.h>
#include "cluster.h"
#include "run.h"

using namespace std;

static void registerRoutineProcedures (map <string, boost::shared_ptr<clusterRun::Routine> > routines) {
	for (map <string, boost::shared_ptr<clusterRun::Routine> > :: iterator r = routines.begin(); r != routines.end(); ++r)
		r->second->registerProcedures();
}

/** Parse hosts, returning (clients, servers). 'host/c' is a client, 'host/s' is server, and 'host' are both */
static pair< vector<remote::Host>, vector<remote::Host> > parseHosts (string hostsString) {
	vector<remote::Host> clients;
	vector<remote::Host> servers;
	vector<string> hosts;
	boost::split (hosts, hostsString, boost::is_any_of (","));
	for (unsigned i = 0; i < hosts.size(); i ++) {
		vector<string> parts;
		boost::split (parts, hosts[i], boost::is_any_of ("/"));
		if (parts.size() == 2) {
			if (parts[1] == "c") clients.push_back (parts[0]);
			else if (parts[1] == "s") servers.push_back (parts[0]);
			else throw std::runtime_error ("expected /c or /h or nothing after host: " + parts[0]);
		} else if (parts.size() == 1) {
			clients.push_back (parts[0]);
			servers.push_back (parts[0]);
		} else throw runtime_error ("bad host: " + hosts[i]);
	}
	return make_pair (clients, servers);
}

/** Parse integer out of given string */
static int parseInt (string s) {
	stringstream ss (s);
	int num;
	if ((ss >> num).fail()) throw runtime_error ("expected " + s + " to be an integer");
	return num;
}

/** Run named routine. If it fails print the failure, otherwise print success */
static void runRoutine (map <string, boost::shared_ptr<clusterRun::Routine> > routines, string routineName) {
	try {
		map <string, boost::shared_ptr<clusterRun::Routine> > :: iterator routine = routines.find (routineName);
		if (routine == routines.end()) {
			cout << "Routine " << routineName << " not found. Available routines are:" << endl;
			for (map <string, boost::shared_ptr<clusterRun::Routine> > :: iterator r = routines.begin(); r != routines.end(); ++r)
				cout << "  " << r->first << endl;
			exit (-1);
		}
		cout << routineName << " running. Wait for 'SUCCESS' or 'FAILURE' to be printed." << endl;
		(* routine->second) ();
		cout << "SUCCESS." << endl;
	} catch (exception &e) {
		cout << "FAILURE: " << e.what() << endl;
	}
}

/** Main procedure for controller, there should be just one controller in the network */
static int controllerMain (map <string, boost::shared_ptr<clusterRun::Routine> > routines, vector <string> args) {
	srand (parseInt (args[1]));
	cout << "Controller running." << endl;
	pair< vector<remote::Host>, vector<remote::Host> > clientsServers = parseHosts (args[3]);
	cluster::members (clientsServers.first, clientsServers.second);
	runRoutine (routines, args[4]);
	cout << "End with Ctrl-c (SIGINT). Causes processes spawned on this machine to terminate as well." << endl;
	while (true) sleep (600);
}

/** Main procedure for worker machines in the network. This should run on every machine in the network except the controller. */
static int workerMain (vector <string> args) {
	srand (parseInt (args[1]));
	cout << "Worker running." << endl;
	cout << "End with Ctrl-c (SIGINT). Causes processes spawned on this machine to terminate as well." << endl;
	while (true) sleep (600);
}

static void printUsage (string program, map <string, boost::shared_ptr<clusterRun::Routine> > routines) {
	cout << "usage: " << program << " Seed (Controller | Worker)" << endl;
	cout << " where:" << endl;
	cout << "  Seed = seed for random # generator. Same seed # will produce same sequence of random #s." << endl;
	cout << "   useful for reproducing the same sequence of actions" << endl;
	cout << "  Controller = controller Host,Host,... Routine" << endl;
	cout << "   the controller spawns threads and processes on listed worker machines." << endl;
	cout << "   the controller machine may also be a worker machine. no space between hosts." << endl;
	cout << "  Worker = worker" << endl;
	cout << "   a worker machine is as a client, server, or both." << endl;
	cout << "   run this on all worker machines except the controller machine." << endl;
	cout << "  Host = Hostname[/(c|s)]" << endl;
	cout << "   c means client, s means server, neither means both" << endl;
	cout << "   eg: localhost,1.2.3.4/c,foo.net/s" << endl;
	cout << "  Routine = name of routine to run, one of:" << endl;
	for (map <string, boost::shared_ptr<clusterRun::Routine> > :: iterator r = routines.begin(); r != routines.end(); ++r)
		cout << "   " << r->first << endl;
}

int clusterRun::main (map <string, boost::shared_ptr<Routine> > routines, int argc, char* argv[]) {
	registerRoutineProcedures (routines);
	cluster::listen();
	vector<string> args = argsVector (argc, argv);
	if (argc == 5 && args[2] == "controller") return controllerMain (routines, args);
	if (argc == 3 && args[2] == "worker") return workerMain (args);
	printUsage (argv[0], routines);
	return -1;
}

