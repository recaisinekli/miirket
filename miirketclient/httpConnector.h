/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef HTTPCONNECTOR_H
#define HTTPCONNECTOR_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include "json.h"
#include <QSslSocket>
#include "httpLocalConnector.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class httpConnector : public QObject
{
Q_OBJECT

public slots:
	void startTransfer();
	void startReadFromRemote();
	void heartbeat();
	void disconnectedFromServer();
	void tryToConnect();
public:
	~httpConnector();
	QSslSocket *httpClient;
	JsonObject clientinfo;
	QTimer *timer1;
        QTimer *heartbeatTimer;
};

#endif //HTTPCONNECTOR_H#
