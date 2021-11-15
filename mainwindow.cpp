#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextEdit>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QDirIterator>
#include <QFont>
#include <QLocale>
#include <QSettings>
#include <QDesktopServices>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalWidget_2->setVisible(false);
    ui->textEditForSQL->setVisible(false);
    ui->horizontalWidgetBottom->setVisible(false);

    sqlSyntaxHighlighter = new SQLSyntaxHighlighter(ui->textEditForSQL->document());

    startProgramWithValueFromRegister();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sqlSyntaxHighlighter;
}

void MainWindow::startProgramWithValueFromRegister()
{
    QSettings set;
    ui->PathEdit->setText(set.value("general/path","").toString());
    ui->ExpressionEdit->setText(set.value("general/expression","").toString());
    ui->SQLcheckBox->setChecked(set.value("general/suffixes/sql",true).toBool());
    ui->VBScheckBox->setChecked(set.value("general/suffixes/vbs",false).toBool());
    ui->TXTcheckBox->setChecked(set.value("general/suffixes/txt",true).toBool());
    ui->CSVcheckBox->setChecked(set.value("general/suffixes/csv",false).toBool());
    ui->JSONcheckBox->setChecked(set.value("general/suffixes/json",false).toBool());

    ui->InTextCheckBox->setChecked(set.value("general/searchIn/text",true).toBool());
    ui->InNameCheckBox->setChecked(set.value("general/searchIn/name",true).toBool());

    switch (set.value("general/orderBy","1").toInt())
    {
    case 0: ui->OrderByPathRadioButton->setChecked(true);
        break;
    case 1: ui->OrderByNameRadioButton->setChecked(true);
        break;
    case 2: ui->OrderBySearchTypeRadioButton->setChecked(true);
        break;
    case 3: ui->OrderByCreatedRadioButton->setChecked(true);
        break;
    case 4: ui->OrderByUpdatedRadioButton->setChecked(true);
        break;
    case 5: ui->OrderBySuffixRadioButton->setChecked(true);
        break;
    default: ui->OrderByNameRadioButton->setChecked(true);
        break;
    }

    trans = new QTranslator();
    if (set.value("general/lang","en").toString() == "cs"){
        locale  = QLocale(QLocale::English);
        ui->languageButton->setText("EN");
        trans->load(":/lang/translation_en.qm");
    }else{
        locale  = QLocale(QLocale::Czech);
        ui->languageButton->setText("CZ");
        trans->load(":/lang/translation_cs.qm");
    }

    qApp->installTranslator(trans);
    ui->retranslateUi(this);
}

void MainWindow::saveValueToRegister()
{
    QSettings set;

    set.setValue("general/path",ui->PathEdit->text());
    set.setValue("general/expression",ui->ExpressionEdit->text());

    set.setValue("general/suffixes/sql",ui->SQLcheckBox->isChecked());
    set.setValue("general/suffixes/vbs",ui->VBScheckBox->isChecked());
    set.setValue("general/suffixes/txt",ui->TXTcheckBox->isChecked());
    set.setValue("general/suffixes/csv",ui->CSVcheckBox->isChecked());
    set.setValue("general/suffixes/json",ui->JSONcheckBox->isChecked());

    set.setValue("general/searchIn/text",ui->InTextCheckBox->isChecked());
    set.setValue("general/searchIn/name",ui->InNameCheckBox->isChecked());
}


void MainWindow::on_pickFolder_clicked()
{
    QSettings set;

    QString dir = QFileDialog::getExistingDirectory(this, tr("Vybrat"),
                                                set.value("general/path","").toString(),
                                                QFileDialog::ShowDirsOnly
                                                );

    ui->PathEdit->setText(dir);
}


