#include "NotifPanel.h"
#include <QScrollBar>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

NotifPanel::NotifPanel(QWidget* parent) : QWidget(parent), nextId(1) {
    setFixedWidth(380);
    setStyleSheet(
        "QWidget {"
        "  background-color: #16213e;"
        "  border-radius: 12px;"
        "}"
        );

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Header
    QWidget* header = new QWidget();
    header->setFixedHeight(50);
    header->setStyleSheet("background-color: #0f3460; border-radius: 12px 12px 0 0;");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);

    QLabel* title = new QLabel("🔔 Notifications");
    title->setStyleSheet("color: #e94560; font-size: 16px; font-weight: bold; border: none;");
    headerLayout->addWidget(title);

    headerLayout->addStretch();

    QPushButton* clearBtn = new QPushButton("Clear all");
    clearBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: #a8a8b3;"
        "  font-size: 11px;"
        "  border: none;"
        "}"
        "QPushButton:hover { color: #e94560; }"
        );
    connect(clearBtn, &QPushButton::clicked, this, &NotifPanel::clearAll);
    headerLayout->addWidget(clearBtn);

    mainLayout->addWidget(header);

    // Scroll Area
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea { background-color: #16213e; border: none; }"
        "QScrollBar:vertical {"
        "  background-color: #0f3460;"
        "  width: 8px;"
        "  border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background-color: #e94560;"
        "  border-radius: 4px;"
        "}"
        );

    scrollWidget = new QWidget();
    notifLayout = new QVBoxLayout(scrollWidget);
    notifLayout->setContentsMargins(10, 10, 10, 10);
    notifLayout->setSpacing(10);
    notifLayout->addStretch();

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    loadNotifications();
    refreshPanel();
    hide();
}

NotifPanel::~NotifPanel() {
    saveNotifications();
}

void NotifPanel::addCartNotification(QString productName, int quantity, double price, int productId) {
    NotificationItem notif;
    notif.id = nextId++;
    notif.type = "cart";
    notif.title = "🛒 Added to Cart";
    notif.message = QString("You added %1 x%2 to your cart ($%3)")
                        .arg(productName)
                        .arg(quantity)
                        .arg(price * quantity, 0, 'f', 2);
    notif.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    notif.productId = productId;
    notif.quantity = quantity;
    notif.price = price;
    notif.isRead = false;

    notifications[notif.id] = notif;
    refreshPanel();
    saveNotifications();
}

void NotifPanel::addPurchaseNotification(QString productName, int quantity, double price) {
    NotificationItem notif;
    notif.id = nextId++;
    notif.type = "purchase";
    notif.title = "✅ Purchase Successful";
    notif.message = QString("You purchased %1 x%2 for $%3 on %4")
                        .arg(productName)
                        .arg(quantity)
                        .arg(price * quantity, 0, 'f', 2)
                        .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy"));
    notif.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    notif.productId = 0;
    notif.quantity = quantity;
    notif.price = price;
    notif.isRead = false;

    notifications[notif.id] = notif;
    refreshPanel();
    saveNotifications();
}

