/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef RELAYINNER_H
#define RELAYINNER_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include <QtSql>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class relayInner : public QObject
{
Q_OBJECT

public slots:
	void startReadFromSshServer();
	void startReadFromSshClient();
	void disconnectedFromSshClient();

public:
    	QTcpSocket *sshServer;
    	QTcpSocket *sshClient;
    	relayInner(QTcpSocket *);
	~relayInner();
    	void incomingChannel(QTcpSocket *);
	char *remotebuffer;
	int storedBytes;
};

#endif //RELAYINNER_H
