#pragma once

#include "Staff.h"
#include "Manager.h"

// Predefined staff accounts
// Note: IDs should be unique across users.
static Staff g_defaultStaff{ 1001, "Default Staff", "staff123", "staff@example.com" };
static Manager g_managerAccount{ 9001, "Manager", "manager123", "manager@example.com" };

// Optionally expose a collection for iteration/login lookup
static const Staff* g_allUsers[] = {
    &g_defaultStaff,
    &g_managerAccount
};
