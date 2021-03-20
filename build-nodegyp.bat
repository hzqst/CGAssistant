cd CGANode

call node-gyp configure --arch=ia32 build

mkdir "..\build\Release\"

copy "build\Release\node_cga.node" "..\build\Release\"