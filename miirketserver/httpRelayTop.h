#ifndef HTTPRELAYTOP_H
#define HTTPRELAYTOP_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include "json.h"
#include "httpRelayInner.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class httpRelayTop : public QObject
{
Q_OBJECT

public slots:
	void disconnectedFromRegisteredHTTPServer();
	void removeInnerRelayFromInnerMap(int);

public:
	QTcpSocket *httpServer;
	QString subdomain;
	void incomingClient(QTcpSocket *, QByteArray);
	void isSubdomainTaken(bool);
	bool isRegistered;
	QMap<int, httpRelayInner *> innerMap;
	int innerRelayID;
	void emitCheckMap();

signals:
	void checkMap(QString);
	void mapMe(QString);
	void removeMeFromMap(QString);
};

#endif //HTTPRELAYTOP_H

