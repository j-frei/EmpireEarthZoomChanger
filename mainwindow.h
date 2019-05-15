#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

enum EEstate { unloaded, loadedEE, loadedEEAOC };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openFileClicked();
    void saveClicked();
    void saveDefaultClicked();

private:
    void printInfoMessage(QString msg, QString detailMsg = nullptr);
    void printErrorMessage(QString msg, QString detailMsg = nullptr);
    void updateByState();
    void overwriteDataArray(float zoom, float cull);
    void writeFile();
    QLabel *fileLabel;
    QLineEdit *zoomEdit;
    QLineEdit *cullEdit;
    QPushButton *saveBtn;
    QPushButton *saveDefaultBtn;

    EEstate state;
    QString path;
    std::vector<char> data;

    // values for Empire Earth.exe
    uint EELength = 6321152; // #bytes
    uint EEzoomPos = 0x42b500; // float pos
    uint EEcullPos = 0x42b504; // float pos
    // values for EE-AOC.exe
    uint EEAOCLength = 6319567; // #bytes
    uint EEAOCzoomPos = 0x441ea0; // float pos
    uint EEAOCcullPos = 0x441ea4; // float pos

    // default values
    float defaultZoom = -20.5;
    float defaultCull = 35.0;
};

#endif // MAINWINDOW_H
