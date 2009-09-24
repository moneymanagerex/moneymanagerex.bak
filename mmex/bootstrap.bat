@echo off

cd build/bakefiles
bakefile_gen %* build/bakefilesBakefiles.bkgen
cd ../..
