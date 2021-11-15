#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "foundfile.h"
#include "sqlsyntaxhighlighter.h"
#include <QListWidget>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QLocale locale;

    const QString &getSelectedFile() const;
    void setSelectedFile(const QString &newSelectedFile);

private slots:
    void on_pickFolder_clicked();

    void on_search_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void populateListWidget();

    void on_OrderByPathRadioButton_clicked();

    void on_OrderByNameRadioButton_clicked();

    void on_OrderBySearchTypeRadioButton_clicked();

    void on_OrderByCreatedRadioButton_clicked();

    void on_OrderByUpdatedRadioButton_clicked();

    void on_OrderBySuffixRadioButton_clicked();

    void on_pushButton_clicked();

    void on_seachInTextLineEdit_textEdited(const QString &arg1);

    void on_nextButton_clicked();

    bool selectSearchText(int position = 0);

    void on_PathEdit_returnPressed();

    void on_ExpressionEdit_returnPressed();

    void on_seachInTextLineEdit_returnPressed();

    void on_languageButton_clicked();

private:
    Ui::MainWindow *ui;
    QList<FoundFile*> foundFileList;
    SQLSyntaxHighlighter *sqlSyntaxHighlighter;
    QString selectedFile;
    int lastIndexFound;
    QTranslator *trans;

protected:
    QList<QString> selectedExtensions();
    void restartListsAndTexts();
    void startProgramWithValueFromRegister();
    void saveValueToRegister();
};
#endif // MAINWINDOW_H
