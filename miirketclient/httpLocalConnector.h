/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef HTTPLOCALCONNECTOR_H
#define HTTPLOCALCONNECTOR_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include <QSslSocket>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class httpLocalConnector : public QObject
{
Q_OBJECT

public slots:
	void startTransfer();
	void startReadFromRemote();
        void startReadFromLocal();
	void disconnectedFromRemote();
	void disconnectedFromLocal();

public:
	httpLocalConnector();
	~httpLocalConnector();
        QTcpSocket localClient;
        QSslSocket *httpClient;
	void tryToConnect();
	int randomID;
	int localPort;
	QString serverAddress;
	int serverPort;
	QString subdomain;
};

#endif //HTTPLOCALCONNECTOR_H#
