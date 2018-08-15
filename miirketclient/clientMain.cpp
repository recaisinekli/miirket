/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

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

	foreach(QVariant plugin, clientinfos["ports"].toList()) {
		JsonObject ports = QtJson::clone(plugin).toMap();

		connectorTop *connector1 = new connectorTop();
		connector1->client = new QSslSocket();
		connector1->client->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

		connector1->clientinfo["server_address"] = clientinfos["server_address"];
		connector1->clientinfo["server_port"] = clientinfos["server_port"];
		connector1->clientinfo["localport"] = ports["local"];
		connector1->clientinfo["remoteport"] = ports["remote"];
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
