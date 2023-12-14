#include "enigma.h"

#include <QDebug>

Enigma::Enigma(QObject *parent)
: QObject{parent}, prev{'A'}
{
    rotorPositions[0] = 0; // zeros for now
    rotorPositions[1] = 0;
    rotorPositions[2] = 0;

    for(char letter = 'A'; letter <= 'Z'; ++letter){
        lampboardState[letter] = false;
    }
}


void Enigma::handleRotorValueChanged(int value, int index){

    // check if the dial has moved by 1/26th of a full circle
    rotorPositions[index] = value;
    emit rotorRotated(index, rotorPositions[index]);
}

void Enigma::reset(){
    rotorPositions[0] = 0; // zeros for now
    rotorPositions[1] = 0;
    rotorPositions[2] = 0;

    for(char letter = 'A'; letter <= 'Z'; ++letter){
        lampboardState[letter] = false;
    }

    emit unCheck();
    emit rotorRotated(0, rotorPositions[0]);
    emit rotorRotated(1, rotorPositions[1]);
    emit rotorRotated(2, rotorPositions[2]);

}


char Enigma::handleKeyPress(char key){
    rotateRotors();

    key = processKey(key);

    emit lampboardUpdated(lampboardState);

    return key;
}


char Enigma::processKey(char key){
    key = swapPlugboard(key);

    for(int i = 0; i < 3; ++i){ // passign through the rotors
        key = (key - 'A' + rotorPositions[i]) + 'A';
    }

    key = ('Z' - key) + 'A'; // reflector..

    for(int i = 2; i >= 0; --i){ // passing back
        key = ((key - 'A' - rotorPositions[i]) % 26 + 26) % 26 + 'A';
    }

    key = swapPlugboard(key);

    updateLampboard(key);

    return key;
}


void Enigma::rotateRotors(){

    rotorPositions[0] = (rotorPositions[0] + 1) % 26;
    if(!rotorPositions[0]){ // only rotate the second when the first does a full revolution
        rotorPositions[1] = (rotorPositions[1] + 1) % 26;
        emit rotorRotated(1, rotorPositions[1]);
        if(!rotorPositions[1]){ // same as above..
            rotorPositions[2] = (rotorPositions[2] + 1) % 26;
            emit rotorRotated(2, rotorPositions[2]);
        }
    }

    emit rotorRotated(0, rotorPositions[0]);

}


void Enigma::setPlugboardSwap(char letter1, char letter2){
    if (letter1 < 'A' || letter1 > 'Z' || letter2 < 'A' || letter2 > 'Z') {
        return;
    }

    configurePlugboard(letter1, letter2);
}


void Enigma::configurePlugboard(char letter1, char letter2){
    plugboardConfig[letter1] = letter2;
    plugboardConfig[letter2] = letter1;
}


char Enigma::swapPlugboard(char input){
    return plugboardConfig.value(input, input);
}

void Enigma::updateLampboard(char key)
{
    lampboardState[prev] = false;
    lampboardState[key] = true;
    prev = key;
}

void Enigma::swapLetters(char letter1, char letter2)
{
    bool temp = lampboardState[letter1];
    lampboardState[letter1] = lampboardState[letter2];
    lampboardState[letter2] = temp;
}
