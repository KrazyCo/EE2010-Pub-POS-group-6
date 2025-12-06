#pragma once

#include <string>
#include <vector>
#include <ctime> // time_t

#include "Item.h"

// A bill keeps references to catalog items (non-owning). It tracks totals,
// whether it is paid, who opened it, and timestamps for opened/paid times.
class Bill
{
private:
    std::string serverName{};              // staff username who created the bill
    std::vector<Item*> itemsOrdered{};     // non-owning pointers to catalog items
    float totalPrice{};                    // running total
    bool paid{};                           // true once marked as paid
    bool fulfilled{};                      // reserved for future use
    float discount{};                      // reserved for future use
    time_t timeOpened{};                   // when the bill was created
    time_t timePaid{};                     // when the bill was marked paid (0 if not paid)

public:
    // Use this to create a new bill. Captures creator id and open time.
    explicit Bill(const std::string& server)
        : serverName(server),
          totalPrice(0.0f),
          paid(false),
          fulfilled(false),
          discount(0.0f),
          timeOpened(std::time(nullptr)),
          timePaid(0)
    {
    }

    // Adds one item to the bill and decrements stock. Returns false if stock is 0.
    bool addItem(Item& item);

    // Removes one matching item pointer from the bill and restores stock by one.
    // Returns true if an occurrence was removed.
    bool removeItem(Item& item);

    float getTotalPrice() const { return totalPrice; }
    const std::vector<Item*>& getItems() const { return itemsOrdered; }

    // Creator and timestamps
    const std::string& getServerName() const { return serverName; }
    time_t getTimeOpened() const { return timeOpened; }
    time_t getTimePaid() const { return timePaid; }

    // Paid status
    bool isPaid() const { return paid; }

    // Marks the bill as paid and captures the paid timestamp.
    void markPaid() {
        paid = true;
        timePaid = std::time(nullptr);
    }
};

// Global bills container (defined in Bill.cpp)
extern std::vector<Bill> bills;

