#ifndef NOTIFPANEL_H
#define NOTIFPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>
#include <QMap>

struct NotificationItem {
    int id;
    QString type;      // "cart" or "purchase"
    QString title;
    QString message;
    QString datetime;
    int productId;
    int quantity;
    double price;
    bool isRead;

    NotificationItem() : id(0), productId(0), quantity(0), price(0), isRead(false) {}
};

class NotifPanel : public QWidget {
    Q_OBJECT
public:
    NotifPanel(QWidget* parent = nullptr);
    ~NotifPanel();

    void addCartNotification(QString productName, int quantity, double price, int productId);
    void addPurchaseNotification(QString productName, int quantity, double price);
    void togglePanel();
    void clearAll();

signals:
    void continueShoppingClicked();
    void checkoutClicked();
    void deleteFromCartClicked(int productId);
    void viewDetailsClicked();

private:
    QVBoxLayout* mainLayout;
    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* notifLayout;
    QMap<int, NotificationItem> notifications;
    int nextId;

    void refreshPanel();
    void saveNotifications();
    void loadNotifications();
    QString getTimeAgo(QString datetime);
};

#endif
