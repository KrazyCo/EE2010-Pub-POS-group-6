// EE2010 Pub POS group 6.cpp
// Entry point and console UI for the POS app.
// This file wires together login, bill workflows, item screens, and manager tools.

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>

#include "Bill.h"
#include "Item.h"
#include "Item_config.h"
#include "Drink.h" // needed to check alcohol flag before adding to a bill
#include "Staff.h"
#include "Manager.h"
#include "Staff_config.h"

// Global exit flag that propagates out of nested menus (e.g., screens, stock, staff)
static bool g_exitRequested = false;

// ------ Helpers ------
// Clears any error flags and flushes the rest of the current input line.
static void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Lowercase copy, safe with std::tolower for unsigned char.
static std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

// Trim whitespace from both ends.
static std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Simple banner renderer for sections and menus.
static void renderHeader(const std::string& title, int currentBillIndex)
{
    std::cout << "\n========================================\n";
    std::cout << " " << title;
    if (currentBillIndex >= 0) {
        std::cout << "  [Current Bill: #" << currentBillIndex << "]";
    } else {
        std::cout << "  [Current Bill: none]";
    }
    std::cout << "\n========================================\n";
}

static void renderDivider()
{
    std::cout << "----------------------------------------\n";
}

// Groups the catalog into screens by the item "screen" id.
// This keeps the rest of the UI agnostic to how items are laid out.
static std::map<uint8_t, std::vector<Item*>> groupCatalogByScreen(const std::vector<Item*>& catalog)
{
    std::map<uint8_t, std::vector<Item*>> byScreen;
    for (Item* it : catalog) {
        if (!it) continue;
        byScreen[it->getScreen()].push_back(it);
    }
    return byScreen;
}

// Case-insensitive exact name match across the unified catalog.
static Item* findItemByName(const std::vector<Item*>& catalog, const std::string& name)
{
    std::string target = toLower(name);
    for (Item* it : catalog) {
        if (!it) continue;
        if (toLower(it->getName()) == target) {
            return it;
        }
    }
    return nullptr;
}

// Show a compact list of bills with totals and who created them.
static void renderBillsSummary(int currentBillIndex)
{
    renderHeader("Bills", currentBillIndex);
    if (bills.empty()) {
        std::cout << " No bills yet.\n";
        renderDivider();
        return;
    }
    for (size_t i = 0; i < bills.size(); ++i) {
        const Bill& b = bills[i];
        std::cout << " [" << i << "] "
                  << "Items: " << b.getItems().size()
                  << " | Total: $" << std::fixed << std::setprecision(2) << b.getTotalPrice()
                  << " | Paid: " << (b.isPaid() ? "Yes" : "No")
                  << " | By: " << b.getServerName()
                  << (static_cast<int>(i) == currentBillIndex ? "  <- current" : "")
                  << "\n";
    }
    renderDivider();
}

// Show full details for the current bill
static void renderCurrentBill(const Bill* bill, int billIndex)
{
    renderHeader("Current Bill Details", billIndex);
    if (!bill) {
        std::cout << " No bill selected.\n";
        renderDivider();
        return;
    }
    // Formats a time_t into "YYYY-MM-DD HH:MM:SS" or "N/A" if unset.
    auto formatTime = [](time_t t) {
        if (t == 0) return std::string("N/A");
        char buf[64]{};
        std::tm tm{};
        if (localtime_s(&tm, &t) == 0) {
            if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm)) {
                return std::string(buf);
            }
        }
        return std::string("N/A");
    };

    std::cout << " Created by: " << bill->getServerName() << "\n";
    std::cout << " Opened: " << formatTime(bill->getTimeOpened()) << "\n";
    std::cout << " Paid at: " << formatTime(bill->getTimePaid()) << "\n";
    std::cout << " Items on Bill: " << bill->getItems().size()
              << " | Total: $" << std::fixed << std::setprecision(2) << bill->getTotalPrice()
              << " | Paid: " << (bill->isPaid() ? "Yes" : "No") << "\n";
    const auto& items = bill->getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        const Item* it = items[i];
        std::cout << " [" << i << "] " << it->getName()
                  << " | $" << std::fixed << std::setprecision(2) << it->getPrice() << "\n";
    }
    renderDivider();
}

