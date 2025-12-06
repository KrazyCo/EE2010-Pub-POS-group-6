#pragma once

#include <string>
#include <vector>
#include <ctime> // time_t

#include "Item.h"

class Bill
{
private:
    std::string serverName{}; // name of the server
    std::vector<Item*> itemsOrdered{}; // non-owning polymorphic pointers - object slices class specific attributes but not needed after adding to bill
    float totalPrice{}; // total price of the bill
    bool paid{}; // whether the bill has been paid
    bool fulfilled{}; // whether the order has been fulfilled
    float discount{}; // discount applied to the bill if any
    time_t timeOpened{}; // time when the bill was opened
public:
    // Non-owning: references existing catalog items (Drink/Food/Liquor/etc.)
    bool addItem(Item& item);

    // Remove one occurrence of the item from the bill and restore stock by one.
    // Returns true if an occurrence was found and removed.
    bool removeItem(Item& item);

    float getTotalPrice() const {
        return totalPrice;
    }

    const std::vector<Item*>& getItems() const {
        return itemsOrdered;
    }

    // Paid status
    bool isPaid() const {
        return paid;
    }

    void markPaid() {
        paid = true;
    }
};

// Declare the global bills container (defined in Bill.cpp)
extern std::vector<Bill> bills;

