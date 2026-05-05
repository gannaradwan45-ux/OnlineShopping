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
        return new Product(id, name, price, "Electronics", qty);

    else if (type == "Grocery" || type == "grocery")
        return new Product(id, name, price, "Grocery", qty);

    else if (type == "Books" || type == "books")
        return new Product(id, name, price, "Books", qty);

    else if (type == "Appliances" || type == "appliances")
        return new Product(id, name, price, "Appliances", qty);

    else
        return new Product(id, name, price, "General", qty);
}