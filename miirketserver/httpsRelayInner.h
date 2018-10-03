/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef HTTPSRELAYINNER_H
#define HTTPSRELAYINNER_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class httpsRelayInner : public QObject
{
Q_OBJECT

public slots:
	void startReadFromHTTPSServer();
	void writeToServer();
	void disconnectedFromHTTPSServer();
	void disconnectedFromClient();
	void destroy();

public:
	~httpsRelayInner();
        QTcpSocket *httpsServer;
        QTcpSocket *httpsClient;
	QByteArray httpsData;
	int randomID;
signals:
	void removeFromInnerMap(int);
};

#endif //HTTPSRELAYINNER_H
