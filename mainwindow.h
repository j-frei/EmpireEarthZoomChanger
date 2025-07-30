#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

enum EEstate { unloaded, loadedEE, loadedEEAOC, loadedEEGOG25, loadedEEAOCGOG25, loadedNeoEE, loadedNeoEEAOC, loadedEENoCD };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openFileClicked();

    void gameZoomChg();
    void gameClipRearChg();

    void patchCbxChanged(int newState);
    void editorZoomChg();
    void editorClipRearChg();

    void setDefaultClicked();
    void saveClicked();

private:
    void printInfoMessage(QString msg, QString detailMsg = nullptr);
    void printErrorMessage(QString msg, QString detailMsg = nullptr);
    void updateByState();
    void writeFile();

    QLabel *fileLabel;

    QLabel *gameZoomStatus;
    QPushButton *gameZoomChgBtn;
    QLabel *gameClipRearStatus;
    QPushButton *gameClipRearChgBtn;

    QCheckBox *editorConstFltPatchCbx;
    QLabel *editorZoomStatus;
    QPushButton *editorZoomChgBtn;
    QLabel *editorClipRearStatus;
    QPushButton *editorClipRearChgBtn;

    QPushButton *setDefaultBtn;
    QPushButton *saveBtn;

    EEstate state;
    float* loadedGameZoom = nullptr;
    float* loadedGameClipRear = nullptr;
    float* loadedEditorConstFlt = nullptr;
    float* loadedEditorZoom = nullptr;
    float* loadedEditorClipRear = nullptr;
    std::string fileString;
    QString path;
    std::vector<char> data;

    // values for Empire Earth.exe
    uint EELength = 6321152; // #bytes
    uint EEGameConstFltPos = 0x42b4e8; // float pos, Content should not be changed!
    uint EEEditorConstFltPos = 0x42b4e4; // float pos
    uint EEGameZoomPos = 0x42b500; // float pos
    uint EEGameClipRearPos = 0x42b504; // float pos
    uint EEEditorZoomPos = 0x42b508; // float pos
    uint EEEditorClipRearPos = 0x42b50c; // float pos

    // values for Empire Earth.exe (GOG 2025); identical to default Empire Earth.exe
    uint EEGOG25Length = 6321664; // #bytes
    uint EEGOG25GameConstFltPos = 0x42b4e8; // float pos, Content should not be changed!
    uint EEGOG25EditorConstFltPos = 0x42b4e4; // float pos
    uint EEGOG25GameZoomPos = 0x42b500; // float pos
    uint EEGOG25GameClipRearPos = 0x42b504; // float pos
    uint EEGOG25EditorZoomPos = 0x42b508; // float pos
    uint EEGOG25EditorClipRearPos = 0x42b50c; // float pos

    // values for EE-AOC.exe
    uint EEAOCLength = 6319567; // #bytes
    uint EEAOCGameConstFltPos = 0x441e88; // float pos, Content should not be changed!
    uint EEAOCEditorConstFltPos = 0x441e84; // float pos
    uint EEAOCGameZoomPos = 0x441ea0; // float pos
    uint EEAOCGameClipRearPos = 0x441ea4; // float pos
    uint EEAOCEditorZoomPos = 0x441ea8; // float pos
    uint EEAOCEditorClipRearPos = 0x441eac; // float pos

    // values for EE-AOC.exe (GOG 2025); identical to default EE-AOC.exe
    uint EEAOCGOG25Length = 6262784; // #bytes
    uint EEAOCGOG25GameConstFltPos = 0x441e88; // float pos, Content should not be changed!
    uint EEAOCGOG25EditorConstFltPos = 0x441e84; // float pos
    uint EEAOCGOG25GameZoomPos = 0x441ea0; // float pos
    uint EEAOCGOG25GameClipRearPos = 0x441ea4; // float pos
    uint EEAOCGOG25EditorZoomPos = 0x441ea8; // float pos
    uint EEAOCGOG25EditorClipRearPos = 0x441eac; // float pos

    // values for Empire Earth.exe (NeoEE)
    uint NeoEELength = 12657664; // #bytes
    uint NeoEEGameConstFltPos = 0x42bee8; // float pos, Content should not be changed!
    uint NeoEEEditorConstFltPos = 0x42bee4; // float pos
    uint NeoEEGameZoomPos = 0x42bf00; // float pos
    uint NeoEEGameClipRearPos = 0x42bf04; // float pos
    uint NeoEEEditorZoomPos = 0x42bf08; // float pos
    uint NeoEEEditorClipRearPos = 0x42bf0c; // float pos

    // values for EE-AOC.exe  (NeoEE)
    uint NeoEEAOCLength = 12062720; // #bytes
    uint NeoEEAOCGameConstFltPos = 0x441c88; // float pos, Content should not be changed!
    uint NeoEEAOCEditorConstFltPos = 0x441c84; // float pos
    uint NeoEEAOCGameZoomPos = 0x441ca0; // float pos
    uint NeoEEAOCGameClipRearPos = 0x441ca4; // float pos
    uint NeoEEAOCEditorZoomPos = 0x441ca8; // float pos
    uint NeoEEAOCEditorClipRearPos = 0x441cac; // float pos

    // values for Empire Earth.exe (No CD)
    uint EENoCDLength = 4079617; // #bytes
    uint EENoCDGameConstFltPos = 0x2ad4b0; // float pos, Content should not be changed!
    uint EENoCDEditorConstFltPos = 0x2ad4ac; // float pos
    uint EENoCDGameZoomPos = 0x2ad4c8; // float pos
    uint EENoCDGameClipRearPos = 0x2ad4cc; // float pos
    uint EENoCDEditorZoomPos = 0x2ad4d0; // float pos
    uint EENoCDEditorClipRearPos = 0x2ad4d4; // float pos

    // default values
    float defaultGameZoom = -20.5;
    float defaultGameClipRear = 35.0;
    float defaultEditorZoom = -42.0;
    float defaultEditorClipRear = 58.0;

    float defaultGameConstFlt = 0.0; // Content should not be changed!
    float defaultEditorConstFlt = 0.1;
};

#endif // MAINWINDOW_H
