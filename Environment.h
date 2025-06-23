#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <variant>
#include <vector>
#include <stdexcept>

// Expanded Value type to match Interpreter's usage
using Value = std::variant<int, float, char, std::string>;

class Environment {
public:
    Environment();

    // Variable management
    void set(const std::string& name, Value value);
    Value get(const std::string& name) const;
    bool exists(const std::string& name) const;

    // Scope control
    void pushScope();
    void popScope();

private:
    std::vector<std::unordered_map<std::string, Value>> scopes;
};

#endif // ENVIRONMENT_H
