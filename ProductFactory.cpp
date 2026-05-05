#include "ProductFactory.h"
using namespace std;

Product* ProductFactory::createProduct(
    string type,
    int id,
    string name,
    double price,
    int qty)
{
    if (type == "Electronics" || type == "electronics")
        return new Product(id, name, price, qty, "Electronics");
    else if (type == "Grocery" || type == "grocery")
        return new Product(id, name, price, qty, "Grocery");
    else if (type == "Books" || type == "books")
        return new Product(id, name, price, qty, "Books");
    else if (type == "Appliances" || type == "appliances")
        return new Product(id, name, price, qty, "Appliances");
    else
        return new Product(id, name, price, qty, "General");
}
