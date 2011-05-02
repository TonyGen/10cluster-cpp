/** A set of machines running a routine.  */

#include <unistd.h>  // sleep
#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include <10util/util.h>
#include "cluster.h"
#include "run.h"
#include <execinfo.h>

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
		cout << "FAILURE: (" << typeid(e).name() << ") " << e.what() << endl;
		void *array[30];
		size_t size;
		size = backtrace (array, 30);  // get void*'s for all entries on the stack
		backtrace_symbols_fd (array, size, 2);  // print out all the frames to stderr
	}
}

/** Main procedure for controller, there should be just one controller in the network */
static int controllerMain (map <string, boost::shared_ptr<clusterRun::Routine> > routines, vector <string> args) {
	cout << "Controller running." << endl;
	pair< vector<remote::Host>, vector<remote::Host> > clientsServers = parseHosts (args[1]);
	cluster::members (clientsServers.first, clientsServers.second);
	runRoutine (routines, args[2]);
	cout << "End with Ctrl-c (SIGINT). Causes processes spawned on this machine to terminate as well." << endl;
	while (true) sleep (600);
}

/** Main procedure for worker machines in the network. This should run on every machine in the network except the controller. */
static int workerMain (vector <string> args) {
	cout << "Worker running." << endl;
	cout << "End with Ctrl-c (SIGINT). Causes processes spawned on this machine to terminate as well." << endl;
	while (true) sleep (600);
}

static void printUsage (string program, map <string, boost::shared_ptr<clusterRun::Routine> > routines) {
	cout << "usage: " << program << " RandomSeed MyHost (Worker | Controller)" << endl;
	cout << " where:" << endl;
	cout << "  RandomSeed = seed for random # generator. Same seed # will produce same sequence of random #s." << endl;
	cout << "   useful for reproducing the same sequence of actions" << endl;
	cout << "  MyHost = Hostname[:Port] to listen on. no port means use default port " << remote::DefaultPort << endl;
	cout << "  Worker = 'worker'" << endl;
	cout << "   a worker machine is as a client, server, or both." << endl;
	cout << "   run this on all worker machines except the controller machine." << endl;
	cout << "  Controller = 'controller' Host,Host,... Routine" << endl;
	cout << "   no space between hosts." << endl;
	cout << "   the controller spawns threads and processes on listed worker machines." << endl;
	cout << "   the controller machine may also be a worker machine." << endl;
	cout << "  Host = Hostname[:Port]['/'('c'|'s')]" << endl;
	cout << "   no Port means the default port " << remote::DefaultPort << endl;
	cout << "   'c' means client, 's' means server, neither means both" << endl;
	cout << "   eg: localhost,1.2.3.4:2222/c,foo.net/s" << endl;
	cout << "  Routine = name of routine to run, one of:" << endl;
	for (map <string, boost::shared_ptr<clusterRun::Routine> > :: iterator r = routines.begin(); r != routines.end(); ++r)
		cout << "   " << r->first << endl;
	cout << " examples:" << endl;
	cout << " host1> " << program << " 666 host1 worker" << endl;
	cout << " host2> " << program << " 666 host2 controller host1,host2 mongoTest::Foo" << endl;
}

int clusterRun::main (map <string, boost::shared_ptr<Routine> > routines, int argc, char* argv[]) {
	registerRoutineProcedures (routines);
	vector<string> args = argsVector (argc, argv);
	if (argc > 2) {
		srand (parse_string<int> (args[1]));
		network::HostPort hostPort = remote::hostPort (args[2]);
		network::initMyHostname (hostPort.hostname);
		cluster::listen (hostPort.port);
		args.erase (args.begin(), args.begin() + 3);
		if (args.size() == 1 && args[0] == "worker") return workerMain (args);
		if (args.size() == 3 && args[0] == "controller") return controllerMain (routines, args);
	}
	printUsage (argv[0], routines);
	return -1;
}

