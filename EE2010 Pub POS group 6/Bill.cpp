#include "Bill.h"

std::vector<Bill> bills{}; // single definition

bool Bill::addItem(Item& item)
{
    // Attempt to consume one from stock
    if (!item.tryConsumeOne()) {
        return false; // out of stock
    }

    totalPrice += item.getPrice();
    itemsOrdered.emplace_back(&item); // store non-owning pointer, keeps polymorphism
    return true;
}

bool Bill::removeItem(Item& item)
{
    // Find one occurrence of the exact item pointer
    for (auto it = itemsOrdered.begin(); it != itemsOrdered.end(); ++it) {
        if (*it == &item) {
            // Restore stock by one
            item.setQuantityLeft(item.getQuantityLeft() + 1);

            // Adjust total
            totalPrice -= item.getPrice();
            if (totalPrice < 0.0f) {
                totalPrice = 0.0f; // guard against negative due to floating errors
            }

            // Remove from bill
            itemsOrdered.erase(it);
            return true;
        }
    }
    return false; // not found on this bill
}
