#pragma once

#include <string>
#include <cstdint>
#include "2dUtils.h"

class Item
{
private:
    std::string name{}; // name of the item
    float price{}; // price of the item
    uint8_t screen{}; // screen on the GUI
    coord2d position{}; // position on the screen
    span2d span{}; // size on the screen
    int quantityLeft{}; // remaining stock of this item
public:
    // Ensure safe polymorphic deletion when used via base pointers
    virtual ~Item() = default;

    Item(std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity);

    std::string  getName() const { return name; }
    float getPrice() const { return price; }
    void setPrice(float newPrice);

    uint8_t getScreen() const { return screen; }
    coord2d getPosition() const { return position; }
    span2d getSpan() const { return span; }

    int getQuantityLeft() const { return quantityLeft; }
    void setQuantityLeft(int newQuantity);

    bool isOutOfStock() const { return quantityLeft <= 0; }

    // Decrements stock by one if available; returns true if decremented
    bool tryConsumeOne();
};

