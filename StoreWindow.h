#pragma once
#include <string>
#include <vector>
    using namespace std;

class CartItem {
private:
    int productID;
    string productName;
    double price;
    int quantity;
    double subtotal;

public:
    CartItem(int pid, string pname, double pprice, int qty);
    ~CartItem();

    int getProductID() const;
    string getProductName() const;
    double getPrice() const;
    int getQuantity() const;
    double getSubtotal() const;

    void setQuantity(int qty);

    void display() const;
    void display(bool showSubtotal) const;
};

class ShoppingCart {
private:
    int cartID;
    int customerID;
    vector<CartItem> items;
    double totalAmount;

    void recalculateTotal();

public:
    ShoppingCart(int cid, int custID);
    ~ShoppingCart();

    void addItem(int productID, string productName, double price, int quantity);
    void removeItem(int productID);
    void updateQuantity(int productID, int newQty);
    void clearCart();

    int getCartID() const;
    int getCustomerID() const;
    double getTotalAmount() const;
    vector<CartItem> getItems() const;
    bool isEmpty() const;
    int getQuantity(int productID) const;  // NEW

    void display() const;
    void display(string header) const;
};

class Order {
private:
    int orderID;
    int customerID;
    string status;
    double totalPrice;
    string orderDate;

public:
    Order(int id, int custID, ShoppingCart& cart, string date);
    Order(int id, int custID, double total, string date);
    ~Order();

    int getOrderID() const;
    string getStatus() const;
    double getTotalPrice() const;

    void setStatus(string s);
    void cancelOrder();
    void trackOrder() const;

    void display() const;
    void display(bool detailed) const;
};

class RecordKeeper {
protected:
    int ownerID;
public:
    RecordKeeper(int id);
    virtual ~RecordKeeper();
    virtual void displayAll() const = 0;
};

class OrderHistory : public RecordKeeper {
private:
    vector<Order> orders;
public:
    OrderHistory(int customerID);
    ~OrderHistory();

    void addOrder(Order& order);
    void displayAll() const override;
    void displayAll(string filterStatus) const;
};