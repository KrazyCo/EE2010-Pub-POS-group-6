#pragma once

#include <string>
#include <vector>
#include <ctime> // time_t

#include "Item.h"

class Bill
{
private:
    std::string serverName{};              // staff who created the bill
    std::vector<Item*> itemsOrdered{};     // non-owning pointers
    float totalPrice{};                    // total price of the bill
    bool paid{};                           // whether the bill has been paid
    bool fulfilled{};                      // whether the order has been fulfilled
    float discount{};                      // discount applied to the bill if any
    time_t timeOpened{};                   // when the bill was opened
    time_t timePaid{};                     // when the bill was marked paid (0 if not paid)

public:
    // Construct a bill with the creating staff's name; captures open time.
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

    // Non-owning: references existing catalog items (Drink/Food/Liquor/etc.)
    bool addItem(Item& item);

    // Remove one occurrence of the item from the bill and restore stock by one.
    // Returns true if an occurrence was found and removed.
    bool removeItem(Item& item);

    float getTotalPrice() const { return totalPrice; }
    const std::vector<Item*>& getItems() const { return itemsOrdered; }

    // Creator and timestamps
    const std::string& getServerName() const { return serverName; }
    time_t getTimeOpened() const { return timeOpened; }
    time_t getTimePaid() const { return timePaid; }

    // Paid status
    bool isPaid() const { return paid; }

    void markPaid() {
        paid = true;
        timePaid = std::time(nullptr);
    }
};

// Declare the global bills container (defined in Bill.cpp)
extern std::vector<Bill> bills;

