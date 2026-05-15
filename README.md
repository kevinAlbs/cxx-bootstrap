# Informative PRs

https://github.com/mongodb/mongo-cxx-driver/pull/1318 describes complexities around macro guards.

# Q&A

# Q12: Why are some APIs marked explicit(false)?
A:

# Q11: Are deprecated v_noabi options converted into v1?
A: Not for indexes. See https://github.com/mongodb/mongo-cxx-driver/pull/1579#discussion_r2795110131

# Q10: Why are some v_noabi classes unable to convert to v1?
A:

Quoting https://github.com/mongodb/mongo-cxx-driver/pull/1566
> potential invalidation of associated [...] objects

See https://github.com/mongodb/mongo-cxx-driver/pull/1563#discussion_r2713719269:

```c++
auto c1 = v1::client(...);
auto s1 = c1.start_session();

// Impossible: `from_v1()` must derive a `v_noabi::client` object
// to return in s1.client()
auto s0 = from_v1(std::move(s1));
CHECK(std::addressof(s1.client()) == std::addressof(???));
```

# Q9: Why do the v_noabi forwarding headers include v1 headers?
Example: https://github.com/mongodb/mongo-cxx-driver/pull/1557/changes#diff-50c95bffa79fdfd798380984b294848eafb88c8c608e8a6d6d40799d4e88055e
A:

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

# Q0.6: When to use `noexcept`?

A: See https://github.com/mongodb/mongo-cxx-driver/pull/1402#discussion_r2096510603

# Q0.5: How to build and run a spec test?

Configure with:
```
cmake -DCMAKE_CXX_STANDARD=17 -Bcmake-build -S.
```

The spec tests have separate test executables. To build the unified tests:
```
cmake --build cmake-build --target test_unified_format_specs
```

Built the `help` target to list all executables.

To run, the test runner (weirdly) expects environment variables to be set to the path of data files. I have this in my running scripts:

```
# Specify paths to test data for the test runner:
data_dir="$(pwd)/data"
export CHANGE_STREAMS_UNIFIED_TESTS_PATH="${data_dir}/change-streams/unified"
export CLIENT_SIDE_ENCRYPTION_LEGACY_TESTS_PATH="${data_dir}/client_side_encryption/legacy"
export CLIENT_SIDE_ENCRYPTION_TESTS_PATH="${data_dir}/client_side_encryption"
export CLIENT_SIDE_ENCRYPTION_UNIFIED_TESTS_PATH="${data_dir}/client_side_encryption/unified"
export COLLECTION_MANAGEMENT_TESTS_PATH="${data_dir}/collection-management"
export COMMAND_MONITORING_TESTS_PATH="${data_dir}/command-monitoring"
export CRUD_LEGACY_TESTS_PATH="${data_dir}/crud/legacy"
export CRUD_UNIFIED_TESTS_PATH="${data_dir}/crud/unified"
export GRIDFS_TESTS_PATH="${data_dir}/gridfs"
export INITIAL_DNS_SEEDLIST_DISCOVERY_TESTS_PATH="${data_dir}/initial_dns_seedlist_discovery"
export READ_WRITE_CONCERN_OPERATION_TESTS_PATH="${data_dir}/read-write-concern/operation"
export RETRYABLE_READS_LEGACY_TESTS_PATH="${data_dir}/retryable-reads/legacy"
export RETRYABLE_READS_UNIFIED_TESTS_PATH="${data_dir}/retryable-reads/unified"
export RETRYABLE_WRITES_UNIFIED_TESTS_PATH="${data_dir}/retryable-writes/unified"
export SESSION_UNIFIED_TESTS_PATH="${data_dir}/sessions/unified"
export TRANSACTIONS_LEGACY_TESTS_PATH="${data_dir}/transactions/legacy"
export TRANSACTIONS_UNIFIED_TESTS_PATH="${data_dir}/transactions/unified"
export UNIFIED_FORMAT_TESTS_PATH="${data_dir}/unified-format"
export URI_OPTIONS_TESTS_PATH="${data_dir}/uri-options"
export VERSIONED_API_TESTS_PATH="${data_dir}/versioned-api"
export WITH_TRANSACTION_TESTS_PATH="${data_dir}/with_transaction"
export INDEX_MANAGEMENT_TESTS_PATH="${data_dir}/index-management"

# Run all unified tests for the index management spec:
./cmake-build/src/mongocxx/test/test_unified_format_specs "index management spec automated tests" 

# Run a single test file:
./cmake-build/src/mongocxx/test/test_unified_format_specs "index management spec automated tests" --section createSearchIndex.json

# Run a single test within a test file:
./cmake-build/src/mongocxx/test/test_unified_format_specs "index management spec automated tests" --section createSearchIndex.json --section "no name provided for an index definition"
```

# Q0.4: When to review a view type? When to return a value type? When to return a view_or_value type?

Return a value type when the function is creating the value. Example: https://github.com/mongodb/mongo-cxx-driver/pull/986#discussion_r1268315781. Cannot return a view to a newly created value.

TODO


# Q0.3: What view/value type should be used for passing and storing options?

A: `view_or_value`. This agrees with more recent API changes (e.g. `rewrap_many_datakey.hpp`) and may provide callers the most flexibility.

```c++
class search_index_model_v2 {
   public:
    search_index_model_v2(bsoncxx::string::view_or_value name) : _name(name) {}

    void print_name() {
        if (_name) {
            std::cout << "name is: " << _name->view() << std::endl;
        } else {
            std::cout << "name is: not set" << std::endl;
        }
    }

   private:
    bsoncxx::stdx::optional<bsoncxx::string::view_or_value> _name;
};

TEST_CASE("search_index_management", "[collection]") {
    instance::current();

    // Caller can pass view.
    // No copy occurs. Caller is responsible for ensuring value `s` outlives the
    // `search_index_model_v2`.
    {
        std::string s = "abc";
        bsoncxx::stdx::string_view sv(s);
        search_index_model_v2 sim(sv.substr(0, 1));
        sim.print_name();
    }

    // Caller can move value.
    {
        std::string s = "abc";
        search_index_model_v2 sim(std::move(s));
        sim.print_name();
    }

    // Caller can copy value.
    {
        std::string s = "abc";
        search_index_model_v2 sim(s);
        sim.print_name();
    }
}
```


# Q0.2: Does the C++ driver implement the standard API or index view API for indexes?

A:

The index view API is implemented: https://jira.mongodb.org/browse/CXX-1358
The standard API is partially implemented: https://jira.mongodb.org/browse/CXX-1351

# Q0.1: Why is a runtime error observed on Windows?

A: 
See https://jira.mongodb.org/browse/CXX-1846?focusedCommentId=2506398&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-2506398:

> A developer only hint: on Windows, very frequently these sorts of weird crashes are caused by inadvertently mixing different runtime libraries. All of the components (C driver, Boost, C++ Driver, Application) must agree on whether the Debug or Release CRT is in play, along with whether you are building against the Static or Dynamic version is being used. Weird string crashes are the canonical symptom of such misconfigurations.

See also: https://jira.mongodb.org/browse/CXX-2707?focusedCommentId=5557340&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-5557340

> We suspect this error may instead be caused by the use of incompatible build configurations for the bsoncxx library vs. the test application. The build configuration for a Windows application must be consistent with the build configuration of all the libraries being linked against. To compile and link the test application using the Release configuration, it must link against a bsoncxx library which was also built using the Release configuration.
