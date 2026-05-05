#include "OrderCommand.h"
using namespace std;

// ================================
// Checkout Command
// ================================
CheckoutCommand::CheckoutCommand(ShoppingCart& c) : cart(c) {
    status = "Pending";
}

void CheckoutCommand::execute() {
    if (cart.isEmpty()) {
        cout << ">> [Checkout] Cart is empty!" << endl;
        return;
    }
    status = "Checked Out";
    cout << ">> [Checkout] Order placed successfully!" << endl;
    cout << "   Total: $" << cart.getTotalAmount() << endl;
    cart.display();
}

void CheckoutCommand::undo() {
    status = "Pending";
    cout << ">> [Checkout UNDO] Order cancelled!" << endl;
    cart.clearCart();
}

// ================================
// Cancel Order Command
// ================================
CancelOrderCommand::CancelOrderCommand(int id) {
    orderID = id;
    previousStatus = "Pending";
    currentStatus = "Pending";
}

void CancelOrderCommand::execute() {
    if (currentStatus == "Delivered") {
        cout << ">> [Cancel] Cannot cancel delivered order!"
            << endl;
        return;
    }
    previousStatus = currentStatus;
    currentStatus = "Cancelled";
    cout << ">> [Cancel] Order #" << orderID
        << " cancelled successfully!" << endl;
}

void CancelOrderCommand::undo() {
    currentStatus = previousStatus;
    cout << ">> [Cancel UNDO] Order #" << orderID
        << " restored to: " << currentStatus << endl;
}