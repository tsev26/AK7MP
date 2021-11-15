#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("UTB");
    QCoreApplication::setOrganizationDomain("utb.cz");
    QCoreApplication::setApplicationName("TSevcuApp");

    /*
    //pro pridani stylu - vyuziju pro formatovani ruznych souboru - sql, vbs..
    QFile st(":/style/style.qss");
    st.open(QIODevice::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(st.readAll());
    st.close();
    */


    MainWindow w;
    w.show();
    return a.exec();
}