void MainWindow::on_search_clicked()
{

    QString targetStr = ui->ExpressionEdit->text();

    if (targetStr.length() < 3) {
        QMessageBox::critical(this, "TSevcu app", tr("Délka hledaného výrazu musím mít alespoň 3 znaky!"));
        return;
    }

    QString directory = ui->PathEdit->text();

    QDir pathDir(directory);
    if (!pathDir.exists() || directory.length() < 2){
        QMessageBox::critical(this, "TSevcu app", tr("Zadaná cesta není validní!"));
        return;
    }
    QList<QString> checkedExtensions = selectedExtensions();
    if (checkedExtensions.count() == 0){
        QMessageBox::critical(this, "TSevcu app", tr("Vyberte alespoň 1 příponu souboru!"));
        return;
    }

    if (!ui->InNameCheckBox->isChecked() && !ui->InTextCheckBox->isChecked()){
        QMessageBox::critical(this, "TSevcu app", tr("Vyberte alespoň 1 možnost, kde vyhledávat!"));
        return;
    }

    restartListsAndTexts();

    QDirIterator it(directory, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filename = it.next();
        QFileInfo file(filename);

        if (file.isDir()) {
            continue;
        }

        //podle obsahu souboru
        if(ui->InTextCheckBox->isChecked()){
            if (checkedExtensions.contains(file.suffix())) {
                QFile qfile(file.absoluteFilePath());
                if (qfile.open(QIODevice::Text | QIODevice::ReadOnly)) {

                    QString string = (QString::fromUtf8(qfile.readAll()));
                    if (string.contains(targetStr, Qt::CaseInsensitive)){
                        int value = string.indexOf(targetStr);
                        QString finalString;
                        if (value < 25){
                            finalString = string.left(50);
                        }else{
                            finalString = string.mid(value-25,50);
                        }
                        foundFileList.push_back(new FoundFile(file, finalString, true));
                        continue;
                    }
                    qfile.close();
                }
            }
        }


        //podle nazvu souboru
        if(ui->InNameCheckBox->isChecked()){
            if (file.fileName().contains(targetStr, Qt::CaseInsensitive) && checkedExtensions.contains(file.suffix())) {
                QFile qfile(file.absoluteFilePath());
                if (qfile.open(QIODevice::Text | QIODevice::ReadOnly)) {
                    QString startString = (QString::fromUtf8(qfile.readAll())).left(50);
                    qfile.close();

                    foundFileList.push_back(new FoundFile(file, startString, false));
                }
            }
        }

    }

    if(ui->OrderByPathRadioButton->isChecked()){
        on_OrderByPathRadioButton_clicked();
    }else if(ui->OrderByNameRadioButton->isChecked()){
        on_OrderByNameRadioButton_clicked();
    }else if(ui->OrderBySearchTypeRadioButton->isChecked()){
        on_OrderBySearchTypeRadioButton_clicked();
    }else if(ui->OrderByCreatedRadioButton->isChecked()){
        on_OrderByCreatedRadioButton_clicked();
    }else if(ui->OrderByUpdatedRadioButton->isChecked()){
        on_OrderByUpdatedRadioButton_clicked();
    }else if(ui->OrderBySuffixRadioButton->isChecked()){
        on_OrderBySuffixRadioButton_clicked();
    }else {
        populateListWidget();
    }

    saveValueToRegister();
}

void MainWindow::populateListWidget()
{
    foreach (auto foundFile, foundFileList){
        QListWidgetItem* itemToAdd = new QListWidgetItem();
        itemToAdd->setText(foundFile->getFileInfo().fileName());
        itemToAdd->setToolTip(foundFile->getTextAroundString());
        if (foundFile->getInText()){
            itemToAdd->setForeground(Qt::blue);
        }else{
            itemToAdd->setForeground(Qt::red);
        }
        itemToAdd->setData(12,foundFile->getFileInfo().absoluteFilePath());
        ui->listWidget->addItem(itemToAdd);
    }
}


QList<QString> MainWindow::selectedExtensions()
{
    QList<QString> extensionsList;
    QList<QCheckBox *> checkBoxes = ui->groupBox->findChildren<QCheckBox *>();
    for (auto &checkbox : checkBoxes){
        if (checkbox->isChecked()){
            extensionsList.append(checkbox->text());
        }
    }
    return extensionsList;
}


