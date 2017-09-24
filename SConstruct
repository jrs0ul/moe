import os
import sys

OBJ_DIR = os.path.join(os.getcwd(), 'obj/');
env = Environment(ENV=os.environ,
                  CCFLAGS=['-O3','-Wall'],
                  OBJPREFIX = OBJ_DIR)


code = ["src/mainer.cpp",
        "src/Xml.cpp",
        "src/gui/Text.cpp",
        "src/TextureLoader.cpp",
        "src/Image.cpp",
        "src/Vectors.cpp",
        "src/gui/Button.cpp",
        "src/Utils.cpp",
        "src/Extensions.cpp",
        "src/Matrix.cpp",
        "src/CppSingleton.cpp",
        "src/audio/OggStream.cpp",
        "src/LevelMap.cpp",
        "src/Creature.cpp",
        "src/audio/SoundSystem.cpp",
        "src/externals.cpp",
        "src/SysConfig.cpp",
        "src/SDLVideo.cpp",
        "src/Threads.cpp",
        "src/Population.cpp",
        "src/Meteor.cpp"

        ]

#Windblows
if sys.platform == 'win32':
    Tool('mingw')(env)
    env.Append(LIBS = ['mingw32','SDLmain', 'SDL', 'OpenGL32',
                       'openal32', 'vorbisfile', 'ogg'])
else: #Mac OS X
    if sys.platform == 'darwin':
        env['FRAMEWORKS'] = ['Cocoa', 'SDL', 'OPENGL', 'OpenAL','vorbis', 'Ogg']
        env.Append(LIBS = ['iconv', 'pthread'])
        env.Append(CCFLAGS = ['-arch', 'i386'])
        env.Object('src/osx/SDLMain.m', OBJPREFIX = OBJ_DIR,
                   CCFLAGS = ['-arch', 'i386', '-I/Library/Frameworks/SDL.framework/Headers/'])
        code.append('obj/SDLMain.o')
        env.Append(LINKFLAGS = ['-arch', 'i386'])
    else: #Linux
        env.Append(LIBS = ['SDL','GL', 'openal', 'vorbisfile',
                           'ogg', 'pthread'])


env.Program(target = "MoE", 
            source = code
            )

