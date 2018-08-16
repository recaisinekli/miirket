/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef CONNECTORTOP_H
#define CONNECTORTOP_H
#include <QDebug>
#include <QTimer>
#include "json.h"
#include "connectorInner.h"
#include <QSslSocket>

using QtJson::JsonObject;
using QtJson::JsonArray;

class connectorTop : public QObject
{
Q_OBJECT

public slots:
	void startTransfer();
	void startReadFromRemote();
	void tryToConnectToServer();
	void disconnectedFromServer();
	void deleteConnectorInner();
	void sslErrors(const QList<QSslError> &errors);
	void socketEncrypted();
	void heartbeat();

public:
	QTimer *timer1;
	QTimer *heartbeatTimer;
	QSslSocket *client;
	JsonObject clientinfo;
	QList<connectorInner *> connectorInnerList;
};

#endif //CONNECTORTOP_H
