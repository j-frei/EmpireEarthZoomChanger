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
    layout->addWidget(infolabel,0,0,1,3);

    QPushButton *loadBtn = new QPushButton;
    loadBtn->setText("open File");
    layout->addWidget(loadBtn,1,0,1,3);

    fileString = "No file loaded";
    fileLabel = new QLabel;
    layout->addWidget(fileLabel,2,0,1,3);

    // Zoom Input
    QLabel *gameZoomEditInfo = new QLabel;
    gameZoomEditInfo->setText("Zoom (lower=further away), (default: -20.5)");
    layout->addWidget(gameZoomEditInfo,3,0,1,1);

    gameZoomStatus = new QLabel;
    layout->addWidget(gameZoomStatus,3,1,1,1);

    gameZoomChgBtn = new QPushButton;
    gameZoomChgBtn->setText(u8"🔧");
    layout->addWidget(gameZoomChgBtn,3,2,1,1);

    // ClipRear/Fog Input
    QLabel *gameClipRearEditInfo = new QLabel;
    gameClipRearEditInfo->setText("Rear Clipping (lower=more fog, faster), (default: 35)");
    layout->addWidget(gameClipRearEditInfo,4,0,1,1);

    gameClipRearStatus = new QLabel;
    layout->addWidget(gameClipRearStatus,4,1,1,1);

    gameClipRearChgBtn = new QPushButton;
    gameClipRearChgBtn->setText(u8"🔧");
    layout->addWidget(gameClipRearChgBtn,4,2,1,1);

    // Editor
    QLabel *editorInfo = new QLabel;
    editorInfo->setText("<b>Scenario Editor Settings</b>");
    layout->addWidget(editorInfo,5,0,1,3);

    // Editor Checkbox Patch
    QLabel *editorConstFltPatchCbxInfo = new QLabel;
    editorConstFltPatchCbxInfo->setText("Enable Patch for Editor modifications");
    layout->addWidget(editorConstFltPatchCbxInfo,6,0,1,1);

    editorConstFltPatchCbx = new QCheckBox;
    layout->addWidget(editorConstFltPatchCbx,6,1,1,1);

    // Editor Zoom Input
    QLabel *editorZoomEditInfo = new QLabel;
    editorZoomEditInfo->setText("Zoom (lower=further away), (default: -42.0)");
    layout->addWidget(editorZoomEditInfo,7,0,1,1);

    editorZoomStatus = new QLabel;
    layout->addWidget(editorZoomStatus,7,1,1,1);

    editorZoomChgBtn = new QPushButton;
    editorZoomChgBtn->setText(u8"🔧");
    layout->addWidget(editorZoomChgBtn,7,2,1,1);

    // Editor ClipRear/Fog Input
    QLabel *editorClipRearEditInfo = new QLabel;
    editorClipRearEditInfo->setText("Rear Clipping (lower=more fog, faster), (default: 58)");
    layout->addWidget(editorClipRearEditInfo,8,0,1,1);

    editorClipRearStatus = new QLabel;
    layout->addWidget(editorClipRearStatus,8,1,1,1);

    editorClipRearChgBtn = new QPushButton;
    editorClipRearChgBtn->setText(u8"🔧");
    layout->addWidget(editorClipRearChgBtn,8,2,1,1);

    // Restore default values
    setDefaultBtn = new QPushButton;
    setDefaultBtn->setText("Set default values");
    layout->addWidget(setDefaultBtn,9,0,1,3);

    // Save Button
    saveBtn = new QPushButton;
    saveBtn->setText("Write values");
    layout->addWidget(saveBtn,10,0,1,3);

    // Set QWidget as the central layout of the main window
    setCentralWidget(window);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
    setUnifiedTitleAndToolBarOnMac(true);

    // SIGNAL/SLOT handling
    connect( loadBtn, SIGNAL(clicked()),this,SLOT(openFileClicked()));

    connect( gameZoomChgBtn, SIGNAL(clicked()),this,SLOT(gameZoomChg()));
    connect( gameClipRearChgBtn, SIGNAL(clicked()),this,SLOT(gameClipRearChg()));

    connect( editorConstFltPatchCbx, SIGNAL(stateChanged(int)),this,SLOT(patchCbxChanged(int)));
    connect( editorZoomChgBtn, SIGNAL(clicked()),this,SLOT(editorZoomChg()));
    connect( editorClipRearChgBtn, SIGNAL(clicked()),this,SLOT(editorClipRearChg()));

    connect( setDefaultBtn, SIGNAL(clicked()),this,SLOT(setDefaultClicked()));
    connect( saveBtn, SIGNAL(clicked()),this,SLOT(saveClicked()));
    updateByState();

}