void MainWindow::restartListsAndTexts()
{
    ui->PickFileLabel->setText(tr("Vyber soubor"));
    ui->textEdit->setText("");
    ui->textEditForSQL->setText("");
    ui->textEditForSQL->setVisible(false);
    ui->textEdit->setVisible(true);
    ui->DateCreatedLabel->setText("");
    ui->DateCreatedNameLabel->setText("");
    ui->DateUpdatedLabel->setText("");
    ui->DateUpdatedNameLabel->setText("");
    ui->PathFileLabel->setText("");
    ui->horizontalWidget_2->setVisible(false);
    for (auto foundFile : foundFileList){
        delete foundFile;
    }
    foundFileList.clear();
    ui->horizontalWidgetBottom->setVisible(false);
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString fileName = item->data(12).toString();

    setSelectedFile(fileName);

    ui->PickFileLabel->setText(item->text());
    QFileInfo fileInfo(fileName);

    ui->DateCreatedLabel->setText(locale.toString(fileInfo.birthTime(), tr("dd.MM.yyyy")));
    ui->DateCreatedNameLabel->setText(tr("Vytvořeno:"));
    ui->DateUpdatedLabel->setText(locale.toString(fileInfo.lastModified(), tr("dd.MM.yyyy")));
    ui->DateUpdatedNameLabel->setText(tr("Upraveno:"));

    ui->PathFileLabel->setText(fileInfo.absolutePath().right(fileInfo.absolutePath().length()-ui->PathEdit->text().length()));

    ui->horizontalWidget_2->setVisible(true);
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::Text | QIODevice::ReadOnly)) {

            ui->horizontalWidgetBottom->setVisible(true);
            ui->seachInTextLineEdit->setFocus();

            QString text = file.readAll();
            if(fileInfo.suffix() == "sql"){
                ui->textEditForSQL->setText(text);
                ui->textEditForSQL->setVisible(true);
                ui->textEdit->setVisible(false);
            }else{
                ui->textEditForSQL->setVisible(false);
                ui->textEdit->setVisible(true);
                ui->textEdit->setText(text);
            }
            file.close();
        }else{
            //FoundFileList.removeAll(item); //nevim jestli funguje tak jak ma, mozna mi tam chybi operator porovnani (ale kdyz funguje contains, tka by mohlo fungovat i tohle)
            QMessageBox::critical(this, "TSevcu App", tr("Soubor se nepodařilo otevřít!"));
        }
    }
}


void MainWindow::on_OrderByPathRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","0");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().absoluteFilePath().toLower() == f2->getFileInfo().absoluteFilePath().toLower()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().absoluteFilePath().toLower() < f2->getFileInfo().absoluteFilePath().toLower();
    });

    populateListWidget();
}


void MainWindow::on_OrderByNameRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","1");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
    });

    populateListWidget();
}


void MainWindow::on_OrderBySearchTypeRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","2");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getInText() == f2->getInText()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getInText() < f2->getInText();
    });

    populateListWidget();
}


void MainWindow::on_OrderByCreatedRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","3");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().birthTime() == f2->getFileInfo().birthTime()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().birthTime() < f2->getFileInfo().birthTime();
    });

    populateListWidget();
}


void MainWindow::on_OrderByUpdatedRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","4");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().lastModified() == f2->getFileInfo().lastModified()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().lastModified() < f2->getFileInfo().lastModified();
    });

    populateListWidget();
}


void MainWindow::on_OrderBySuffixRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","5");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().suffix() == f2->getFileInfo().suffix()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().suffix() < f2->getFileInfo().suffix();
    });

    populateListWidget();
}


void MainWindow::on_pushButton_clicked()
{
    QString path = QDir::fromNativeSeparators(getSelectedFile());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

const QString &MainWindow::getSelectedFile() const
{
    return selectedFile;
}

void MainWindow::setSelectedFile(const QString &newSelectedFile)
{
    selectedFile = newSelectedFile;
}


void MainWindow::on_seachInTextLineEdit_textEdited(const QString &arg1)
{
    selectSearchText(0);
}


void MainWindow::on_nextButton_clicked()
{
    if (!selectSearchText(lastIndexFound)){
        selectSearchText(0);
    }
}

bool MainWindow::selectSearchText(int position)
{
    QFileInfo fileInfo(getSelectedFile());
    QTextEdit* textEdit;
    if(fileInfo.suffix() == "sql"){
        textEdit = ui->textEditForSQL;
    }else{
        textEdit = ui->textEdit;
    }
    QString text = textEdit->toPlainText();
    QString searchedText = ui->seachInTextLineEdit->text();

    int startPos = text.indexOf(searchedText,position,Qt::CaseInsensitive);
    int endPos = startPos + searchedText.length();

    if (startPos != -1){
        lastIndexFound = endPos;

        QTextCursor c = textEdit->textCursor();
        c.setPosition(startPos);
        c.setPosition(endPos, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(c);
    }

    return startPos != -1;
}


void MainWindow::on_PathEdit_returnPressed()
{
    on_pickFolder_clicked();
}


void MainWindow::on_ExpressionEdit_returnPressed()
{
    on_search_clicked();
}


void MainWindow::on_seachInTextLineEdit_returnPressed()
{
    on_nextButton_clicked();
}


void MainWindow::on_languageButton_clicked()
{
    QSettings set;
    if(ui->languageButton->text() == "CZ"){
        set.setValue("general/lang","en");
        locale  = QLocale(QLocale::English);
        trans->load(":/lang/translation_en.qm");
    }else{
        set.setValue("general/lang","cz");
        locale  = QLocale(QLocale::Czech);
        trans->load(":/lang/translation_cs.qm");
    }
    qApp->installTranslator(trans);
    ui->retranslateUi(this);
}

