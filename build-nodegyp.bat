cd CGANode

call node-gyp configure --arch=ia32 --msvs_version=2017 build

mkdir "..\build\Release\"

copy "build\Release\node_cga.node" "..\build\Release\"