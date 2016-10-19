# Install (do once)

1. `cd <path-to-llvm-source>/lib/Transforms`
2. `git clone https://github.com/loki04/llvm-cfo.git CFO`
3. `echo "add_subdirectory(CFO) " >> CMakeLists.txt`

# Simple build instruction

1. `cd <path-to-llvm-source>`
2. `mkdir -p build/x86_64 && cd build/x86_64`
3. `cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<path-to-llvm-source>/install`
4. `make`

# Execute

1. `./x86_64/bin/clang <source> -emit-llvm -c -o <bcfile>`
2. `./x86_64/bin/opt -load ./x86_64/lib/LLVMCFO.so -cfo < <bcfile>`

# Help

1. http://llvm.org/docs/WritingAnLLVMPass.html

# References

1. https://www.gnu.org/software/gcc/projects/cfo.html
2. http://www.inf.u-szeged.hu/~akiss/pub/pdf/loki_factoring.pdf
3. http://lists.llvm.org/pipermail/llvm-dev/2016-August/104170.html
