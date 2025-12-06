#pragma once

#include "Staff.h"
#include <string>

// Manager role, inherits login fields from Staff and flips isManager().
class Manager : public Staff
{
	public:
	Manager(int id, const std::string& name, const std::string& pwd, const std::string& mail)
		: Staff(id, name, pwd, mail) {}

	// Override to indicate manager role
	bool isManager() const override { return true; }
};

