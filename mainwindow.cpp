#include "mainwindow.h"
#include <QtWidgets>
#include <QMainWindow>
#include <iostream>
#include <fstream>
#include <string>

MainWindow::MainWindow()

{
    MainWindow::state = EEstate::unloaded;

    // Set layout in QWidget
    QWidget *window = new QWidget();

    // Set layout
    QGridLayout *layout = new QGridLayout(window);

    QLabel *infolabel = new QLabel;
    infolabel->setText("Select the file to patch (EE-AOC.exe / Empire Earth.exe)");
    layout->addWidget(infolabel,0,0,1,2);

    QPushButton *loadBtn = new QPushButton;
    loadBtn->setText("open File");
    layout->addWidget(loadBtn,1,0,1,2);

    fileLabel = new QLabel;
    fileLabel->setText("No file loaded");
    layout->addWidget(fileLabel,2,0,1,2);

    // Zoom Input
    QLabel *zoomEditInfo = new QLabel;
    zoomEditInfo->setText("Zoom (lower=further away), (default: -20.5)");
    layout->addWidget(zoomEditInfo,3,0,1,1);

    zoomEdit = new QLineEdit;
    //zoomEdit->setText(QString::fromStdString(std::to_string(-20.5)));
    zoomEdit->setText(QString::number(-20.5));
    layout->addWidget(zoomEdit,3,1,1,1);

    // Cull/Fog Input
    QLabel *cullEditInfo = new QLabel;
    cullEditInfo->setText("Cull (lower=more fog, faster), (default: 35)");
    layout->addWidget(cullEditInfo,4,0,1,1);

    cullEdit = new QLineEdit;
    //cullEdit->setText(QString::fromStdString(std::to_string(35.0)));
    cullEdit->setText(QString::number(35.0));
    layout->addWidget(cullEdit,4,1,1,1);

    // Save Button
    saveBtn = new QPushButton;
    saveBtn->setText("Write values");
    layout->addWidget(saveBtn,5,0,1,2);

    saveDefaultBtn = new QPushButton;
    saveDefaultBtn->setText("Write default values");
    layout->addWidget(saveDefaultBtn,6,0,1,2);

    // Set QWidget as the central layout of the main window
    setCentralWidget(window);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
    setUnifiedTitleAndToolBarOnMac(true);

    connect( loadBtn, SIGNAL(clicked()),this,SLOT(openFileClicked()));
    connect( saveBtn, SIGNAL(clicked()),this,SLOT(saveClicked()));
    connect( saveDefaultBtn, SIGNAL(clicked()),this,SLOT(saveDefaultClicked()));
    updateByState();

}

void MainWindow::updateByState(){
    QString stateStringEE;
    float zoomValue = defaultZoom;
    float cullValue = defaultCull;

    switch (state) {
    case unloaded:
        zoomEdit->setEnabled(false);
        cullEdit->setEnabled(false);
        saveBtn->setEnabled(false);
        saveDefaultBtn->setEnabled(false);

        fileLabel->setText("No file loaded");

        zoomEdit->setText(QString::number(zoomValue));
        cullEdit->setText(QString::number(cullValue));

        break;
    case loadedEE:
        zoomEdit->setEnabled(true);
        cullEdit->setEnabled(true);
        saveBtn->setEnabled(true);
        saveDefaultBtn->setEnabled(true);

        stateStringEE = QString("File EE (Empire Earth) loaded:\n");
        stateStringEE.append(path);
        fileLabel->setText(stateStringEE);

        zoomValue = *(reinterpret_cast<float*>(data.data()+EEzoomPos));
        cullValue = *(reinterpret_cast<float*>(data.data()+EEcullPos));

        zoomEdit->setText(QString::number(zoomValue));
        cullEdit->setText(QString::number(cullValue));
        break;
    case loadedEEAOC:
        zoomEdit->setEnabled(true);
        cullEdit->setEnabled(true);
        saveBtn->setEnabled(true);
        saveDefaultBtn->setEnabled(true);

        stateStringEE = QString("File EE-AOC (Empire Earth - The Art of Conquest) loaded:\n");
        stateStringEE.append(path);
        fileLabel->setText(stateStringEE);

        zoomValue = *(reinterpret_cast<float*>(data.data()+EEAOCzoomPos));
        cullValue = *(reinterpret_cast<float*>(data.data()+EEAOCcullPos));

        zoomEdit->setText(QString::number(zoomValue));
        cullEdit->setText(QString::number(cullValue));

        break;
    case loadedNeoEE:
        zoomEdit->setEnabled(true);
        cullEdit->setEnabled(true);
        saveBtn->setEnabled(true);
        saveDefaultBtn->setEnabled(true);

        stateStringEE = QString("File EE (NeoEE Empire Earth) loaded:\n");
        stateStringEE.append(path);
        fileLabel->setText(stateStringEE);

        zoomValue = *(reinterpret_cast<float*>(data.data()+NeoEEzoomPos));
        cullValue = *(reinterpret_cast<float*>(data.data()+NeoEEcullPos));

        zoomEdit->setText(QString::number(zoomValue));
        cullEdit->setText(QString::number(cullValue));
        break;
    case loadedNeoEEAOC:
        zoomEdit->setEnabled(true);
        cullEdit->setEnabled(true);
        saveBtn->setEnabled(true);
        saveDefaultBtn->setEnabled(true);

        stateStringEE = QString("File EE-AOC (NeoEE Empire Earth - The Art of Conquest) loaded:\n");
        stateStringEE.append(path);
        fileLabel->setText(stateStringEE);

        zoomValue = *(reinterpret_cast<float*>(data.data()+NeoEEAOCzoomPos));
        cullValue = *(reinterpret_cast<float*>(data.data()+NeoEEAOCcullPos));

        zoomEdit->setText(QString::number(zoomValue));
        cullEdit->setText(QString::number(cullValue));

        break;
    default:
        std::cerr << "Unknown state" << std::endl;
        break;
    }
}

