# Remove the build directory when cleaning
Clean('.', 'build')

# Where to put all executables
exec_dir = Dir('#/run')
Export('exec_dir')

build_dir = Dir('#/build')

env = Environment(tools=['default', 'textfile'])

# C++ compiler
# BOOST_UBLAS_NDEBUG turns off uBLAS debugging (very slow)
env.MergeFlags('-O2 -g3 -Wall -DBOOST_UBLAS_NDEBUG')
env.Append(CPPPATH = [Dir('#/common')])

# Variables
# Can't put this in build_dir because scons wants to read it before build_dir exists.
# I don't want to create build_dir for the sole purpose of hiding that error...
var_file = File('#/.scons_variables').abspath
vv = Variables(var_file)
vv.AddVariables(
	BoolVariable('profile', 'build for profiling', False)
)
vv.Update(env)
vv.Save(var_file, env)
Help(vv.GenerateHelpText(env))

# Profiling
if env['profile']:
	print '*** Profiling enabled'
	env.Append(CPPFLAGS='-pg ', LINKFLAGS='-pg ')

# Qt
env['QT4DIR'] = '/usr'
env.Tool('qt4')
env.EnableQt4Modules(['QtCore', 'QtGui', 'QtNetwork', 'QtXml', 'QtOpenGL'])

# All executables need to link with the common library, which depends on protobuf
env.Append(LIBS=['common', 'protobuf'])

# http://www.scons.org/wiki/GoFastButton
env.Decider('MD5-timestamp')
SetOption('max_drift', 1)
SetOption('implicit_cache', 1)
env.SourceCode(".", None)

# Make a new environment for code that must be 32-bit
env32 = env.Clone()

# Search paths for native code
env.Append(LIBPATH=[build_dir.Dir('common')])
env.Append(CPPPATH=[build_dir.Dir('common')])

# Hack up some compatibility on 64-bit systems
import platform
if platform.machine() == 'x86_64':
	f = file('/etc/issue')
	issue = f.readlines()[0].split()
	f.close()

	if issue[0] == 'Ubuntu':
		if issue[1].startswith('10.04'):
			compat_dir = Dir('#/SoccSim/lib/32-on-64/lucid')
			protobuf_lib = compat_dir.File('libprotobuf.so.5')
		elif issue[1] == '10.10':
			compat_dir = Dir('#/SoccSim/lib/32-on-64/maverick')
			protobuf_lib = compat_dir.File('libprotobuf.so.6')
		else:
			raise '32-bit compatibility: Unsupported version of Ubuntu'
		env32.Install(exec_dir, protobuf_lib)
	else:
		raise '32-bit compatibility only works on Ubuntu'

	# SoccSim must build 32-bit code, even on a 64-bit system, because PhysX is only
	# available as 32-bit binaries.
	env32.Append(CPPFLAGS='-m32')
	env32.Append(LINKFLAGS='-m32')
	env32.Append(LIBPATH=[build_dir.Dir('common32'), compat_dir])
	env32.Append(CPPPATH=[build_dir.Dir('common32')])

	# Build a 32-bit version of common for SoccSim.
	Export({'env': env32, 'cross_32bit': True})
	SConscript('common/SConscript', exports={'env':env32, 'cross_32bit':True}, variant_dir=build_dir.Dir('common32'), duplicate=0)
else:
	# This is a 32-bit system, so we only need one version of common
	env32 = env

# PhysX libraries will be copied to the run directory.
# Use rpath to tell the dynamic linker where to find them.
env32.Append(RPATH=[Literal('\\$$ORIGIN')])

# Use SConscripts
def do_build(dir, exports={}):
	SConscript(dir + '/SConscript', exports=exports, variant_dir=build_dir.Dir(dir), duplicate=0)

Export({'env': env, 'cross_32bit': False})
do_build('common')

Export({'env': env32, 'cross_32bit': True})
do_build('SoccSim', {'env': env32})

Export({'env': env, 'cross_32bit': False})
for dir in ['logging', 'radio', 'soccer']:
	do_build(dir)

# Build sslrefbox with its original makefile (no dependency checking)
env.Command('sslrefbox/sslrefbox', 'sslrefbox/Makefile', 'make -C sslrefbox')
env.Install(exec_dir, 'sslrefbox/sslrefbox')
