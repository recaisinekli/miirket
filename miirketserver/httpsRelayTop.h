/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef HTTPSRELAYTOP_H
#define HTTPSRELAYTOP_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include "json.h"
#include "httpsRelayInner.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class httpsRelayTop : public QObject
{
Q_OBJECT

public slots:
	void disconnectedFromRegisteredHTTPSServer();
	void removeInnerRelayFromInnerMap(int);

public:
	QTcpSocket *httpsServer;
	QString subdomain;
	void incomingClient(QTcpSocket *, QByteArray);
	void isSubdomainTaken(bool);
	bool isRegistered;
	QMap<int, httpsRelayInner *> innerMap;
	int innerRelayID;
	void emitCheckMap();

signals:
	void checkMap(QString);
	void mapMe(QString);
	void removeMeFromMap(QString);
};

#endif //HTTPSRELAYTOP_H