void MainWindow::openFileClicked() {
    QString filepath = QFileDialog::getOpenFileName();
    std::cout << "Try to open file from: " << filepath.toStdString() << std::endl;

    std::ifstream inputFile (filepath.toStdString(),std::ios::binary);
    if(inputFile.good()){
        // read file
        std::vector<char> const file(
            (std::istreambuf_iterator<char>(inputFile)),
            (std::istreambuf_iterator<char>())
            );

        MainWindow::path = filepath;
        MainWindow::data = file;

        if (file.size() == EELength) {
            std::cout << "File is Empire Earth" << std::endl;
            MainWindow::state = EEstate::loadedEE;

        } else if (file.size() == EEAOCLength) {
            std::cout << "File is Empire Earth Art of Conquest" << std::endl;
            MainWindow::state = EEstate::loadedEEAOC;

        } else if (file.size() == NeoEELength) {
            std::cout << "File is Empire Earth (NeoEE)" << std::endl;
            MainWindow::state = EEstate::loadedNeoEE;

        } else if (file.size() == NeoEEAOCLength) {
            std::cout << "File is Empire Earth Art of Conquest (NeoEE)" << std::endl;
            MainWindow::state = EEstate::loadedNeoEEAOC;

        } else {
            std::cout << "Unknown file with size " << std::to_string(file.size()) << std::endl;
            printErrorMessage("Unknown file! A file of the following size is expected: 6321152, 6319567, 12657664, 12062720 bytes");
            state = EEstate::unloaded;
        }
    } else{
        printErrorMessage("Error while reading the file");
        state = EEstate::unloaded;
    }
    inputFile.close();
    updateByState();
}

void MainWindow::saveClicked() {
    float zoomValue = defaultZoom;
    float cullValue = defaultCull;
    bool validZoom = true;
    bool validCull = true;

    // check text fields
    try {
        zoomValue = std::stof(zoomEdit->text().toStdString());
    } catch (...) {
        validZoom = false;
    }
    try {
        cullValue = std::stof(cullEdit->text().toStdString());
    } catch (...) {
        validCull = false;
    }

    if (validZoom && validCull) {
        overwriteDataArray(zoomValue,cullValue);

        // Write data array to file
        writeFile();

    } else {
        QString err = "Invalid values.";
        if (!validZoom)
            err.append("\nZoom value is invalid");
        if (!validCull)
            err.append("\nCull value is invalid");

        printErrorMessage(err);
    }

    updateByState();
}

void MainWindow::saveDefaultClicked(){
    overwriteDataArray(defaultZoom,defaultCull);
    // Write data array to file
    writeFile();

    updateByState();
}

void MainWindow::overwriteDataArray(float zoom, float cull){
    float *zoomPtr = nullptr;
    float *cullPtr = nullptr;
    // find float positions in std::vector data
    if (state == EEstate::loadedEE){
        zoomPtr = reinterpret_cast<float*>(data.data()+EEzoomPos);
        cullPtr = reinterpret_cast<float*>(data.data()+EEcullPos);
    }
    if (state == EEstate::loadedEEAOC){
        zoomPtr = reinterpret_cast<float*>(data.data()+EEAOCzoomPos);
        cullPtr = reinterpret_cast<float*>(data.data()+EEAOCcullPos);
    }
    // Overwrite floats in loaded data array
    *zoomPtr = zoom;
    *cullPtr = cull;
}

void MainWindow::writeFile(){
    // try to write file
    std::ofstream outFile (path.toStdString(),std::ios::out | std::ios::binary);
    if (outFile.good()){

        outFile.write(data.data(),data.size());
        outFile.flush();
        printInfoMessage("File was successfully patched!");
    } else {
        printErrorMessage("Error during file writing.","You can try to run this program as run as administrator.");
    }
    outFile.close();
}

void MainWindow::printErrorMessage(QString msg,QString detailMsg){
    QMessageBox errMsg;
    errMsg.setIcon(QMessageBox::Warning);
    if (detailMsg != nullptr)
        errMsg.setInformativeText(detailMsg);
    errMsg.setText(msg);
    errMsg.exec();
}

void MainWindow::printInfoMessage(QString msg,QString detailMsg){
    QMessageBox infoMsg;
    infoMsg.setIcon(QMessageBox::Information);
    if (detailMsg != nullptr)
        infoMsg.setInformativeText(detailMsg);
    infoMsg.setText(msg);
    infoMsg.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    //event->ignore();
}
