# Q4: When to review a view type? When to review a value type? When to return a view_or_value type?

Return a value type when the function is creating the value. Example: https://github.com/mongodb/mongo-cxx-driver/pull/986#discussion_r1268315781. Cannot return a view to a newly created value.

TODO


# Q3: What view/value type should be used for passing and storing options?

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


# Q2: Does the C++ driver implement the standard API or index view API for indexes?

A:

The index view API is implemented: https://jira.mongodb.org/browse/CXX-1358
The standard API is partially implemented: https://jira.mongodb.org/browse/CXX-1351

# Q1: Why is a runtime error observed on Windows?

A: 
See https://jira.mongodb.org/browse/CXX-1846?focusedCommentId=2506398&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-2506398:

> A developer only hint: on Windows, very frequently these sorts of weird crashes are caused by inadvertently mixing different runtime libraries. All of the components (C driver, Boost, C++ Driver, Application) must agree on whether the Debug or Release CRT is in play, along with whether you are building against the Static or Dynamic version is being used. Weird string crashes are the canonical symptom of such misconfigurations.

See also: https://jira.mongodb.org/browse/CXX-2707?focusedCommentId=5557340&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-5557340

> We suspect this error may instead be caused by the use of incompatible build configurations for the bsoncxx library vs. the test application. The build configuration for a Windows application must be consistent with the build configuration of all the libraries being linked against. To compile and link the test application using the Release configuration, it must link against a bsoncxx library which was also built using the Release configuration.
