#include "ReceiptWindow.h"
#include <QFont>
#include <QScrollBar>

ReceiptWindow::ReceiptWindow(QString receiptText, QWidget* parent)
    : QMainWindow(parent) {
    
    setWindowTitle("🧾 Purchase Receipt");
    setMinimumSize(500, 700);
    
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");
    
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    
    receiptDisplay = new QTextEdit();
    receiptDisplay->setReadOnly(true);
    receiptDisplay->setPlainText(receiptText);
    receiptDisplay->setFont(QFont("Courier New", 12));
    receiptDisplay->setStyleSheet(
        "QTextEdit {"
        "  background-color: #16213e;"
        "  color: #e94560;"
        "  border: 2px solid #e94560;"
        "  border-radius: 10px;"
        "  padding: 20px;"
        "}"
    );
    layout->addWidget(receiptDisplay);
    
    closeBtn = new QPushButton("✓ Close");
    closeBtn->setFixedHeight(45);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: white;"
        "  border-radius: 10px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #c73652; }"
    );
    connect(closeBtn, &QPushButton::clicked, this, &ReceiptWindow::onClose);
    layout->addWidget(closeBtn);
}

void ReceiptWindow::onClose() {
    this->close();
}
