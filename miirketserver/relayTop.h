/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef RELAYTOP_H
#define RELAYTOP_H
#include <QDebug>
#include <QTimer>
#include <QtSql>
#include <QSslSocket>
#include "json.h"
#include "relayInner.h"
#include "sslserver.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class relayTop : public QObject
{
Q_OBJECT

public slots:
    void acceptConnectionFromSshClient();
	void disconnectedFromRegisteredSshServer();

public:
    void incomingChannel(QTcpSocket *);
    QTcpServer *sshRelay;
	QTcpSocket *sshServer;
    int port;
    relayInner *waitingInnerRelay;
	QString userEmail;
	int accountType;

signals:
	void requestNewChannel();
	void disconnectedFromServer(QString, int, int);
};

#endif //RELAY_H
