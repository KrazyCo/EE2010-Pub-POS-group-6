#pragma once

#include <string>
#include <cstdint>

class Item
{
private:
    std::string name{}; // name of the item
    float price{}; // price of the item
    uint8_t screen{}; // screen on the GUI
    int quantityLeft{}; // remaining stock of this item
public:
    // Ensure safe polymorphic deletion when used via base pointers
    virtual ~Item() = default;

    Item(std::string name, float price, uint8_t screen, int quantity);

    std::string  getName() const { return name; }
    float getPrice() const { return price; }
    void setPrice(float newPrice);

    uint8_t getScreen() const { return screen; }

    int getQuantityLeft() const { return quantityLeft; }
    void setQuantityLeft(int newQuantity);

    bool isOutOfStock() const { return quantityLeft <= 0; }

    // Decrements stock by one if available; returns true if decremented
    bool tryConsumeOne();
};