// Pause after printing a section so users can read before continuing.
static void waitForEnter()
{
    std::cout << " Press Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// Quick yes/no for alcohol ID checks.
static bool confirmIdChecked()
{
    std::cout << " This item is alcoholic. Confirm ID checked (y/n): ";
    std::string resp;
    std::getline(std::cin, resp);
    std::string r = toLower(trim(resp));
    return (r == "y" || r == "yes");
}

// --- Logon/Logoff ---
static const Staff* g_currentUser = nullptr;

// Login banner only.
static void renderLoginHeader()
{
    std::cout << "\n========================================\n";
    std::cout << " Login";
    std::cout << "\n========================================\n";
}

// Login lookup searches the user list.
static const Staff* findUserByName(const std::string& name)
{
    // Case-insensitive match over the mutable user directory
    std::string needle = toLower(trim(name));
    for (Staff* u : g_users) {
        if (!u) continue;
        if (toLower(u->getUserName()) == needle) {
            return u;
        }
    }
    return nullptr;
}

// Basic username/password auth.
// Type "exit" at the username prompt to quit the whole app.
static const Staff* tryLogin()
{
    renderLoginHeader();
    std::cout << " Enter username (or 'exit' to quit): ";
    std::string name;
    std::getline(std::cin, name);
    if (toLower(trim(name)) == "exit") {
        g_exitRequested = true;
        return nullptr;
    }

    const Staff* user = findUserByName(name);
    if (!user) {
        std::cout << "\nERROR: User not found.\n";
        return nullptr;
    }

    std::cout << " Enter password: ";
    std::string pwd;
    std::getline(std::cin, pwd);

    if (pwd != user->getPassword()) {
        std::cout << "\nERROR: Invalid password.\n";
        return nullptr;
    }

    std::cout << "\nLogged in as: " << user->getUserName()
              << (user->isManager() ? " (Manager)\n" : " (Staff)\n");
    return user;
}

// --- helpers to parse stock commands ---
// Parses "name count" where name is a single token (no spaces) and count is an int.
// Returns false on parse errors. Used by stock commands.
static bool tryParseTwoTokens(const std::string& s, std::string& nameOut, int& countOut)
{
    std::istringstream iss(s);
    std::string name;
    std::string countStr;
    if (!(iss >> name)) return false;
    if (!(iss >> countStr)) return false;
    try {
        int c = std::stoi(countStr);
        countOut = c;
        nameOut = name;
        return true;
    } catch (...) {
        return false;
    }
}

// Global command parser that can run from any menu.
// Returns true if the command was handled here.
// Notes:
// - "screen <x>" is not fully handled here; returning false lets the caller continue into the screen flow.
// - Stock commands are manager-only and protected by a role check.
static bool handleGlobalCommand(const std::string& input, Bill*& currentBill, int& currentBillIndex, const std::vector<Item*>& catalog)
{
    std::string cmd = toLower(input);

    // exit (global)
    if (cmd == "exit") {
        g_exitRequested = true;
        std::cout << "\nExiting...\n";
        return true;
    }

    // logoff (global): return to login screen
    if (cmd == "logoff" || cmd == "logout") {
        std::cout << "\nLogging off...\n";
        g_currentUser = nullptr;
        return true;
    }

    // Manager-only: stock add/remove
    if (cmd.rfind("stock add ", 0) == 0 || cmd.rfind("stock remove ", 0) == 0) {
        if (!g_currentUser || !g_currentUser->isManager()) {
            std::cout << "\nERROR: Only managers can modify stock.\n";
            return true;
        }
        const bool isAdd = (cmd.rfind("stock add ", 0) == 0);
        std::string args = trim(input.substr(isAdd ? 10 : 13)); // after "stock add " or "stock remove "
        std::string nameToken;
        int count = 0;
        if (!tryParseTwoTokens(args, nameToken, count) || count == 0) {
            std::cout << "\nERROR: Usage: " << (isAdd ? "stock add <name> <count>" : "stock remove <name> <count>") << "\n";
            return true;
        }
        Item* target = findItemByName(catalog, nameToken);
        if (!target) {
            std::cout << "\nERROR: Item not found: " << nameToken << "\n";
            return true;
        }
        int currentQty = target->getQuantityLeft();
        int newQty = isAdd ? (currentQty + count) : (currentQty - count);
        if (newQty < 0) newQty = 0;
        target->setQuantityLeft(newQty);
        std::cout << "\nStock " << (isAdd ? "added" : "removed") << " for " << target->getName()
                  << ". New stock: " << newQty << "\n";
        return true;
    }

    // bill new
    if (cmd == "bill new") {
        const std::string creator = g_currentUser ? g_currentUser->getUserName() : std::string{};
        bills.emplace_back(Bill{ creator });
        currentBill = &bills.back();
        currentBillIndex = static_cast<int>(bills.size()) - 1;
        std::cout << "\nCreated bill #" << currentBillIndex << " by " << (creator.empty() ? "unknown" : creator) << " and set as current.\n";
        return true;
    }

    // bill paid (must come before "bill <x>")
    if (cmd == "bill paid") {
        if (!currentBill) {
            std::cout << "\nERROR: No bill selected.\n";
            return true;
        }
        if (currentBill->isPaid()) {
            std::cout << "\nERROR: Bill is already paid.\n";
            return true;
        }
        currentBill->markPaid();
        std::cout << "\nMarked current bill as paid.\n";
        return true;
    }

    // bill <x>
    if (cmd.rfind("bill ", 0) == 0 && cmd.size() > 5) {
        std::string numStr = trim(cmd.substr(5));
        bool allDigits = !numStr.empty() && std::all_of(numStr.begin(), numStr.end(),
            [](char c){ return std::isdigit(static_cast<unsigned char>(c)); });
        if (!allDigits) {
            std::cout << "\nERROR: Invalid bill number.\n";
            return true;
        }
        try {
            int idx = std::stoi(numStr);
            if (idx >= 0 && idx < static_cast<int>(bills.size())) {
                currentBill = &bills[static_cast<size_t>(idx)];
                currentBillIndex = idx;
                std::cout << "\nSwitched to bill #" << idx << ".\n";
            } else {
                std::cout << "\nERROR: Bill #" << idx << " does not exist.\n";
            }
        } catch (...) {
            std::cout << "\nERROR: Invalid bill number.\n";
        }
        return true;
    }

    // bill
    if (cmd == "bill") {
        std::cout << "\n";
        renderCurrentBill(currentBill, currentBillIndex);
        waitForEnter();
        return true;
    }

    // bills
    if (cmd == "bills") {
        std::cout << "\n";
        renderBillsSummary(currentBillIndex);
        waitForEnter();
        return true;
    }

    // screen <x> is parsed here but handled by the caller (main or screenMenu).
    if (cmd.rfind("screen ", 0) == 0 && cmd.size() > 7) {
        if (!currentBill) {
            std::cout << "\nERROR: Select or create a bill first.\n";
            return true;
        }
        return false;
    }

    // add <name> (global add by name; screen-specific add-by-index is handled in screenMenu)
    if (cmd.rfind("add ", 0) == 0 && cmd.size() > 4) {
        if (!currentBill) {
            std::cout << "\nERROR: Select or create a bill first.\n";
            return true;
        }
        if (currentBill->isPaid()) {
            std::cout << "\nERROR: Bill is already paid.\n";
            return true;
        }
        std::string name = input.substr(4);
        Item* it = findItemByName(catalog, name);
        if (!it) {
            std::cout << "\nERROR: Item not found: " << name << "\n";
            return true;
        }

        // For alcoholic drinks, we require an ID check confirmation.
        if (auto* drink = dynamic_cast<Drink*>(it)) {
            if (drink->getIsAlcohol()) {
                if (!confirmIdChecked()) {
                    std::cout << "\nAction cancelled. ID must be checked for alcoholic items.\n";
                    return true;
                }
            }
        }

        if (currentBill->addItem(*it)) {
            std::cout << "\nAdded: " << it->getName()
                      << " | New total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
        } else {
            std::cout << "\nERROR: Failed to add (likely out of stock).\n";
        }
        return true;
    }

    // remove <name>
    if (cmd.rfind("remove ", 0) == 0 && cmd.size() > 7) {
        if (!currentBill) {
            std::cout << "\nERROR: Select or create a bill first.\n";
            return true;
        }
        if (currentBill->isPaid()) {
            std::cout << "\nERROR: Bill is already paid.\n";
            return true;
        }
        std::string name = input.substr(7);
        Item* it = findItemByName(catalog, name);
        if (!it) {
            std::cout << "\nERROR: Item not found: " << name << "\n";
            return true;
        }
        if (currentBill->removeItem(*it)) {
            std::cout << "\nRemoved: " << it->getName()
                      << " | New total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
        } else {
            std::cout << "\nERROR: Item not found on current bill.\n";
        }
        return true;
    }

    return false; // not handled here
}

// Screen list for a single screen id. Lets users add/remove by index or name.
// Notes:
// - We show stock here to help staff avoid adding out-of-stock items.
// - "back" or "0" returns to the screen selection menu.
// - Stock admin is not shown here; managers have a separate Stock screen.
static void screenMenu(Bill*& currentBill, int& currentBillIndex, uint8_t screenId, const std::vector<Item*>& screenItems)
{
    for (;;) {
        if (g_exitRequested) return;

        renderHeader("Screen " + std::to_string(screenId), currentBillIndex);
        if (screenItems.empty()) {
            std::cout << " No items on this screen.\n";
        } else {
            for (size_t i = 0; i < screenItems.size(); ++i) {
                const Item* it = screenItems[i];
                std::cout << " [" << i << "] "
                          << it->getName()
                          << " | $" << std::fixed << std::setprecision(2) << it->getPrice()
                          << " | Stock: " << it->getQuantityLeft()
                          << "\n";
            }
        }
        renderDivider();

        if (!currentBill) {
            std::cout << " ERROR: No bill selected.\n";
            std::cout << " [0] Back to screens\n";
            renderDivider();
            return;
        }

        std::cout << " Bill total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
        std::cout << " [0] Back to screens\n";
        renderDivider();


        std::cout << " Commands: add <name>, add <index>, remove <name>, screen <x>, bill new, bill <x>, bill, bills, bill paid, logoff, logout, exit\n";
        renderDivider();

        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::string lower = toLower(trim(input));
        if (lower == "exit") { g_exitRequested = true; return; }
        if (lower == "back" || lower == "0" || lower == "[0]") { return; }

        // Command: add by name or index
        if (lower.rfind("add ", 0) == 0 && lower.size() > 4) {
            if (currentBill->isPaid()) {
                std::cout << "\nERROR: Bill is already paid.\n";
                continue;
            }

            std::string paramRaw = input.substr(4);
            std::string param = trim(paramRaw);

            // Try index first: accepts "5" or "[5]"
            bool addedByIndex = false;
            if (!param.empty()) {
                if (param.front() == '[' && param.back() == ']' && param.size() >= 3) {
                    param = trim(param.substr(1, param.size() - 2));
                }
                bool allDigits = !param.empty() && std::all_of(param.begin(), param.end(),
                    [](char c){ return std::isdigit(static_cast<unsigned char>(c)); });
                if (allDigits) {
                    int idx = std::stoi(param);
                    if (idx >= 0 && idx < static_cast<int>(screenItems.size())) {
                        Item* selected = screenItems[static_cast<size_t>(idx)];

       
                        if (selected == nullptr) {
                            std::cout << "\nERROR: Item not found: " << param << "\n";
                            addedByIndex = true;
                            continue;
                        }

                        // Alcohol ID check
                        if (auto* drinkPtr = dynamic_cast<Drink*>(selected)) {
                            if (drinkPtr->getIsAlcohol() && !confirmIdChecked()) {
                                std::cout << "\nAction cancelled. ID must be checked for alcoholic items.\n";
                                addedByIndex = true;
                                continue;
                            }
                        }

                        if (currentBill->addItem(*selected)) {
                            std::cout << "\nAdded: " << selected->getName()
                                      << " | New total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
                        } else {
                            std::cout << "\nERROR: Failed to add (likely out of stock).\n";
                        }
                        addedByIndex = true;
                        continue;
                    } else {
                        std::cout << "\nERROR: Item not found: " << param << "\n";
                        addedByIndex = true;
                        continue;
                    }
                }
            }
            if (addedByIndex) continue;

            // By name within this screen
            Item* selected = nullptr;
            std::string nameLower = toLower(param);
            for (Item* it : screenItems) {
                if (toLower(it->getName()) == nameLower) { selected = it; break; }
            }
            if (selected == nullptr) {
                std::cout << "\nERROR: Item not found: " << param << "\n";
                continue;
            }

            // Alcohol ID check
            if (auto* drinkPtr = dynamic_cast<Drink*>(selected)) {
                if (drinkPtr->getIsAlcohol() && !confirmIdChecked()) {
                    std::cout << "\nAction cancelled. ID must be checked for alcoholic items.\n";
                    continue;
                }
            }

            if (currentBill->addItem(*selected)) {
                std::cout << "\nAdded: " << selected->getName()
                          << " | New total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
            } else {
                std::cout << "\nERROR: Failed to add (likely out of stock).\n";
            }
            continue;
        }

        // Command: remove by name (screen-local)
        if (lower.rfind("remove ", 0) == 0 && lower.size() > 7) {
            if (currentBill->isPaid()) {
                std::cout << "\nERROR: Bill is already paid.\n";
                continue;
            }

            std::string name = input.substr(7);
            Item* selected = nullptr;
            for (Item* it : screenItems) {
                if (toLower(it->getName()) == toLower(name)) { selected = it; break; }
            }
            if (!selected) { std::cout << "\nERROR: Item not found on this screen: " << name << "\n"; continue; }
            if (currentBill->removeItem(*selected)) {
                std::cout << "\nRemoved: " << selected->getName()
                          << " | New total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
            } else {
                std::cout << "\nERROR: Item not found on current bill.\n";
            }
            continue;
        }

        // Let common commands run (bill, bills, logoff, etc.).
        if (handleGlobalCommand(input, currentBill, currentBillIndex, catalogItems)) {
            if (g_exitRequested) return;
            // If user logged off, break out to login loop
            if (!g_currentUser) return;
            continue;
        }

        std::cout << "\nERROR: Unknown command.\n";
    }
}

// Manager-only stock management.
// Keep operations explicit: set, add, remove. Negative stock is clamped to 0.
// This screen intentionally separates stock admin from order-taking.
static void stockMenu(const std::vector<Item*>& catalog)
{
    for (;;) {
        if (g_exitRequested) return;

        renderHeader("Stock Management", -1);
        if (catalog.empty()) {
            std::cout << " Catalog is empty.\n";
        } else {
            // Simple fixed-width table for readability.
            std::cout << std::left
                      << std::setw(8)  << "Index"
                      << std::setw(32) << "Name"
                      << std::setw(10) << "Stock"
                      << std::setw(10) << "Price"
                      << "\n";

            for (size_t i = 0; i < catalog.size(); ++i) {
                const Item* it = catalog[i];
                if (!it) continue;

                std::cout << std::left
                          << std::setw(8)  << ("[" + std::to_string(i) + "]")
                          << std::setw(32) << it->getName()
                          << std::setw(10) << it->getQuantityLeft()
                          << std::setw(10) << (std::string("$") + (static_cast<std::ostringstream&&>(std::ostringstream() << std::fixed << std::setprecision(2) << it->getPrice())).str())
                          << "\n";
            }
        }
        renderDivider();
        std::cout << " Commands:\n";
        std::cout << "  set <index> <count>      - set stock to exact value\n";
        std::cout << "  add <index> <count>      - add to stock\n";
        std::cout << "  remove <index> <count>   - remove from stock\n";
        std::cout << "  back                     - go back\n";
        std::cout << "  exit                     - exit application\n";
        renderDivider();

        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::string lower = toLower(trim(input));
        if (lower == "exit") { g_exitRequested = true; return; }
        if (lower == "back") { return; }

        // Parse commands: verb index count
        std::istringstream iss(lower);
        std::string verb;
        std::string idxStr;
        std::string countStr;
        if (!(iss >> verb)) { std::cout << "\nERROR: Invalid command.\n"; continue; }

        if (verb == "set" || verb == "add" || verb == "remove") {
            if (!(iss >> idxStr >> countStr)) {
                std::cout << "\nERROR: Usage: " << verb << " <index> <count>\n";
                continue;
            }
            int idx = -1;
            int count = 0;
            try { idx = std::stoi(idxStr); } catch (...) { idx = -1; }
            try { count = std::stoi(countStr); } catch (...) { count = 0; }
            if (idx < 0 || idx >= static_cast<int>(catalog.size())) {
                std::cout << "\nERROR: Invalid index.\n";
                continue;
            }
            Item* target = catalog[static_cast<size_t>(idx)];
            if (!target) {
                std::cout << "\nERROR: Item not found.\n";
                continue;
            }
            if (count < 0) {
                std::cout << "\nERROR: Count must be non-negative.\n";
                continue;
            }

            int currentQty = target->getQuantityLeft();
            int newQty = currentQty;
            if (verb == "set") {
                newQty = count;
            } else if (verb == "add") {
                newQty = currentQty + count;
            } else if (verb == "remove") {
                newQty = currentQty - count;
                if (newQty < 0) newQty = 0;
            }

            target->setQuantityLeft(newQty);
            std::cout << "\nUpdated " << target->getName()
                      << " | New stock: " << target->getQuantityLeft() << "\n";
            continue;
        }

        std::cout << "\nERROR: Unknown command.\n";
    }
}

// Manager-only staff management.
// Keeps a simple in-memory directory (g_users). Static built-ins remain, while
// dynamically added users are heap-allocated and cleaned up when removed.
static void staffMenu()
{
    for (;;) {
        if (g_exitRequested) return;

        renderHeader("Staff Management", -1);

        // List users in a readable table.
        std::cout << std::left
                  << std::setw(8)  << "ID"
                  << std::setw(24) << "Name"
                  << std::setw(12) << "Role"
                  << std::setw(32) << "Email"
                  << "\n";
        for (Staff* u : g_users) {
            if (!u) continue;
            const bool isMgr = u->isManager();
            std::cout << std::left
                      << std::setw(8)  << u->getId()
                      << std::setw(24) << u->getUserName()
                      << std::setw(12) << (isMgr ? "Manager" : "Staff")
                      << std::setw(32) << u->getEmail()
                      << "\n";
        }

        renderDivider();
        std::cout << " Commands:\n";
        std::cout << "  add <id> <name> <password> <email> <role>\n";
        std::cout << "     - role: manager | staff\n";
        std::cout << "  edit <id> [name=<n>] [password=<p>] [email=<e>] [role=manager|staff]\n";
        std::cout << "  remove <id>\n";
        std::cout << "  back\n";
        std::cout << "  exit\n";
        renderDivider();

        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::string lower = toLower(trim(input));
        if (lower == "exit") { g_exitRequested = true; return; }
        if (lower == "back") { return; }

        // Minimal tokenizer: verb followed by args. Quotes are not supported.
        std::istringstream iss(input);
        std::string verb;
        if (!(iss >> verb)) { std::cout << "\nERROR: Invalid command.\n"; continue; }
        std::string verbLower = toLower(verb);

        if (verbLower == "add") {
            int id; std::string name, password, email, role;
            if (!(iss >> id >> name >> password >> email >> role)) {
                std::cout << "\nERROR: Usage: add <id> <name> <password> <email> <role>\n";
                continue;
            }
            bool wantManager = (toLower(role) == "manager");
            if (!wantManager && toLower(role) != "staff") {
                std::cout << "\nERROR: Role must be 'manager' or 'staff'.\n";
                continue;
            }
            Staff* created = addStaff(id, name, password, email, wantManager);
            if (!created) {
                std::cout << "\nERROR: ID or name already exists.\n";
                continue;
            }
            std::cout << "\nAdded " << (wantManager ? "Manager" : "Staff")
                      << " '" << name << "' (ID " << id << ").\n";
            continue;
        }

        if (verbLower == "remove") {
            int id = -1;
            if (!(iss >> id)) {
                std::cout << "\nERROR: Usage: remove <id>\n";
                continue;
            }
            if (!removeUserById(id)) {
                std::cout << "\nERROR: User not found or cannot be removed.\n";
            } else {
                std::cout << "\nRemoved user with ID " << id << ".\n";
            }
            continue;
        }

        if (verbLower == "edit") {
            int id = -1;
            if (!(iss >> id)) {
                std::cout << "\nERROR: Usage: edit <id> [name=<n>] [password=<p>] [email=<e>] [role=manager|staff]\n";
                continue;
            }
            Staff* u = findUserById(id);
            if (!u) { std::cout << "\nERROR: User not found.\n"; continue; }

            // Accept key=value pairs to update fields.
            std::string kv;
            bool roleChangeToManager = u->isManager();
            bool roleChangeRequested = false;
            std::string newName, newPwd, newEmail;

            while (iss >> kv) {
                auto pos = kv.find('=');
                if (pos == std::string::npos) continue;
                std::string key = toLower(kv.substr(0, pos));
                std::string val = kv.substr(pos + 1);
                if (key == "name") newName = val;
                else if (key == "password") newPwd = val;
                else if (key == "email") newEmail = val;
                else if (key == "role") {
                    std::string r = toLower(val);
                    if (r == "manager") { roleChangeToManager = true; roleChangeRequested = true; }
                    else if (r == "staff") { roleChangeToManager = false; roleChangeRequested = true; }
                    else { std::cout << "\nERROR: role must be 'manager' or 'staff'.\n"; }
                }
            }

            // Apply simple field edits.
            if (!newName.empty()) u->setUserName(newName);
            if (!newPwd.empty()) u->setPassword(newPwd);
            if (!newEmail.empty()) u->setEmail(newEmail);

            // To change role, we replace the instance with a new one of the desired type.
            // For built-in static users we do not delete memory; for dynamic users we do.
            if (roleChangeRequested && u->isManager() != roleChangeToManager) {
                const int oldId = u->getId();
                const std::string oldName = u->getUserName();
                const std::string oldPwd = u->getPassword();
                const std::string oldEmail = u->getEmail();

                removeUserById(oldId);
                Staff* replaced = addStaff(oldId, oldName, oldPwd, oldEmail, roleChangeToManager);
                if (!replaced) {
                    std::cout << "\nERROR: Failed to change role (ID/name conflict).\n";
                } else {
                    std::cout << "\nRole updated to " << (roleChangeToManager ? "Manager" : "Staff") << " for ID " << oldId << ".\n";
                }
            } else {
                std::cout << "\nUpdated user (ID " << u->getId() << ").\n";
            }
            continue;
        }

        std::cout << "\nERROR: Unknown command.\n";
    }
}

// Main menu loop that hosts navigation and routes into child menus.
// Important: we always check g_exitRequested after nested screens and logoff.
int main()
{
    Bill* currentBill = nullptr;
    int currentBillIndex = -1;

    // Login loop
    while (!g_exitRequested) {
        g_currentUser = nullptr;
        while (!g_exitRequested && !g_currentUser) {
            g_currentUser = tryLogin();
            if (!g_currentUser) {
                if (g_exitRequested) break;
                std::cout << " Try again.\n";
            }
        }
        if (g_exitRequested) break;

        auto& catalog = catalogItems;
        auto byScreen = groupCatalogByScreen(catalog);

        for (;;) {
            if (g_exitRequested) break;
            if (!g_currentUser) break; // user logged off -> return to login

            renderHeader("PUB POS - MAIN MENU", currentBillIndex);
            std::cout << " User: " << g_currentUser->getUserName()
                      << (g_currentUser->isManager() ? " (Manager)\n" : " (Staff)\n");
            std::cout << " [1] Select Bill\n";
            std::cout << " [2] Create New Bill\n";
            std::cout << " [3] Open Screen\n";
            std::cout << " [4] View Bills Summary\n";
            std::cout << " [5] Mark Current Bill Paid\n";
            std::cout << " [6] Logoff\n";
            if (g_currentUser && g_currentUser->isManager()) {
                std::cout << " [7] Stock Management\n";
                std::cout << " [8] Staff Management\n";
            }
            std::cout << " [0] Exit\n";
            renderDivider();

            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);
            if (input.empty()) continue;

            bool handled = handleGlobalCommand(input, currentBill, currentBillIndex, catalog);
            if (handled && toLower(input).rfind("screen ", 0) != 0) {
                if (g_exitRequested || !g_currentUser) break;
                continue;
            }

            std::string lower = toLower(input);
            if (lower == "exit" || lower == "[0]" || lower == "0") {
                g_exitRequested = true;
                renderHeader("Goodbye!", currentBillIndex);
                break;
            } else if (lower == "[6]" || lower == "6" || lower == "logoff" || lower == "logout") {
                std::cout << "\nLogging off...\n";
                g_currentUser = nullptr;
                break;
            } else if (lower == "[7]" || lower == "7") {
                if (g_currentUser && g_currentUser->isManager()) {
                    stockMenu(catalog);
                } else {
                    std::cout << "\nERROR: Only managers can manage stock.\n";
                }
                continue;
            } else if (lower == "[8]" || lower == "8") {
                if (g_currentUser && g_currentUser->isManager()) {
                    staffMenu();
                } else {
                    std::cout << "\nERROR: Only managers can manage staff.\n";
                }
                continue;
            } else if (lower == "[1]" || lower == "1") {
                Bill* chosen = nullptr;
                renderBillsSummary(currentBillIndex);
                if (!bills.empty()) {
                    std::cout << " Enter bill index or type 'back' to go back\n> ";
                    std::string sel;
                    std::getline(std::cin, sel);
                    std::string s = toLower(trim(sel));
                    if (s == "back") {
                        continue;
                    }
                    try {
                        int idx = std::stoi(sel);
                        if (idx >= 0 && idx < static_cast<int>(bills.size())) {
                            chosen = &bills[static_cast<size_t>(idx)];
                            currentBill = chosen;
                            currentBillIndex = idx;
                            std::cout << "\nCurrent bill set to #" << idx << ".\n";
                        } else {
                            std::cout << "\nERROR: Invalid bill index.\n";
                        }
                    } catch (...) {
                        std::cout << "\nERROR: Invalid input.\n";
                    }
                } else {
                    std::cout << "\nERROR: No bills to select.\n";
                }
            } else if (lower == "[2]" || lower == "2") {
                const std::string creator = g_currentUser ? g_currentUser->getUserName() : std::string{};
                bills.emplace_back(Bill{ creator });
                currentBill = &bills.back();
                currentBillIndex = static_cast<int>(bills.size()) - 1;
                std::cout << "\nCreated bill #" << currentBillIndex << " by " << (creator.empty() ? "unknown" : creator) << " and set as current.\n";
            } else if (lower == "[3]" || lower == "3" || lower.rfind("screen ", 0) == 0) {
                if (!currentBill) {
                    std::cout << "\nERROR: Select or create a bill first.\n";
                    continue;
                }

                int directScreen = -1;
                if (lower.rfind("screen ", 0) == 0) {
                    try {
                        directScreen = std::stoi(trim(lower.substr(7)));
                    } catch (...) {
                        std::cout << "\nERROR: Invalid screen number.\n";
                        directScreen = -1;
                    }
                }

                // Screen selection loop. This re-groups the catalog to reflect any runtime changes.
                for (;;) {
                    if (g_exitRequested) break;
                    if (!g_currentUser) break; // user logged off

                    auto byScreen = groupCatalogByScreen(catalog);

                    if (byScreen.empty()) {
                        std::cout << "\nERROR: No screens available (catalog empty).\n";
                        break;
                    }

                    renderHeader("Available Screens", currentBillIndex);
                    for (const auto& kv : byScreen) {
                        uint8_t sid = kv.first;
                        std::cout << " [" << static_cast<int>(sid) << "] "
                                  << "Items: " << kv.second.size() << "\n";
                    }
                    renderDivider();
                    std::cout << " Enter screen id, or [0] to go back\n> ";

                    int sid = -1;
                    if (directScreen >= 0) {
                        sid = directScreen;
                        directScreen = -1;
                        std::cout << sid << "\n";
                    } else {
                        std::string sel;
                        std::getline(std::cin, sel);
                        std::string sLower = toLower(sel);
                        if (sLower == "exit") { g_exitRequested = true; break; }
                        if (sLower == "[0]" || sLower == "0") {
                            break;
                        }
                        try {
                            sid = std::stoi(sel);
                        } catch (...) {
                            std::cout << "\nERROR: Invalid input.\n";
                            continue;
                        }
                    }

                    auto it = byScreen.find(static_cast<uint8_t>(sid));
                    if (it == byScreen.end()) {
                        std::cout << "\nERROR: Screen not found.\n";
                        continue;
                    }

                    screenMenu(currentBill, currentBillIndex, static_cast<uint8_t>(sid), it->second);
                    if (g_exitRequested || !g_currentUser) break;
                }
            } else if (lower == "[4]" || lower == "4") {
                renderBillsSummary(currentBillIndex);
                waitForEnter();
            } else if (lower == "[5]" || lower == "5") {
                if (!currentBill) {
                    std::cout << "\nERROR: No bill selected.\n";
                } else if (currentBill->isPaid()) {
                    std::cout << "\nERROR: Bill is already paid.\n";
                } else {
                    currentBill->markPaid();
                    std::cout << "\nMarked current bill as paid.\n";
                }
            } else {
                if (!handled && !handleGlobalCommand(input, currentBill, currentBillIndex, catalog)) {
                    std::cout << "\nERROR: Unknown command.\n";
                }
            }

            if (g_exitRequested) break;
        }

        if (g_exitRequested) break;
    }

    return 0;
}

