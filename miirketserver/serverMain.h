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
#include "httpRelayTop.h"
#include "httpsRelayTop.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class serverMain : QObject
{
Q_OBJECT

public slots:
	void acceptConnectionFromUnknownSshServer();
	void startReadFromUnknownSshServer();
	void requestNewChannel();

	void acceptConnectionFromHTTPClientServer();
	void startReadFromHTTPClient();

	void acceptConnectionFromHTTPDataServer();
	void startReadFromHTTPDataServer();
	void checkMapForHTTPServer(QString);
	void mapHTTPServer(QString);
	void removeHTTPServerFromMap(QString);

	void acceptConnectionFromHTTPSClientServer();
        void startReadFromHTTPSClient();

        void acceptConnectionFromHTTPSDataServer();
        void startReadFromHTTPSDataServer();
        void checkMapForHTTPSServer(QString);
        void mapHTTPSServer(QString);
        void removeHTTPSServerFromMap(QString);

public:
	serverMain();
        ~serverMain();
	void bindNewChannelToWaitingRelay(QTcpSocket *);
	relayTop *waitingRelay;
	QMap<QString, httpRelayTop *> httpServers;
	QMap<QString, httpsRelayTop *> httpsServers;

private:
	SslServer server;
	QTcpServer httpClientServer;
	SslServer httpDataServer;
	SslServer httpsClientServer;
	SslServer httpsDataServer;
};


#endif //SERVERMAIN_H