void MainWindow::updateByState(){
    QString stateStringEE;
    float gameZoomValue = defaultGameZoom;
    float gameClipRearValue = defaultGameClipRear;
    float editorConstFlt = defaultEditorConstFlt;
    float editorZoomValue = defaultEditorZoom;
    float editorClipRearValue = defaultEditorClipRear;
    bool hasEditorPatch = false;

    if (state == EEstate::unloaded) {
        gameZoomChgBtn->setEnabled(false);
        gameClipRearChgBtn->setEnabled(false);
        editorConstFltPatchCbx->setEnabled(false);
        editorZoomChgBtn->setEnabled(false);
        editorClipRearChgBtn->setEnabled(false);
        saveBtn->setEnabled(false);
        setDefaultBtn->setEnabled(false);

        fileLabel->setText(QString::fromStdString(fileString));

        gameZoomStatus->setText(QString::fromStdString(std::to_string(defaultGameZoom)));
        gameClipRearStatus->setText(QString::fromStdString(std::to_string(defaultGameClipRear)));

        editorConstFltPatchCbx->blockSignals(true);
        editorConstFltPatchCbx->setCheckState(Qt::Unchecked);
        editorConstFltPatchCbx->blockSignals(false);
        editorZoomStatus->setText(QString::fromStdString(std::to_string(defaultEditorZoom)));
        editorClipRearStatus->setText(QString::fromStdString(std::to_string(defaultEditorClipRear)));
    } else {
        gameZoomChgBtn->setEnabled(true);
        gameClipRearChgBtn->setEnabled(true);
        editorConstFltPatchCbx->setEnabled(true);

        saveBtn->setEnabled(true);
        setDefaultBtn->setEnabled(true);

        fileLabel->setText(QString::fromStdString(fileString));

        gameZoomValue = *loadedGameZoom;
        gameClipRearValue = *loadedGameClipRear;

        gameZoomStatus->setText(QString::fromStdString(std::to_string(gameZoomValue)));
        gameClipRearStatus->setText(QString::fromStdString(std::to_string(gameClipRearValue)));

        editorConstFlt = *loadedEditorConstFlt;
        hasEditorPatch = defaultGameConstFlt == editorConstFlt;

        if (hasEditorPatch) {
            editorConstFltPatchCbx->blockSignals(true);
            editorConstFltPatchCbx->setCheckState(Qt::Checked);
            editorConstFltPatchCbx->blockSignals(false);
            editorZoomChgBtn->setEnabled(true);
            editorClipRearChgBtn->setEnabled(true);
            editorZoomValue = *loadedEditorZoom;
            editorClipRearValue = *loadedEditorClipRear;
        } else {
            editorConstFltPatchCbx->blockSignals(true);
            editorConstFltPatchCbx->setCheckState(Qt::Unchecked);
            editorConstFltPatchCbx->blockSignals(false);
            editorZoomChgBtn->setEnabled(false);
            editorClipRearChgBtn->setEnabled(false);
            editorZoomValue = defaultEditorZoom;
            editorClipRearValue = defaultEditorClipRear;
        }
        editorZoomStatus->setText(QString::fromStdString(std::to_string(editorZoomValue)));
        editorClipRearStatus->setText(QString::fromStdString(std::to_string(editorClipRearValue)));
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
            state = EEstate::loadedEE;
            loadedGameZoom = reinterpret_cast<float*>(data.data()+EEGameZoomPos);
            loadedGameClipRear = reinterpret_cast<float*>(data.data()+EEGameClipRearPos);
            loadedEditorConstFlt = reinterpret_cast<float*>(data.data()+EEEditorConstFltPos);
            loadedEditorZoom = reinterpret_cast<float*>(data.data()+EEEditorZoomPos);
            loadedEditorClipRear = reinterpret_cast<float*>(data.data()+EEEditorClipRearPos);
            fileString = "File EE (Empire Earth) loaded:\n" + path.toStdString();
        } else if (file.size() == EEAOCLength) {
            std::cout << "File is Empire Earth Art of Conquest" << std::endl;
            state = EEstate::loadedEEAOC;
            loadedGameZoom = reinterpret_cast<float*>(data.data()+EEAOCGameZoomPos);
            loadedGameClipRear = reinterpret_cast<float*>(data.data()+EEAOCGameClipRearPos);
            loadedEditorConstFlt = reinterpret_cast<float*>(data.data()+EEAOCEditorConstFltPos);
            loadedEditorZoom = reinterpret_cast<float*>(data.data()+EEAOCEditorZoomPos);
            loadedEditorClipRear = reinterpret_cast<float*>(data.data()+EEAOCEditorClipRearPos);
            fileString = "File EE-AOC (Empire Earth - The Art of Conquest) loaded:\n" + path.toStdString();

        } else if (file.size() == NeoEELength) {
            std::cout << "File is Empire Earth (NeoEE)" << std::endl;
            state = EEstate::loadedNeoEE;
            loadedGameZoom = reinterpret_cast<float*>(data.data()+NeoEEGameZoomPos);
            loadedGameClipRear = reinterpret_cast<float*>(data.data()+NeoEEGameClipRearPos);
            loadedEditorConstFlt = reinterpret_cast<float*>(data.data()+NeoEEEditorConstFltPos);
            loadedEditorZoom = reinterpret_cast<float*>(data.data()+NeoEEEditorZoomPos);
            loadedEditorClipRear = reinterpret_cast<float*>(data.data()+NeoEEEditorClipRearPos);
            fileString = "File EE (NeoEE Empire Earth) loaded:\n" + path.toStdString();

        } else if (file.size() == NeoEEAOCLength) {
            std::cout << "File is Empire Earth Art of Conquest (NeoEE)" << std::endl;
            state = EEstate::loadedNeoEEAOC;
            loadedGameZoom = reinterpret_cast<float*>(data.data()+NeoEEAOCGameZoomPos);
            loadedGameClipRear = reinterpret_cast<float*>(data.data()+NeoEEAOCGameClipRearPos);
            loadedEditorConstFlt = reinterpret_cast<float*>(data.data()+NeoEEAOCEditorConstFltPos);
            loadedEditorZoom = reinterpret_cast<float*>(data.data()+NeoEEAOCEditorZoomPos);
            loadedEditorClipRear = reinterpret_cast<float*>(data.data()+NeoEEAOCEditorClipRearPos);
            fileString = "File EE-AOC (NeoEE Empire Earth - The Art of Conquest) loaded:\n" + path.toStdString();

        } else if (file.size() == EENoCDLength) {
            std::cout << "File is Empire Earth (No CD)" << std::endl;
            state = EEstate::loadedEENoCD;
            loadedGameZoom = reinterpret_cast<float*>(data.data()+EENoCDGameZoomPos);
            loadedGameClipRear = reinterpret_cast<float*>(data.data()+EENoCDGameClipRearPos);
            loadedEditorConstFlt = reinterpret_cast<float*>(data.data()+EENoCDEditorConstFltPos);
            loadedEditorZoom = reinterpret_cast<float*>(data.data()+EENoCDEditorZoomPos);
            loadedEditorClipRear = reinterpret_cast<float*>(data.data()+EENoCDEditorClipRearPos);
            fileString = "File EE (Empire Earth No CD) loaded:\n" + path.toStdString();

        } else {
            std::cout << "Unknown file with size " << std::to_string(file.size()) << std::endl;
            printErrorMessage("Unknown file! A file of the following size is expected: 6321152, 6319567, 12657664, 12062720 bytes");
            state = EEstate::unloaded;
            loadedGameZoom = nullptr;
            loadedGameClipRear = nullptr;
            loadedEditorConstFlt = nullptr;
            loadedEditorZoom = nullptr;
            loadedEditorClipRear = nullptr;
            fileString = "No file loaded";
        }
    } else{
        printErrorMessage("Error while reading the file");
        state = EEstate::unloaded;
        loadedGameZoom = nullptr;
        loadedGameClipRear = nullptr;
        loadedEditorConstFlt = nullptr;
        loadedEditorZoom = nullptr;
        loadedEditorClipRear = nullptr;
        fileString = "No file loaded";
    }
    inputFile.close();
    updateByState();
}

