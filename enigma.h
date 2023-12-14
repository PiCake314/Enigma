#ifndef ENIGMA_H
#define ENIGMA_H

#include <QObject>
#include <QMap>

class Enigma : public QObject
{
    Q_OBJECT

public:
    Enigma();

public:
    explicit Enigma(QObject *parent = nullptr);

    char handleKeyPress(char key);
    char processKey(char);
    void rotateRotors();
    void updateLampboard(char key);
    void swapLetters(char letter1, char letter2);
    char swapPlugboard(char input);
    void configurePlugboard(char letter1, char letter2);
    void setPlugboardSwap(char letter1, char letter2);
    void reset();

public slots:
    void handleRotorValueChanged(int, int);

signals:
    void rotorRotated(int, int);
    void lampboardUpdated(QMap<char, bool> lampboardState);
    void unCheck();


private:
    int rotorPositions[3];
    char prev;
    QMap<char, bool> lampboardState;
    QMap<char, char> plugboardConfig;
};


#endif // ENIGMA_H
