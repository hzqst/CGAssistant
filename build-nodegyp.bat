cd CGANode

call node-gyp configure build --arch=ia32

mkdir "..\build\Release\"

copy "build\Release\node_cga.node" "..\build\Release\"