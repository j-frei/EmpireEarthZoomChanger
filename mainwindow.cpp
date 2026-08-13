#include "mainwindow.h"
#include <QtWidgets>
#include <QMainWindow>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

const std::vector<unsigned char> MainWindow::CameraPrefixPattern = {
    0xcd, 0xcc, 0xcc, 0x3d,
    0xcd, 0xcc, 0xcc, 0x3e,
    0x00, 0x00, 0x20, 0x41,
    0x35, 0xfa, 0x8e, 0x3c,
    0xdb, 0x0f, 0x49, 0x3f
};
const std::vector<unsigned char> MainWindow::AudioPrefixPattern = {
    0x8f, 0xc2, 0xb5, 0x3f,
    0xcd, 0xcc, 0xcc, 0xbe,
    0x0a, 0xd7, 0xa3, 0x3b,
    0x0a, 0xd7, 0xa3, 0xbb,
    0x00, 0x00, 0x80, 0x3f
};

MainWindow::MainWindow()

{
    MainWindow::loaded = false;

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

    // Audio
    QLabel *audioInfo = new QLabel;
    audioInfo->setText("<b>Audio Distance</b>");
    layout->addWidget(audioInfo,5,0,1,3);

    // Audio Link Checkbox
    QLabel *audioLinkCbxInfo = new QLabel;
    audioLinkCbxInfo->setText("Derive from Rear Clipping");
    layout->addWidget(audioLinkCbxInfo,6,0,1,1);

    audioLinkCbx = new QCheckBox;
    audioLinkCbx->setChecked(true);
    layout->addWidget(audioLinkCbx,6,1,1,1);

    // Audio Cutoff Input
    QLabel *audioCutoffEditInfo = new QLabel;
    audioCutoffEditInfo->setText("Audio cutoff distance (units beyond are mute), (default: 35)");
    layout->addWidget(audioCutoffEditInfo,7,0,1,1);

    audioCutoffStatus = new QLabel;
    layout->addWidget(audioCutoffStatus,7,1,1,1);

    audioCutoffChgBtn = new QPushButton;
    audioCutoffChgBtn->setText(u8"🔧");
    layout->addWidget(audioCutoffChgBtn,7,2,1,1);

    // Editor
    QLabel *editorInfo = new QLabel;
    editorInfo->setText("<b>Scenario Editor Settings</b>");
    layout->addWidget(editorInfo,8,0,1,3);

    // Editor Checkbox Patch
    QLabel *editorConstFltPatchCbxInfo = new QLabel;
    editorConstFltPatchCbxInfo->setText("Enable Patch for Editor modifications");
    layout->addWidget(editorConstFltPatchCbxInfo,9,0,1,1);

    editorConstFltPatchCbx = new QCheckBox;
    layout->addWidget(editorConstFltPatchCbx,9,1,1,1);

    // Editor Zoom Input
    QLabel *editorZoomEditInfo = new QLabel;
    editorZoomEditInfo->setText("Zoom (lower=further away), (default: -42.0)");
    layout->addWidget(editorZoomEditInfo,10,0,1,1);

    editorZoomStatus = new QLabel;
    layout->addWidget(editorZoomStatus,10,1,1,1);

    editorZoomChgBtn = new QPushButton;
    editorZoomChgBtn->setText(u8"🔧");
    layout->addWidget(editorZoomChgBtn,10,2,1,1);

    // Editor ClipRear/Fog Input
    QLabel *editorClipRearEditInfo = new QLabel;
    editorClipRearEditInfo->setText("Rear Clipping (lower=more fog, faster), (default: 58)");
    layout->addWidget(editorClipRearEditInfo,11,0,1,1);

    editorClipRearStatus = new QLabel;
    layout->addWidget(editorClipRearStatus,11,1,1,1);

    editorClipRearChgBtn = new QPushButton;
    editorClipRearChgBtn->setText(u8"🔧");
    layout->addWidget(editorClipRearChgBtn,11,2,1,1);

    // Restore default values
    setDefaultBtn = new QPushButton;
    setDefaultBtn->setText("Set default values");
    layout->addWidget(setDefaultBtn,12,0,1,3);

    // Save Button
    saveBtn = new QPushButton;
    saveBtn->setText("Write values");
    layout->addWidget(saveBtn,13,0,1,3);

    // Set QWidget as the central layout of the main window
    setCentralWidget(window);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
    setUnifiedTitleAndToolBarOnMac(true);

    // SIGNAL/SLOT handling
    connect( loadBtn, SIGNAL(clicked()),this,SLOT(openFileClicked()));

    connect( gameZoomChgBtn, SIGNAL(clicked()),this,SLOT(gameZoomChg()));
    connect( gameClipRearChgBtn, SIGNAL(clicked()),this,SLOT(gameClipRearChg()));

    connect( audioLinkCbx, SIGNAL(stateChanged(int)),this,SLOT(audioLinkCbxChanged(int)));
    connect( audioCutoffChgBtn, SIGNAL(clicked()),this,SLOT(audioCutoffChg()));

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

    if (!loaded) {
        gameZoomChgBtn->setEnabled(false);
        gameClipRearChgBtn->setEnabled(false);
        audioLinkCbx->setEnabled(false);
        audioCutoffChgBtn->setEnabled(false);
        editorConstFltPatchCbx->setEnabled(false);
        editorZoomChgBtn->setEnabled(false);
        editorClipRearChgBtn->setEnabled(false);
        saveBtn->setEnabled(false);
        setDefaultBtn->setEnabled(false);

        fileLabel->setText(QString::fromStdString(fileString));

        gameZoomStatus->setText(QString::fromStdString(std::to_string(defaultGameZoom)));
        gameClipRearStatus->setText(QString::fromStdString(std::to_string(defaultGameClipRear)));

        audioCutoffStatus->setText(QString::number(defaultAudioFadeEnd,'f',2)
                                   + " (fade from " + QString::number(defaultAudioFadeStart,'f',2) + ")");

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

        // Not sure if the audio prefix is present everywhere -> separate state/check
        audioLinkCbx->setEnabled(hasAudioPatch());
        audioCutoffChgBtn->setEnabled(hasAudioPatch());
        if (hasAudioPatch()) {
            audioCutoffStatus->setText(
                QString::number(*loadedAudioFadeEnd,'f',2)
                + " (fade from "
                + QString::number(*loadedAudioFadeStart,'f',2)
                + ")"
            );
        } else {
            audioCutoffStatus->setText("audio pattern not found in this file");
        }

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
        inputFile.close();

        MainWindow::path = filepath;
        MainWindow::data = file;

        if (locateCameraFields()) {
            // Found camera bits -> can load
            loaded = true;
            // Check if audio bits are present too
            locateAudioFields();

            fileString = "File loaded ("
                + std::to_string(data.size())
                + " bytes"
                + (hasAudioPatch() ? ", audio patch available" : ", audio pattern not found")
                + "):\n"
                + path.toStdString();
        } else {
            // Error msg
            std::cout << "Camera constants pattern not found in file of size " << std::to_string(data.size()) << std::endl;
            printErrorMessage(
                "Unrecognized file.",
                "Could not locate the camera constants in this file. It may not be a supported "
                + "Empire Earth / Empire Earth: The Art of Conquest executable, or it has already "
                + "been patched by an incompatible tool."
            );

            loaded = false;
            loadedGameZoom = nullptr;
            loadedGameClipRear = nullptr;
            loadedEditorConstFlt = nullptr;
            loadedEditorZoom = nullptr;
            loadedEditorClipRear = nullptr;
            loadedAudioFadeStart = nullptr;
            loadedAudioFadeEnd = nullptr;
            loadedAudioFadeSlope = nullptr;
            fileString = "No file loaded";
        }
    } else{
        inputFile.close();
        printErrorMessage("Error while reading the file");

        loaded = false;
        loadedGameZoom = nullptr;
        loadedGameClipRear = nullptr;
        loadedEditorConstFlt = nullptr;
        loadedEditorZoom = nullptr;
        loadedEditorClipRear = nullptr;
        loadedAudioFadeStart = nullptr;
        loadedAudioFadeEnd = nullptr;
        loadedAudioFadeSlope = nullptr;
        fileString = "No file loaded";
    }
    updateByState();
}

