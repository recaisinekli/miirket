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

public:
	serverMain();
        ~serverMain();
	void bindNewChannelToWaitingRelay(QTcpSocket *);
	relayTop *waitingRelay;
	QMap<QString, httpRelayTop *> httpServers;

private:
	SslServer server;
	QTcpServer httpClientServer;
	SslServer httpDataServer;
};


#endif //SERVERMAIN_H
