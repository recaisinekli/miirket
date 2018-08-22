/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef SERVERMAIN_H
#define SERVERMAIN_H
#include <QDebug>
#include <QTimer>
#include <QtSql>
#include "json.h"
#include "relayTop.h"
#include <stdlib.h>
#include <time.h>
#include "sslserver.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class serverMain : QObject
{
Q_OBJECT

public slots:
	void acceptConnectionFromUnknownSshServer();
	void startReadFromUnknownSshServer();
	void requestNewChannel();

public:
	serverMain();
    ~serverMain();
	void bindNewChannelToWaitingRelay(QTcpSocket *);
	relayTop *waitingRelay;

private:
	SslServer server;
};


#endif //SERVERMAIN_H
