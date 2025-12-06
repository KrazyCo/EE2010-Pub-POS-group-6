// EE2010 Pub POS group 6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <cctype>

#include "Bill.h"
#include "Item.h"
#include "Item_config.h"

// Global exit flag to allow exiting from any context (including screen menus)
static bool g_exitRequested = false;

// ------ Helpers ------
static void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

// trim whitespace from both ends
static std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

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

// Build combined non-owning catalog from premade vectors
static std::vector<Item*> buildCatalog()
{
    std::vector<Item*> catalog;
    catalog.reserve(liqourItems.size() + draftItems.size() + foodItems.size());
    for (auto& l : liqourItems) catalog.push_back(&l);
    for (auto& d : draftItems)  catalog.push_back(&d);
    for (auto& f : foodItems)   catalog.push_back(&f);
    return catalog;
}

// Group catalog items by their screen attribute
static std::map<uint8_t, std::vector<Item*>> groupCatalogByScreen(const std::vector<Item*>& catalog)
{
    std::map<uint8_t, std::vector<Item*>> byScreen;
    for (Item* it : catalog) {
        if (!it) continue;
        byScreen[it->getScreen()].push_back(it);
    }
    return byScreen;
}

// Find item by name (case-insensitive, exact match)
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
                  << (static_cast<int>(i) == currentBillIndex ? "  <- current" : "")
                  << "\n";
    }
    renderDivider();
}

