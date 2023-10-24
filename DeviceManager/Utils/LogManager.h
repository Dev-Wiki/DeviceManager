//
// Created by zyz on 2023/10/17.
//

#ifndef DEVICEMANAGER_LOGMANAGER_H
#define DEVICEMANAGER_LOGMANAGER_H


#include <QtMsgHandler>
#include <QString>

class LogManager {
public:
    static void CustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};


#endif //DEVICEMANAGER_LOGMANAGER_H
