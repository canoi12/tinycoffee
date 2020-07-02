JIT=

if [[ $@ == "nojit" ]]; then
	JIT=NOJIT=true
	echo "compiling with Lua 5.4"
fi

make clean
rm -f tico.zip
rm -rf release/


mkdir -p release
mkdir -p release/include
mkdir -p release/lib/linux
mkdir -p release/lib/windows

cp src/tico.h release/include/

make all MODE=Release $JIT
mv ./bin/tico ./release/
mv libtico.a ./release/lib/linux
mv libtico.so ./release/lib/linux

make clean

make all PLATFORM=Windows MODE=Release $JIT
mv ./bin/tico.exe ./release/
mv libtico.a ./release/lib/windows
mv libtico.dll ./release/lib/windows

strip ./release/lib/windows/libtico.dll
strip ./release/lib/linux/libtico.so

zip -r tico.zip release/

make clean
