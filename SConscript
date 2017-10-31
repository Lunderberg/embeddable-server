Import('env')

if ARGUMENTS.get('ENABLE_SSL', 0):
    env.Append(LIBS=['ssl', 'crypto'],
               CPPDEFINES=['EWEB_ENABLE_SSL'])


if ARGUMENTS.get('LIBBOOST', 0):
    requires = []
    env.Append(CPPPATH=['/usr/include/boost'],
               LIBS=['boost_system'])
else:
    requires = ['asio']

env.CompileFolderDWIM('.', requires=requires)