long MainWindow::findPattern(const std::vector<char> &binary_data, const std::vector<unsigned char> &pattern) {
    if (pattern.empty() || binary_data.size() < pattern.size()) return -1;

    // Search using iterator
    auto it = std::search(binary_data.begin(), binary_data.end(), pattern.begin(), pattern.end(),
        [](char a, unsigned char b) {
            return static_cast<unsigned char>(a) == b;
        }
    );

    if (it == binary_data.end()) {
        //No match found :(
        return -1;
    } else {
        // Found it!
        return static_cast<long>(std::distance(binary_data.begin(), it));
    }
}

bool MainWindow::locateCameraFields() {
    long match = findPattern(data, CameraPrefixPattern);

    if (match < 0) {
        // Stop if no match
        return false;
    }

    // Get offset of prefix end
    long fieldsBase = match + static_cast<long>(CameraPrefixPattern.size());

    if (static_cast<size_t>(fieldsBase) + EditorClipRearFieldOffset + sizeof(float) > data.size()) {
        // Last field is outside of file -> weird -> stop!
        return false;
    }

    // Check if the constant is as expected (somewhat unnecessary though :O -> should not happen!)
    float gameConstCheck;
    std::memcpy(&gameConstCheck, data.data() + fieldsBase + GameConstFieldOffset, sizeof(float));
    if (std::fabs(gameConstCheck - defaultGameConstFlt) > 0.0001f) {
        std::cout << "Unexpected game constant value: " << gameConstCheck << std::endl;
        return false;
    }

    // Set pointers by offset
    loadedEditorConstFlt = reinterpret_cast<float*>(data.data() + fieldsBase + EditorConstFieldOffset);
    loadedGameZoom = reinterpret_cast<float*>(data.data() + fieldsBase + GameZoomFieldOffset);
    loadedGameClipRear = reinterpret_cast<float*>(data.data() + fieldsBase + GameClipRearFieldOffset);
    loadedEditorZoom = reinterpret_cast<float*>(data.data() + fieldsBase + EditorZoomFieldOffset);
    loadedEditorClipRear = reinterpret_cast<float*>(data.data() + fieldsBase + EditorClipRearFieldOffset);
    return true;
}

