#pragma once

#include "Staff.h"
#include <string>

class Manager : public Staff
{
	public:
	Manager(int id, const std::string& name, const std::string& pwd, const std::string& mail)
		: Staff(id, name, pwd, mail) {}

	// Override to indicate manager role
	bool isManager() const override { return true; }
};

