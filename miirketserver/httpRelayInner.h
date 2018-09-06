#ifndef HTTPRELAYINNER_H
#define HTTPRELAYINNER_H
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class httpRelayInner : public QObject
{
Q_OBJECT

public slots:
	void startReadFromHTTPServer();
	void writeToServer();
	void disconnectedFromHTTPServer();
	void disconnectedFromClient();
	void destroy();

public:
	~httpRelayInner();
        QTcpSocket *httpServer;
        QTcpSocket *httpClient;
	QByteArray httpData;
	int randomID;
signals:
	void removeFromInnerMap(int);
};

#endif //HTTPRELAYINNER_H
