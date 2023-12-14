#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    enigma{this}
{
    ui->setupUi(this);

    ui->rotortext_1->setText("0");
    ui->rotortext_2->setText("0");
    ui->rotortext_3->setText("0");

    connect(&enigma, &Enigma::rotorRotated, this, &MainWindow::handleRotorRotated);
    connect(&enigma, &Enigma::lampboardUpdated, this, &MainWindow::handleLampboardUpdated);
    connect(&enigma, &Enigma::unCheck, this, &MainWindow::unCheckButtons);
    // connect(&enigma, &Enigma::reset, this, &MainWindow::handleLampboardUpdated);

    ui->dial_1->setMinimum(0);
    ui->dial_2->setMinimum(0);
    ui->dial_3->setMinimum(0);

    ui->dial_1->setMaximum(25);
    ui->dial_2->setMaximum(25);
    ui->dial_3->setMaximum(25);

    connect(ui->dial_1, &QDial::valueChanged, this, &MainWindow::handleDialValueChanged);
    connect(ui->dial_2, &QDial::valueChanged, this, &MainWindow::handleDialValueChanged);
    connect(ui->dial_3, &QDial::valueChanged, this, &MainWindow::handleDialValueChanged);

    connect(this, &MainWindow::handleDialIncr, &enigma,&Enigma::handleRotorValueChanged);

    createSwapGrid();

    QPushButton* apply= new QPushButton("Apply", this);
    connect(apply, &QPushButton::clicked, this, &MainWindow::handleApplyClicked);
    ui->okClear->addWidget(apply, 26, 0);

    QPushButton* clear= new QPushButton("Clear", this);
    connect(clear, &QPushButton::clicked, this, &MainWindow::handleClearClicked);
    ui->okClear->addWidget(clear, 26, 1);

    QPushButton* reset = new QPushButton("Reset", this);
    connect(reset, &QPushButton::clicked, this, &MainWindow::handleResetClicked);
    ui->okClear->addWidget(reset, 26, 2);
}


void MainWindow::handleDialValueChanged(int value){

    QObject* senderObject = sender();


    bool oldState = senderObject->blockSignals(true);
    if (senderObject == ui->dial_1)      emit handleDialIncr(value, 0);
    else if (senderObject == ui->dial_2) emit handleDialIncr(value, 1);
    else if (senderObject == ui->dial_3) emit handleDialIncr(value, 2);

    senderObject->blockSignals(oldState);
}



void MainWindow::unCheckButtons(){

    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        QString buttonName = "Letter" + QString(letter);
        QRadioButton* button = ui->centralwidget->findChild<QRadioButton*>(buttonName);
        button->setAutoExclusive(false);
        button->setChecked(false);
        button->setAutoExclusive(true);
    }
}


void MainWindow::handleResetClicked(){
    ui->rotortext_1->setText("0");
    ui->rotortext_2->setText("0");
    ui->rotortext_3->setText("0");

    handleClearClicked();
    enigma.reset();
}


void MainWindow::handleClearClicked(){
    auto keys = letterComboBoxes.keys();
    for(char letter : keys){
        QComboBox* comboBox = letterComboBoxes.value(letter);
        comboBox->setCurrentIndex(letter - 'A');
    }

    // ui->input_text->setText("");
    // ui->output_text->setText("");
}


void MainWindow::handleApplyClicked(){
    auto keys = letterComboBoxes.keys();
    for (char letter : keys) {
        QComboBox* comboBox = letterComboBoxes.value(letter);
        char swapPartner = comboBox->currentText().at(0).toLatin1();
        enigma.setPlugboardSwap(letter, swapPartner);
    }
}


void MainWindow::createSwapGrid()
{
    int gridRow = 0;
    int gridColumn = 0;

    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        QComboBox* comboBox = new QComboBox(this);

        for (char swapPartner = 'A'; swapPartner <= 'Z'; ++swapPartner) {
            comboBox->addItem(QString(swapPartner));
        }

        comboBox->setCurrentIndex(letter - 'A'); // set the letter to itself in the beginning

        ui->swap_grid->addWidget(new QLabel(QString(letter)), gridRow, gridColumn);
        ui->swap_grid->addWidget(comboBox, gridRow, gridColumn + 1);

        letterComboBoxes[letter] = comboBox;


        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::handleComboBoxIndexChanged);

        // Move to the next row after every 3 columns
        gridColumn += 2;
        if (gridColumn > 15) {
            gridColumn = 0;
            gridRow++;
        }
    }
}



void MainWindow::handleComboBoxIndexChanged(int index){
    // TODO
}



void MainWindow::keyPressEvent(QKeyEvent* e)
{
    auto text = e->text();
    if(!text.size()) return;

    auto ch = text[0].toUpper();
    if (ch.isLetter()) {
        char key = enigma.handleKeyPress(ch.toLatin1());

        // auto text = ui->input_text->text();
        // text.append(ch);
        // ui->input_text->setText(text);


        // text = ui->output_text->text();
        // text.append(key);
        // ui->output_text->setText(text);
    }
}

void MainWindow::rotateDial(QDial* dial, int inc){
    int curr = ui->dial_1->value();
    int newV = (curr + inc) % dial->maximum();

    if (newV < 0) newV += dial->maximum();


    bool oldState = dial->blockSignals(true);
    dial->setValue(newV);
    dial->blockSignals(oldState);
}

void MainWindow::handleRotorRotated(int rotor_index, int new_pos)
{
    QLabel* rotortext = [this, rotor_index]{
        switch(rotor_index){
            case 0: return ui->rotortext_1;
            case 1: return ui->rotortext_2;
            // case 2: return ui->rotortext_3;
            default: return ui->rotortext_3;
        }
    }();

    rotortext->setText(QString::number(new_pos));


    QDial* dial = [this, rotor_index]{
        switch(rotor_index){
        case 0: return ui->dial_1;
        case 1: return ui->dial_2;
        // case 2: return ui->rotortext_3;
        default: return ui->dial_3;
        }
    }();

    bool oldState = dial->blockSignals(true);
    int dialMaximum = dial->maximum();
    int dialValue = (new_pos * dialMaximum) / 26;  // Adjust based on your actual dial setup
    dial->setValue(dialValue);
    dial->blockSignals(oldState);

}

void MainWindow::handleLampboardUpdated(QMap<char, bool> lampboardState)
{
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        QString radioButtonName = "Letter" + QString(letter);
        QRadioButton* radioButton = ui->centralwidget->findChild<QRadioButton*>(radioButtonName);
        // if (radioButton)
            radioButton->setChecked(lampboardState.value(letter));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
