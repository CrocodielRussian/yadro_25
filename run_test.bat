@echo off

chcp 65001

cd build_ninja

ctest -j -N --output-on-failure -R %1
	