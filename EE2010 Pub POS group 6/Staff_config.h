#pragma once

#include "Staff.h"
#include "Manager.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>

// Built-in demo accounts. These are static lifetime objects.
static Staff g_defaultStaff{ 1001, "conor", "123", "staff@example.com" };
static Manager g_managerAccount{ 9001, "clive", "123", "manager@example.com" };

// Mutable user directory used for login and staff management.
// Notes on ownership:
// - Built-in users (above) are stored as pointers in this vector but are not deleted.
// - Users added at runtime are new'ed and deleted when removed.
static std::vector<Staff*> g_users{
    &g_defaultStaff,
    &g_managerAccount
};

// Find a user by numeric id.
inline Staff* findUserById(int id) {
    auto it = std::find_if(g_users.begin(), g_users.end(), [id](Staff* u){ return u && u->getId() == id; });
    return (it != g_users.end()) ? *it : nullptr;
}

// Case-insensitive name lookup for staff management helpers (login uses a similar function in main).
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

// Add a new account. Returns nullptr if id or name already exists.
// isManager=true creates a Manager, otherwise a Staff.
inline Staff* addStaff(int id, const std::string& name, const std::string& password, const std::string& email, bool isManager) {
    if (findUserById(id) || findUserByNameCI(name)) return nullptr;

    if (isManager) {
        auto* created = new Manager(id, name, password, email);
        g_users.push_back(created);
        return created;
    } else {
        auto* created = new Staff(id, name, password, email);
        g_users.push_back(created);
        return created;
    }
}

// Remove a user by id. Frees memory for dynamic users, keeps built-ins alive.
inline bool removeUserById(int id) {
    for (auto it = g_users.begin(); it != g_users.end(); ++it) {
        Staff* u = *it;
        if (u && u->getId() == id) {
            if (u != &g_defaultStaff && u != (Staff*)&g_managerAccount) {
                delete u;
            }
            g_users.erase(it);
            return true;
        }
    }
    return false;
}
