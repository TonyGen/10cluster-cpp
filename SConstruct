libname = 'cluster'

lib = SharedLibrary (libname, ['cluster.cpp'],
	CCFLAGS = ['-g'],
	CPPPATH = ['.', '/opt/local/include'],
	LIBPATH = ['/opt/local/lib'],
	LIBS = Split ('10util remote boost_serialization-mt') )

prog = Program (libname, ['main.cpp'],
	CCFLAGS = ['-g'],
	CPPPATH = ['.', '/opt/local/include'],
	LIBPATH = ['.', '/opt/local/lib'],
	LIBS = [libname, '10util', 'remote', 'boost_serialization-mt', 'readline'] )

Alias ('install', '/usr/local')
Install ('/usr/local/lib', lib)
Install ('/usr/local/include/' + libname, Glob('*.h'))
Install ('/usr/local/bin', prog)