// Display current bill details
static void renderCurrentBill(const Bill* bill, int billIndex)
{
    renderHeader("Current Bill Details", billIndex);
    if (!bill) {
        std::cout << " No bill selected.\n";
        renderDivider();
        return;
    }
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

// Pause helper to wait for Enter (used after bill/bills outputs)
static void waitForEnter()
{
    std::cout << " Press Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// Parse global commands available from anywhere.
// Returns true if handled; false if not a global command.
static bool handleGlobalCommand(const std::string& input, Bill*& currentBill, int& currentBillIndex, const std::vector<Item*>& catalog)
{
    std::string cmd = toLower(input);

    // exit (global)
    if (cmd == "exit") {
        g_exitRequested = true;
        std::cout << "\nExiting...\n";
        return true;
    }

    // bill new
    if (cmd == "bill new") {
        bills.emplace_back(Bill{});
        currentBill = &bills.back();
        currentBillIndex = static_cast<int>(bills.size()) - 1;
        std::cout << "\nCreated bill #" << currentBillIndex << " and set as current.\n";
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
        // Ensure it's numeric to avoid treating "paid" as a number
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

    // screen <x>
    if (cmd.rfind("screen ", 0) == 0 && cmd.size() > 7) {
        if (!currentBill) {
            std::cout << "\nERROR: Select or create a bill first.\n";
            return true;
        }
        // Not fully handled here; caller will process screen switch.
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
        std::string name = input.substr(4); // preserve case for messages
        Item* it = findItemByName(catalog, name);
        if (!it) {
            std::cout << "\nERROR: Item not found: " << name << "\n";
            return true;
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

    return false; // not a global command
}

// Render one screen's items and allow adding to current bill with commands.
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

        // Guard against null currentBill before dereferencing
        if (!currentBill) {
            std::cout << " ERROR: No bill selected.\n";
            std::cout << " [0] Back to screens\n";
            renderDivider();
            std::cout << "> ";
            std::string dummy;
            std::getline(std::cin, dummy); // allow user to go back or type commands; [0]/0 handled below
            // Treat any input as back when no bill is selected
            return;
        }

        std::cout << " Bill total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
        std::cout << " [0] Back to screens\n";
        renderDivider();
        // Commands shown only on screen menus
        std::cout << " Commands: add <name>, add <index>, remove <name>, screen <x>, bill new, bill <x>, bill, bills, bill paid, exit\n";
        renderDivider();

        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::string lower = toLower(input);
        if (lower == "exit") { g_exitRequested = true; return; }
        if (lower == "[0]" || lower == "0") {
            return;
        }

        // screen <x> inside screen menu: jump to another screen quickly
        if (lower.rfind("screen ", 0) == 0) {
            int nextSid = -1;
            try {
                nextSid = std::stoi(trim(lower.substr(7)));
            } catch (...) {
                std::cout << "\nERROR: Invalid screen number.\n";
                continue;
            }
            auto catalog = buildCatalog();
            auto byScreen = groupCatalogByScreen(catalog);
            auto it = byScreen.find(static_cast<uint8_t>(nextSid));
            if (it == byScreen.end()) {
                std::cout << "\nERROR: Screen not found.\n";
                continue;
            }
            // Switch screen in-place
            screenId = static_cast<uint8_t>(nextSid);
            // Tail-call style: render new screen
            screenMenu(currentBill, currentBillIndex, screenId, it->second);
            return;
        }

        // Command: add by name or index
        if (lower.rfind("add ", 0) == 0 && lower.size() > 4) {
            // If bill is paid, block before any work
            if (currentBill && currentBill->isPaid()) {
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
                        if (selected) {
                            if (currentBill->addItem(*selected)) {
                                std::cout << "\nAdded: " << selected->getName()
                                          << " | New total: $" << std::fixed << std::setprecision(2) << currentBill->getTotalPrice() << "\n";
                            } else {
                                std::cout << "\nERROR: Failed to add (likely out of stock).\n";
                            }
                            addedByIndex = true;
                        }
                    } else {
                        std::cout << "\nERROR: Item not found: " << param << "\n";
                        addedByIndex = true;
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
            if (!selected) {
                std::cout << "\nERROR: Item not found: " << param << "\n";
                continue;
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
            // If bill is paid, block before any work
            if (currentBill && currentBill->isPaid()) {
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

        // NOW call global commands after local handlers
        auto catalogAll = buildCatalog();
        if (handleGlobalCommand(input, currentBill, currentBillIndex, catalogAll)) {
            if (g_exitRequested) return;
            continue;
        }

        std::cout << "\nERROR: Unknown command.\n";
    }
}

// Main UI flow: choose bill, choose screen, add items, with command parsing
int main()
{
    Bill* currentBill = nullptr;
    int currentBillIndex = -1;

    auto catalog = buildCatalog();
    auto byScreen = groupCatalogByScreen(catalog);

    for (;;) {
        if (g_exitRequested) break;

        renderHeader("PUB POS - MAIN MENU", currentBillIndex);
        std::cout << " [1] Select Bill\n";
        std::cout << " [2] Create New Bill\n";
        std::cout << " [3] Open Screen\n";
        std::cout << " [4] View Bills Summary\n";
        std::cout << " [5] Mark Current Bill Paid\n";
        std::cout << " [0] Exit\n";
        renderDivider();

        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        // Global commands still work, but not shown in main menu
        bool handled = handleGlobalCommand(input, currentBill, currentBillIndex, catalog);
        // Prevent duplicate prints; if handled and not a 'screen' command, short-circuit
        if (handled && toLower(input).rfind("screen ", 0) != 0) {
            if (g_exitRequested) break;
            continue;
        }

        std::string lower = toLower(input);
        if (lower == "exit" || lower == "[0]" || lower == "0") {
            renderHeader("Goodbye!", currentBillIndex);
            break;
        } else if (lower == "[1]" || lower == "1") {
            Bill* chosen = nullptr;
            renderBillsSummary(currentBillIndex);
            if (!bills.empty()) {
                std::cout << " Enter bill index or [0] to go back\n> ";
                std::string sel;
                std::getline(std::cin, sel);
                if (toLower(sel) == "[0]" || toLower(sel) == "0") {
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
            bills.emplace_back(Bill{});
            currentBill = &bills.back();
            currentBillIndex = static_cast<int>(bills.size()) - 1;
            std::cout << "\nCreated bill #" << currentBillIndex << " and set as current.\n";
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

            for (;;) {
                if (g_exitRequested) break;

                catalog = buildCatalog();
                byScreen = groupCatalogByScreen(catalog);

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
                if (g_exitRequested) break;
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

    return 0;
}

