libname = '10cluster'

lib = SharedLibrary (libname, Glob('*.cpp'),
	CCFLAGS = ['-g', '-rdynamic'],
	CPPPATH = ['.', '/usr/local/include'],
	LIBPATH = ['/usr/local/lib'],
	LIBS = Split ('10util 10remote boost_serialization-mt') )

Alias ('install', '/usr/local')
Install ('/usr/local/lib', lib)
Install ('/usr/local/include/' + libname, Glob('*.h'))
