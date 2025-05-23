// Environment.cpp
#include "Environment.h"

Environment::Environment() {
    // Start with one global scope
    scopes.emplace_back();
}

bool Environment::exists(const std::string& name) const {
    // Check from innermost to outermost scope
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) return true;
    }
    return false;
}

int Environment::get(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    throw std::runtime_error("Variable '" + name + "' not defined");
}

void Environment::set(const std::string& name, int value) {
    // Assign to existing variable in nearest scope
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            found->second = value;
            return;
        }
    }
    // If not found, define in current (innermost) scope
    scopes.back()[name] = value;
}

void Environment::pushScope() {
    scopes.emplace_back();
}

void Environment::popScope() {
    if (scopes.size() == 1)
        throw std::runtime_error("Cannot pop global scope");
    scopes.pop_back();
}
