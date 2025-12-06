#pragma once

#include "Staff.h"
#include "Manager.h"
#include <vector>
#include <memory>
#include <algorithm>

// Predefined staff accounts
// Note: IDs should be unique across users.
static Staff g_defaultStaff{ 1001, "conor", "123", "staff@example.com" };
static Manager g_managerAccount{ 9001, "clive", "123", "manager@example.com" };

// Mutable directory initialized with predefined accounts (non-owning for statics, owning for dynamic additions)
static std::vector<Staff*> g_users{
    &g_defaultStaff,
    &g_managerAccount
};

// Helpers to manage directory
inline Staff* findUserById(int id) {
    auto it = std::find_if(g_users.begin(), g_users.end(), [id](Staff* u){ return u && u->getId() == id; });
    return (it != g_users.end()) ? *it : nullptr;
}

inline Staff* findUserByNameCI(const std::string& name) {
    std::string nLower;
    nLower.reserve(name.size());
    for (char c : name) nLower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    for (Staff* u : g_users) {
        if (!u) continue;
        std::string uname = u->getUserName();
        std::string uLower;
        uLower.reserve(uname.size());
        for (char c : uname) uLower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        if (uLower == nLower) return u;
    }
    return nullptr;
}

// Add new Staff (returns pointer in directory)
inline Staff* addStaff(int id, const std::string& name, const std::string& password, const std::string& email, bool isManager) {
    // Ensure uniqueness by id or name
    if (findUserById(id) || findUserByNameCI(name)) return nullptr;

    if (isManager) {
        auto* created = new Manager(id, name, password, email); // owning
        g_users.push_back(created);
        return created;
    } else {
        auto* created = new Staff(id, name, password, email); // owning
        g_users.push_back(created);
        return created;
    }
}

// Remove user by id (deletes if dynamically allocated; leaves statics alone)
inline bool removeUserById(int id) {
    for (auto it = g_users.begin(); it != g_users.end(); ++it) {
        Staff* u = *it;
        if (u && u->getId() == id) {
            // Do not delete predefined statics
            if (u != &g_defaultStaff && u != (Staff*)&g_managerAccount) {
                delete u;
            }
            g_users.erase(it);
            return true;
        }
    }
    return false;
}
