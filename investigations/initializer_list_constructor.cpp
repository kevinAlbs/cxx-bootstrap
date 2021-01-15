#include <iostream>
#include <optional>
#include <variant>
#include <vector>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/json.hpp>

enum class value_type { INT32, STRING, DOCUMENT };

/* Represents a non-recursive BSON type. */
class primitive {
public:
    primitive() = delete;

    primitive (const char* value) {
        this->value = std::string(value);
        this->type = type::STRING;
    }

    primitive (std::int32_t value) {
        this->value = value;
        this->type = type::INT32;
    }

    void append_to (bsoncxx::builder::core& core) {
        if (this->type == type::INT32) {
            core.append (std::get<int32_t>(this->value));
        } else if (this->type == type::STRING) {
            core.append (std::get<std::string>(this->value));
        }
    }

    enum class type { INT32, STRING } type;
    std::variant<int32_t, std::string> value;
};

class key_value_list {
public:
    key_value_list (std::string key, primitive value) : core(false) {
        this->core.key_owned (key);
        value.append_to (this->core);
    }

    key_value_list (std::string key, key_value_list value) : core(false) {
        this->core.key_owned (key);
        this->core.append(value.core.view_document());
    }

    key_value_list (std::initializer_list<key_value_list> list) : core(false) {
        for (const key_value_list& kvl : list) {
            this->core.concatenate (kvl.core.view_document());
        }
    }

    std::string to_json () {
        return bsoncxx::to_json (this->core.view_document());
    }

    bsoncxx::builder::core core;
};

int main() {
    key_value_list one_value = {{"a", "b"}};
    std::cout << one_value.to_json() << std::endl;
    key_value_list two_values = {{"a", "b"}, {"c", 1}};
    std::cout << two_values.to_json() << std::endl;
    key_value_list nested = {{"a", "b"}, {"c", 1}, {"d", {"e", "f"}}};
    std::cout << nested.to_json() << std::endl;
}