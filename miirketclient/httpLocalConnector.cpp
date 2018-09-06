#include "httpLocalConnector.h"

httpLocalConnector::httpLocalConnector(){
	connect(&localClient, SIGNAL(readyRead()), this, SLOT(startReadFromLocal()));
	connect(&localClient, SIGNAL(disconnected()), this, SLOT(disconnectedFromLocal()));
	this->httpClient = new QSslSocket();
}

httpLocalConnector::~httpLocalConnector(){
	this->httpClient->deleteLater();
}

void httpLocalConnector::startTransfer(){
	JsonObject serverinfo;
        serverinfo["command"] = "newchannel";
        serverinfo["randomid"] = this->randomID;
	serverinfo["subdomain"] = this->subdomain;
        QByteArray serverdata = QtJson::serialize(serverinfo);
        this->httpClient->write(serverdata, serverdata.length());
}

void httpLocalConnector::startReadFromRemote(){
	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	if(localClient.state()==0){
		localClient.connectToHost(QHostAddress::LocalHost, this->localPort);
	}
        if(localClient.waitForConnected()){
        	localClient.write(tcpclient->readAll());
        }
}

void httpLocalConnector::startReadFromLocal(){
        QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	httpClient->write(tcpclient->readAll());
	httpClient->waitForBytesWritten();
}

void httpLocalConnector::tryToConnect(){
        httpClient->connectToHostEncrypted(this->serverAddress, this->serverPort);
}

void httpLocalConnector::disconnectedFromRemote(){
	this->deleteLater();
}

void httpLocalConnector::disconnectedFromLocal(){
}
