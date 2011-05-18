libname = 'cluster'

lib = SharedLibrary (libname, Glob('*.cpp'),
	CCFLAGS = ['-g'],
	CPPPATH = ['.', '/opt/local/include'],
	LIBPATH = ['/opt/local/lib'],
	LIBS = Split ('10util remote boost_serialization-mt') )

Alias ('install', '/usr/local')
Install ('/usr/local/lib', lib)
Install ('/usr/local/include/' + libname, Glob('*.h'))
