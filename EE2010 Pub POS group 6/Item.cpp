#include "Item.h"

Item::Item(std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity)
	: name(std::move(name)), price(price), screen(screen), position(pos), span(span), quantityLeft(quantity) {
}

void Item::setPrice(float newPrice) {
	price = newPrice;
}

void Item::setQuantityLeft(int newQuantity) {
	quantityLeft = newQuantity < 0 ? 0 : newQuantity;
}

bool Item::tryConsumeOne() {
	if (quantityLeft > 0) {
		--quantityLeft;
		return true;
	}
	return false;
}
