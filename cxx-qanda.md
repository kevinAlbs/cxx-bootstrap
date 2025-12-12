# Q8: Why are references to `optional<write_concern>` not returned in v1 API? 
See https://github.com/mongodb/mongo-cxx-driver/pull/1482

> Do not return `T const&` or `Optional<T> const&`: return `T` or `Optional<T>` instead.
> * Avoid returning references to internal representation (e.g. `Optional<T> const&`) which severely restrict implementation freedom.

# Q7: When is converting between v1 and v_noabi implicit / explicit?

A: Upgrade is explicit (v_noabi => v1) but downgrade is implicit (v1 => v_noabi)

# Q6: When should a class have the `_options` suffix?
A:

See https://github.com/mongodb/mongo-cxx-driver/pull/1491

> * Limited to options for commands with the same name (e.g. `.count()` vs. `count_options`). The `_options` suffix is not used for options-only types for non-commands (e.g. `.apm_opts()` vs. `apm`).

In short: use `_options` if suggested by name from specification, or if options for a command.

# Q5: Why is constructing v_noabi::document::value from v1::document::value not permitted?
A: I think because of user defined constructor.

# Q4: Why are some v_noabi classes not implemented in terms of v1?
A: One reason: to preserve "transitive view" behavior. A v_noabi class may store a view where a v1 class stores a value. See https://github.com/mongodb/mongo-cxx-driver/pull/1516:

```c++
bsoncxx::v_noabi::types::view t;

{
  hint h{"abc"};    // Stored as a view.
  t = h.to_value(); // Copied as a view.
} // `h` is destroyed, but the view of the "abc" string literal remains valid.

CHECK(t == bsoncxx::v_noabi::types::b_string{"abc"}); // OK...? :(
```

# Q3: What is the difference between an error code, category, and condition:
- code: int (e.g. `type_errc`) + category (platform dependent)
- condition: int + category (platform independent)

A category can define equivalences between the int and a condition:

```c++
bool equivalent(int v, std::error_condition const& ec)
```

In the C++ ABI V1: the category class is not directly exposed.

A code can be compared to a condition.

# Q2: When should a public method use `view`, `value`, `view_or_value`?

A: (Open)

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
