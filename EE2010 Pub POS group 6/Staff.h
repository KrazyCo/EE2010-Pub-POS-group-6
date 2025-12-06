#pragma once

#include <string>

class Staff
{
private:
    int userID;
    std::string userName;
    std::string password;
    std::string email;
public:
    Staff(int id, const std::string& name, const std::string& pwd, const std::string& mail)
        : userID(id), userName(name), password(pwd), email(mail) {}

    // Polymorphic discriminator
    virtual bool isManager() const { return false; }
    virtual ~Staff() = default;

    int getUserID() const { return userID; }
    std::string getUserName() const { return userName; }
    std::string getPassword() const { return password; }
    std::string getEmail() const { return email; }
    void setUserName(const std::string& name) { userName = name; }
    void setPassword(const std::string& pwd) { password = pwd; }
    void setEmail(const std::string& mail) { email = mail; }
};

