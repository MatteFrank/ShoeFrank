SHELL /usr/bin/tcsh
*/1 * * * * cd @CMAKE_BINARY_DIR@/Reconstruction/level0 && ./nightbuild >> nightbuild.log 2>&1