void NotifPanel::refreshPanel() {
    // Clear existing (keep stretch)
    while (notifLayout->count() > 1) {
        QLayoutItem* item = notifLayout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (notifications.isEmpty()) {
        QLabel* empty = new QLabel("📭 No notifications yet");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("color: #a8a8b3; font-size: 13px; padding: 30px; border: none;");
        notifLayout->insertWidget(0, empty);
    } else {
        // Show newest first
        QList<int> ids = notifications.keys();
        std::sort(ids.begin(), ids.end(), std::greater<int>());

        int count = 0;
        for (int id : ids) {
            if (count >= 10) break;
            NotificationItem& n = notifications[id];

            QWidget* card = new QWidget();
            card->setStyleSheet(
                "QWidget {"
                "  background-color: #0f3460;"
                "  border-radius: 10px;"
                "  border: none;"
                "}"
                );

            QVBoxLayout* cardLayout = new QVBoxLayout(card);
            cardLayout->setSpacing(8);
            cardLayout->setContentsMargins(12, 12, 12, 12);

            // Title
            QLabel* titleLabel = new QLabel(n.title);
            titleLabel->setStyleSheet("color: #e94560; font-size: 13px; font-weight: bold; border: none;");
            cardLayout->addWidget(titleLabel);

            // Message
            QLabel* msgLabel = new QLabel(n.message);
            msgLabel->setWordWrap(true);
            msgLabel->setStyleSheet("color: white; font-size: 12px; border: none;");
            cardLayout->addWidget(msgLabel);

            // Time
            QLabel* timeLabel = new QLabel(getTimeAgo(n.datetime));
            timeLabel->setStyleSheet("color: #a8a8b3; font-size: 10px; border: none;");
            cardLayout->addWidget(timeLabel);

            // Action Buttons based on type
            if (n.type == "cart") {
                QHBoxLayout* btnLayout = new QHBoxLayout();
                btnLayout->setSpacing(8);

                QPushButton* continueBtn = new QPushButton("Continue");
                continueBtn->setStyleSheet(
                    "QPushButton {"
                    "  background-color: #e94560;"
                    "  color: white;"
                    "  border-radius: 6px;"
                    "  padding: 5px 10px;"
                    "  font-size: 11px;"
                    "  border: none;"
                    "}"
                    "QPushButton:hover { background-color: #c73652; }"
                    );
                connect(continueBtn, &QPushButton::clicked, this, &NotifPanel::continueShoppingClicked);

                QPushButton* checkoutBtn = new QPushButton("Checkout");
                checkoutBtn->setStyleSheet(
                    "QPushButton {"
                    "  background-color: transparent;"
                    "  color: #e94560;"
                    "  border: 1px solid #e94560;"
                    "  border-radius: 6px;"
                    "  padding: 5px 10px;"
                    "  font-size: 11px;"
                    "}"
                    "QPushButton:hover { background-color: #e94560; color: white; }"
                    );
                connect(checkoutBtn, &QPushButton::clicked, this, &NotifPanel::checkoutClicked);

                QPushButton* deleteBtn = new QPushButton("Delete");
                deleteBtn->setStyleSheet(
                    "QPushButton {"
                    "  background-color: transparent;"
                    "  color: #ff6b6b;"
                    "  border: 1px solid #ff6b6b;"
                    "  border-radius: 6px;"
                    "  padding: 5px 10px;"
                    "  font-size: 11px;"
                    "}"
                    "QPushButton:hover { background-color: #ff6b6b; color: white; }"
                    );
                connect(deleteBtn, &QPushButton::clicked, [this, n]() {
                    emit deleteFromCartClicked(n.productId);
                });

                btnLayout->addWidget(continueBtn);
                btnLayout->addWidget(checkoutBtn);
                btnLayout->addWidget(deleteBtn);
                cardLayout->addLayout(btnLayout);

            } else if (n.type == "purchase") {
                QHBoxLayout* btnLayout = new QHBoxLayout();

                QPushButton* detailsBtn = new QPushButton("View Details →");
                detailsBtn->setStyleSheet(
                    "QPushButton {"
                    "  background-color: transparent;"
                    "  color: #e94560;"
                    "  font-size: 11px;"
                    "  border: none;"
                    "}"
                    "QPushButton:hover { text-decoration: underline; }"
                    );
                connect(detailsBtn, &QPushButton::clicked, this, &NotifPanel::viewDetailsClicked);

                btnLayout->addStretch();
                btnLayout->addWidget(detailsBtn);
                cardLayout->addLayout(btnLayout);
            }

            if (!n.isRead) {
                card->setStyleSheet(card->styleSheet() + " QWidget { border-left: 3px solid #e94560; }");
            }

            notifLayout->insertWidget(0, card);
            count++;
        }
    }

    // Update height
    int height = 60 + (notifLayout->count() * 130);
    setFixedHeight(qMin(height, 500));
}

void NotifPanel::togglePanel() {
    if (isVisible()) {
        hide();
    } else {
        show();
        raise();
    }
}

void NotifPanel::clearAll() {
    notifications.clear();
    refreshPanel();
    saveNotifications();
}

void NotifPanel::saveNotifications() {
    QFile file("notifications.json");
    if (!file.open(QIODevice::WriteOnly)) return;

    QJsonArray jsonArray;
    for (auto& n : notifications) {
        QJsonObject obj;
        obj["id"] = n.id;
        obj["type"] = n.type;
        obj["title"] = n.title;
        obj["message"] = n.message;
        obj["datetime"] = n.datetime;
        obj["productId"] = n.productId;
        obj["quantity"] = n.quantity;
        obj["price"] = n.price;
        obj["isRead"] = n.isRead;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
}

void NotifPanel::loadNotifications() {
    QFile file("notifications.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    notifications.clear();
    for (auto val : doc.array()) {
        QJsonObject obj = val.toObject();
        NotificationItem n;
        n.id = obj["id"].toInt();
        n.type = obj["type"].toString();
        n.title = obj["title"].toString();
        n.message = obj["message"].toString();
        n.datetime = obj["datetime"].toString();
        n.productId = obj["productId"].toInt();
        n.quantity = obj["quantity"].toInt();
        n.price = obj["price"].toDouble();
        n.isRead = obj["isRead"].toBool();

        notifications[n.id] = n;
        if (n.id >= nextId) nextId = n.id + 1;
    }
}

QString NotifPanel::getTimeAgo(QString datetime) {
    QDateTime notifTime = QDateTime::fromString(datetime, "yyyy-MM-dd hh:mm:ss");
    qint64 seconds = notifTime.secsTo(QDateTime::currentDateTime());

    if (seconds < 60) return "Just now";
    if (seconds < 3600) return QString("%1 minutes ago").arg(seconds / 60);
    if (seconds < 86400) return QString("%1 hours ago").arg(seconds / 3600);
    return QString("%1 days ago").arg(seconds / 86400);
}