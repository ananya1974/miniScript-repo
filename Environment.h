// Environment.h
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

class Environment {
public:
    Environment();

    bool exists(const std::string& name) const;
    int get(const std::string& name) const;
    void set(const std::string& name, int value);

    void pushScope();
    void popScope();

private:
    // Each scope is a map of variable name -> value
    std::vector<std::unordered_map<std::string, int>> scopes;
};

#endif // ENVIRONMENT_H