void MainWindow::gameZoomChg() {
    if (state == EEstate::unloaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Game Zoom",
               "Value:", *loadedGameZoom);

    *loadedGameZoom = val;
    updateByState();
}

void MainWindow::gameClipRearChg() {
    if (state == EEstate::unloaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Game Clip Rear",
               "Value:", *loadedGameClipRear);

    *loadedGameClipRear = val;
    updateByState();
}

void MainWindow::patchCbxChanged(int newState) {
    // ignore invalid state
    if (state == EEstate::unloaded)
        return;

    // Patch binary for scenario editor constant
    if (newState == Qt::Unchecked) {
        *loadedEditorConstFlt = defaultEditorConstFlt;
    } else {
        *loadedEditorConstFlt = defaultGameConstFlt;
    }
    *loadedEditorZoom = defaultEditorZoom;
    *loadedEditorClipRear = defaultEditorClipRear;

    updateByState();
}

void MainWindow::editorZoomChg() {
    if (state == EEstate::unloaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Editor Zoom",
               "Value:", *loadedEditorZoom);

    *loadedEditorZoom = val;
    updateByState();
}

void MainWindow::editorClipRearChg() {
    if (state == EEstate::unloaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Editor Clip Rear",
               "Value:", *loadedEditorClipRear);

    *loadedEditorClipRear = val;
    updateByState();
}

void MainWindow::setDefaultClicked(){
    if (state == EEstate::unloaded)
        return;

    *loadedGameZoom = defaultGameZoom;
    *loadedGameClipRear = defaultGameClipRear;
    *loadedEditorConstFlt = defaultEditorConstFlt;
    *loadedEditorZoom = defaultEditorZoom;
    *loadedEditorClipRear = defaultEditorClipRear;
    updateByState();
}

void MainWindow::saveClicked() {
    writeFile();
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
