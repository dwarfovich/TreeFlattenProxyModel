#include "main_window.hpp"

#include <QApplication>

#include <iostream>

void logHandler (QtMsgType type, const QMessageLogContext& context, const QString& message) {
    Q_UNUSED(type)
    Q_UNUSED(context)

    std::cerr << message.toStdString() << std::endl;
}

int main (int argc, char* argv[]) {
    qInstallMessageHandler(logHandler);

    QApplication a(argc, argv);
    MainWindow   w;
    w.show();
    return a.exec();
}
