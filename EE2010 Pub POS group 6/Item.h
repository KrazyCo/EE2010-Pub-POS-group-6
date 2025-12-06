#pragma once

#include <string>
#include <cstdint>

// Base item shared by all sellable products.
// Note: items do not own memory for anything else; they hold basic info only.
class Item
{
private:
    std::string name{}; // display name
    float price{};      // unit price
    uint8_t screen{};   // grouping id for the "screens" UI
    int quantityLeft{}; // current stock

public:
    // Virtual deletor so deleting through Item* is safe.
    virtual ~Item() = default;

    Item(std::string name, float price, uint8_t screen, int quantity);

    std::string getName() const { return name; }
    float getPrice() const { return price; }
    void setPrice(float newPrice);

    uint8_t getScreen() const { return screen; }

    int getQuantityLeft() const { return quantityLeft; }
    void setQuantityLeft(int newQuantity);

    bool isOutOfStock() const { return quantityLeft <= 0; }

    // Try to decrement stock by one. Returns true if successful.
    bool tryConsumeOne();
};