void MainWindow::locateAudioFields() {
    // Similar to locateCameraFields
    long match = findPattern(data, AudioPrefixPattern);

    long fieldsBase = match < 0 ? -1 : match + static_cast<long>(AudioPrefixPattern.size());
    if (fieldsBase < 0 || static_cast<size_t>(fieldsBase) + AudioFadeSlopeFieldOffset + sizeof(float) > data.size()) {
        // Found no prefix -> stop here
        loadedAudioFadeStart = nullptr;
        loadedAudioFadeEnd = nullptr;
        loadedAudioFadeSlope = nullptr;
        return;
    }

    // Set pointers by offset
    loadedAudioFadeStart = reinterpret_cast<float*>(data.data() + fieldsBase + AudioFadeStartFieldOffset);
    loadedAudioFadeEnd = reinterpret_cast<float*>(data.data() + fieldsBase + AudioFadeEndFieldOffset);
    loadedAudioFadeSlope = reinterpret_cast<float*>(data.data() + fieldsBase + AudioFadeSlopeFieldOffset);
}

void MainWindow::gameZoomChg() {
    if (!loaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Game Zoom",
               "Value:", *loadedGameZoom);

    *loadedGameZoom = val;
    updateByState();
}

void MainWindow::gameClipRearChg() {
    if (!loaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Game Clip Rear",
               "Value:", *loadedGameClipRear);

    *loadedGameClipRear = val;
    if (audioLinkCbx->isChecked())
        applyAudioCutoff(*loadedGameClipRear);
    updateByState();
}

bool MainWindow::hasAudioPatch() const {
    return loadedAudioFadeStart != nullptr && loadedAudioFadeEnd != nullptr && loadedAudioFadeSlope != nullptr;
}

void MainWindow::applyAudioCutoff(float fadeEnd) {
    if (!hasAudioPatch())
        return;

    float fadeStart = fadeEnd * audioFadeStartRatio;
    if (!(fadeEnd > 0.0) || !(fadeEnd - fadeStart > 0.001)) {
        // Invalid valeu!
        printErrorMessage("Invalid fadeEnd distance.", "The value must be greater than 0. Common values are 35 (stock) up to your Rear Clipping value.");
        return;
    }

    *loadedAudioFadeStart = fadeStart;
    *loadedAudioFadeEnd = fadeEnd;
    // slope is precomputed
    *loadedAudioFadeSlope = 1.0 / (fadeEnd - fadeStart);
}

void MainWindow::audioLinkCbxChanged(int newState) {
    if (!loaded)
        return;

    if (newState == Qt::Checked) {
        applyAudioCutoff(*loadedGameClipRear);
    }

    updateByState();
}

void MainWindow::audioCutoffChg() {
    if (!loaded || !hasAudioPatch())
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Audio Fade End",
        "Value:", *loadedAudioFadeEnd);

    audioLinkCbx->blockSignals(true);
    audioLinkCbx->setCheckState(Qt::Unchecked);
    audioLinkCbx->blockSignals(false);

    applyAudioCutoff(val);
    updateByState();
}

void MainWindow::patchCbxChanged(int newState) {
    // ignore invalid state
    if (!loaded)
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
    if (!loaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Editor Zoom",
               "Value:", *loadedEditorZoom);

    *loadedEditorZoom = val;
    updateByState();
}

void MainWindow::editorClipRearChg() {
    if (!loaded)
        return;

    double val = QInputDialog::getDouble(nullptr, "Set value for Editor Clip Rear",
               "Value:", *loadedEditorClipRear);

    *loadedEditorClipRear = val;
    updateByState();
}

void MainWindow::setDefaultClicked(){
    if (!loaded)
        return;

    *loadedGameZoom = defaultGameZoom;
    *loadedGameClipRear = defaultGameClipRear;
    *loadedEditorConstFlt = defaultEditorConstFlt;
    *loadedEditorZoom = defaultEditorZoom;
    *loadedEditorClipRear = defaultEditorClipRear;
    if (hasAudioPatch()) {
        *loadedAudioFadeStart = defaultAudioFadeStart;
        *loadedAudioFadeEnd = defaultAudioFadeEnd;
        *loadedAudioFadeSlope = defaultAudioFadeSlope;
    }
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
