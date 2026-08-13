#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <vector>

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

    void audioLinkCbxChanged(int newState);
    void audioCutoffChg();

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

    static long findPattern(const std::vector<char> &binary_data, const std::vector<unsigned char> &pattern);
    bool locateCameraFields();
    void locateAudioFields();

    bool hasAudioPatch() const;
    void applyAudioCutoff(float silenceDistance);

    QLabel *fileLabel;

    QLabel *gameZoomStatus;
    QPushButton *gameZoomChgBtn;
    QLabel *gameClipRearStatus;
    QPushButton *gameClipRearChgBtn;

    QCheckBox *audioLinkCbx;
    QLabel *audioCutoffStatus;
    QPushButton *audioCutoffChgBtn;

    QCheckBox *editorConstFltPatchCbx;
    QLabel *editorZoomStatus;
    QPushButton *editorZoomChgBtn;
    QLabel *editorClipRearStatus;
    QPushButton *editorClipRearChgBtn;

    QPushButton *setDefaultBtn;
    QPushButton *saveBtn;

    bool loaded = false;
    float* loadedGameZoom = nullptr;
    float* loadedGameClipRear = nullptr;
    float* loadedEditorConstFlt = nullptr;
    float* loadedEditorZoom = nullptr;
    float* loadedEditorClipRear = nullptr;

    // nullptr -> audio prefix pattern not found
    float* loadedAudioFadeStart = nullptr;
    float* loadedAudioFadeEnd = nullptr;
    float* loadedAudioFadeSlope = nullptr;

    std::string fileString;
    QString path;
    std::vector<char> data;

    // Prefix patterns
    static const std::vector<unsigned char> CameraPrefixPattern;
    static const std::vector<unsigned char> AudioPrefixPattern;

    // Field offsets at end of a prefix
    // -> Camera pattern
    static constexpr long EditorConstFieldOffset = 0x00;
    static constexpr long GameConstFieldOffset = 0x04; // Content should not be changed!
    static constexpr long GameZoomFieldOffset = 0x1c;
    static constexpr long GameClipRearFieldOffset = 0x20;
    static constexpr long EditorZoomFieldOffset = 0x24;
    static constexpr long EditorClipRearFieldOffset = 0x28;
    // -> Audio pattern
    static constexpr long AudioFadeStartFieldOffset = 0x00;
    static constexpr long AudioFadeEndFieldOffset = 0x04;
    static constexpr long AudioFadeSlopeFieldOffset = 0x08;

    // default values
    float defaultGameZoom = -20.5;
    float defaultGameClipRear = 35.0;
    float defaultEditorZoom = -42.0;
    float defaultEditorClipRear = 58.0;

    float defaultGameConstFlt = 0.0; // Content should not be changed!
    float defaultEditorConstFlt = 0.1;

    float defaultAudioFadeStart = 27.0;
    float defaultAudioFadeEnd = 35.0;
    float defaultAudioFadeSlope = 0.125; // slope = 1/(fade_end - fade_start)
    // Keep the ratio of start -> end fade constant with changed ClipRear
    float audioFadeStartRatio = defaultAudioFadeStart / defaultAudioFadeEnd;
};

#endif // MAINWINDOW_H
