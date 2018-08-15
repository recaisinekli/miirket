/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef CONNECTORINNER_H
#define CONNECTORINNER_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include "json.h"
#include <QSslSocket>

using QtJson::JsonObject;
using QtJson::JsonArray;

class connectorInner : public QObject
{
Q_OBJECT

public slots:
	void startTransfer();
	void startReadFromRemote();
	void startReadFromLocal();
	void disconnectedFromRemote();

public:
	connectorInner();
	~connectorInner();
	void tryToConnect();
	bool localConnected;
	int id;
	QSslSocket *client;
	QTcpSocket *localClient;
	QTcpSocket *sshClient;
	JsonObject clientinfo;
signals:
	void deleteMe();
};

#endif //CONNECTORINNER_H
