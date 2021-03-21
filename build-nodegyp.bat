cd CGANode

call node-gyp configure build

mkdir "..\build\Release\"

copy "build\Release\node_cga.node" "..\build\Release\"