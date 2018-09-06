#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSslSocket>
#include "json.h"
#include "connectorTop.h"
#include "httpConnector.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class clientMain : QObject
{
Q_OBJECT

public slots:

public:
	clientMain();
        ~clientMain();
	QString readConfigFile(const QString &);
	JsonObject clientinfos;
};


#endif //CLIENTMAIN_H
