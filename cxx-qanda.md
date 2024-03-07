# Q1: Are private methods included in the ABI?

A: I think "yes". Example:

```c++
// foo.hpp
class foo {
   public:
    int foo_public_method();

   private:
    int foo_private_method();
};
```

```c++
// foo.cpp
#include "foo.hpp"
int foo::foo_public_method() {
    return 123;
}

int foo::foo_private_method() {
    return 456;
}
```

```bash
% clang++ -o foo.dylib -dynamiclib foo.cpp
% nm foo.dylib                            
0000000000003f90 T __ZN3foo17foo_public_methodEv
0000000000003fa4 T __ZN3foo18foo_private_methodEv
```
