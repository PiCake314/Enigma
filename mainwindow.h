#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDial>
#include <QPushButton>
#include <QComboBox>
#include "enigma.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    Enigma enigma;
    QMap<char, QComboBox*> letterComboBoxes;

    void rotateDial(QDial*, int angle);

    void createSwapGrid();


protected:
    void keyPressEvent(QKeyEvent* e) override;

public slots:
    void handleRotorRotated(int rotorNumber, int newPosition);
    void handleLampboardUpdated(QMap<char, bool> lampboardState);
    void handleApplyClicked();
    void handleClearClicked();
    void handleResetClicked();
    void handleComboBoxIndexChanged(int);
    void unCheckButtons();
    void handleDialValueChanged(int);

signals:
    void handleDialIncr(int, int);

};

#endif // MAINWINDOW_H
