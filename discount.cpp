#include "Discount.h"
#include <stdexcept>

Discount::Discount(int Code, double Percent) {
    if (Percent < 0 || Percent > 100)
        throw ("Discount percent must be between 0 and 100");

    CouponCode = Code;
    DiscountPercent = Percent;
}

double Discount::applyDiscount(double Price) {
    if (Price < 0)
        throw ("Price cannot be negative");

    return Price - (Price * DiscountPercent / 100);
}

int Discount::getCouponCode()const {
    return CouponCode;
}

double Discount::getDiscountPercent()const {
    return DiscountPercent;
}
