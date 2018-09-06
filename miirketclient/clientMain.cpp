#include "clientMain.h"

clientMain::clientMain(){

        QDir::setCurrent(qApp->applicationFilePath());

	QString CONFIGDATA = readConfigFile("client.cfg");
	if (CONFIGDATA.isEmpty()){
        	qFatal("Configuration file not found or empty!");
	}
	bool ok;
        clientinfos = QtJson::parse(CONFIGDATA, ok).toMap();
	if (!ok) {
        	qFatal("Configuration file is not in JSON format!");
    	}

	foreach(QVariant plugin, clientinfos["bindings"].toList()) {
		JsonObject binding = QtJson::clone(plugin).toMap();

		if(binding["type"].toString()=="ssh"){
			connectorTop *connector1 = new connectorTop();
			connector1->client = new QSslSocket();
			connector1->client->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

			connector1->clientinfo["server_address"] = clientinfos["server_address"];
			connector1->clientinfo["server_port"] = clientinfos["server_tcp_port"];
			connector1->clientinfo["localport"] = binding["local"];
			connector1->clientinfo["remoteport"] = binding["remote"];
			connector1->clientinfo["authtoken"] = clientinfos["authtoken"];

			connect(connector1->client, SIGNAL(connected()), connector1, SLOT(startTransfer()));
                	connect(connector1->client, SIGNAL(readyRead()), connector1, SLOT(startReadFromRemote()));
			connect(connector1->client, SIGNAL(disconnected()), connector1, SLOT(disconnectedFromServer()));
			connect(connector1->client, SIGNAL(encrypted()), connector1, SLOT(socketEncrypted()));
        		connect(connector1->client, SIGNAL(sslErrors(QList<QSslError>)), connector1, SLOT(sslErrors(QList<QSslError>)));

			connector1->tryToConnectToServer();
                	connector1->timer1 = new QTimer();
                	connect(connector1->timer1, SIGNAL(timeout()), connector1, SLOT(tryToConnectToServer()));
                	connector1->timer1->start(5000);
		}
		if(binding["type"].toString()=="http"){
			httpConnector *h1 = new httpConnector();
        		h1->httpClient = new QSslSocket();
			h1->clientinfo["localport"] = binding["local"];
			h1->clientinfo["subdomain"] = binding["subdomain"];
			h1->clientinfo["server_address"] = clientinfos["server_address"];
                        h1->clientinfo["server_port"] = clientinfos["server_http_port"];
        		connect(h1->httpClient, SIGNAL(encrypted()), h1, SLOT(startTransfer()));
        		connect(h1->httpClient, SIGNAL(readyRead()), h1, SLOT(startReadFromRemote()));
			connect(h1->httpClient, SIGNAL(disconnected()), h1, SLOT(disconnectedFromServer()));
        		h1->tryToConnect();
			h1->timer1 = new QTimer();
                        connect(h1->timer1, SIGNAL(timeout()), h1, SLOT(tryToConnect()));
                        h1->timer1->start(5000);
		}
	}
}

clientMain::~clientMain(){

}

QString clientMain::readConfigFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}
