@echo off

cd build/bakefiles
bakefile_gen %* Bakefiles.bkgen
cd ../..
