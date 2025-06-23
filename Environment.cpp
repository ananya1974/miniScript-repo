#include "Environment.h"

Environment::Environment() {
    // Start with a global scope
    scopes.emplace_back();
}

void Environment::set(const std::string& name, Value value) {
    if (scopes.empty()) throw std::runtime_error("No scope to define variable in.");
    scopes.back()[name] = value;
}

Value Environment::get(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    throw std::runtime_error("Variable not found: " + name);
}

bool Environment::exists(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) return true;
    }
    return false;
}

void Environment::pushScope() {
    scopes.emplace_back();
}

void Environment::popScope() {
    if (scopes.empty()) throw std::runtime_error("No scope to pop.");
    scopes.pop_back();
}
