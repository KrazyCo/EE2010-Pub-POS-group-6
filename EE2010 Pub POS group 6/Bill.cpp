#include "Bill.h"
#include <ctime>

std::vector<Bill> bills{}; // single definition

bool Bill::addItem(Item& item)
{
    // Try to take one from stock first.
    if (!item.tryConsumeOne()) {
        return false; // out of stock
    }

    totalPrice += item.getPrice();
    itemsOrdered.emplace_back(&item); // keep pointer to the catalog item
    return true;
}

bool Bill::removeItem(Item& item)
{
    // Remove a single matching pointer and fix up stock and total.
    for (auto it = itemsOrdered.begin(); it != itemsOrdered.end(); ++it) {
        if (*it == &item) {
            item.setQuantityLeft(item.getQuantityLeft() + 1);

            totalPrice -= item.getPrice();
            if (totalPrice < 0.0f) {
                totalPrice = 0.0f; // defensive clamp
            }

            itemsOrdered.erase(it);
            return true;
        }
    }
    return false; // nothing matched
}
