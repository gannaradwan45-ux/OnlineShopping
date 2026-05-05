#pragma once
#include <string>

class Discount {
private:
    int CouponCode;
    double DiscountPercent;

public:
    Discount(int Code, double Percent);
    double applyDiscount(double Price);
    int getCouponCode()const;
    double getDiscountPercent()const;
};
