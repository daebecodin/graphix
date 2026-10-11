script_folder="/Users/dmac/graphics/sdl/build/Debug/generators"
echo "echo Restoring environment" > "$script_folder/deactivate_conanbuildenv-debug-armv8.sh"
for v in LLVM_INSTALL_DIR CC CXX AS AR LD NM OBJCOPY OBJDUMP RANLIB SIZE STRINGS STRIP ADDR2LINE GDB PATH
do
   is_defined="true"
   value=$(printenv $v) || is_defined="" || true
   if [ -n "$value" ] || [ -n "$is_defined" ]
   then
       echo export "$v='$value'" >> "$script_folder/deactivate_conanbuildenv-debug-armv8.sh"
   else
       echo unset $v >> "$script_folder/deactivate_conanbuildenv-debug-armv8.sh"
   fi
done

export LLVM_INSTALL_DIR="/Users/dmac/.conan2/p/b/llvm-42d30c077eb0a/p"
export CC="clang"
export CXX="clang++"
export AS="clang"
export AR="llvm-ar"
export LD="ld"
export NM="llvm-nm"
export OBJCOPY="llvm-objcopy"
export OBJDUMP="llvm-objdump"
export RANLIB="llvm-ranlib"
export SIZE="llvm-size"
export STRINGS="llvm-strings"
export STRIP="llvm-strip"
export ADDR2LINE="llvm-addr2line"
export GDB="lldb"
export PATH="/Users/dmac/.conan2/p/b/llvm-42d30c077eb0a/p/bin:/Users/dmac/.conan2/p/ninja0d2a721f2424c/p/bin:/Users/dmac/.conan2/p/cmake25da4a7245917/p/CMake.app/Contents/bin${PATH:+:$PATH}"