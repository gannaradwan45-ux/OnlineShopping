#ifndef RECEIPTWINDOW_H
#define RECEIPTWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class ReceiptWindow : public QMainWindow {
    Q_OBJECT
public:
    ReceiptWindow(QString receiptText, QWidget* parent = nullptr);
    
private slots:
    void onClose();
    
private:
    QTextEdit* receiptDisplay;
    QPushButton* closeBtn;
};

#endif
