Import('env')

if ARGUMENTS.get('ENABLE_SSL', 0):
    env.Append(LIBS=['ssl', 'crypto'],
               CPPDEFINES=['EWEB_ENABLE_SSL'])

env.CompileFolderDWIM('.', requires='asio')
