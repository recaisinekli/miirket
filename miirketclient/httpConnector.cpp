/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "httpConnector.h"

httpConnector::~httpConnector(){
	delete this->timer1;
        delete this->heartbeatTimer;
	httpClient->deleteLater();
}

void httpConnector::startTransfer(){
	this->timer1->stop();
	this->heartbeatTimer = new QTimer();
        connect(this->heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));;
        this->heartbeatTimer->start(60000*3);

	this->clientinfo["command"] = "register";

	QByteArray clientdata = QtJson::serialize(this->clientinfo);
	httpClient->write(clientdata, clientdata.length());
}

void httpConnector::startReadFromRemote(){
	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	QString remotedata = tcpclient->readAll();
        bool ok;
        JsonObject remoteinfo = QtJson::parse(remotedata, ok).toMap();

        if(remoteinfo["command"].toString()=="registered"){
		qDebug()<<"REGISTERED TO THE SERVER WITH SUBDOMAIN "<< this->clientinfo["subdomain"].toString();
	}
	else if(remoteinfo["command"].toString()=="subdomaintaken"){
		qDebug()<<"SUBDOMAIN "<< this->clientinfo["subdomain"].toString()<<" IS ALREADY TAKEN. TRY ANOTHER ONE.";
		this->deleteLater();
	}
	else if(remoteinfo["command"].toString()=="createnewchannel"){
		httpLocalConnector *l1 = new httpLocalConnector();
		l1->randomID = remoteinfo["randomid"].toInt();
		l1->serverAddress = this->clientinfo["server_address"].toString();
		l1->serverPort = this->clientinfo["server_port"].toInt();
		l1->localPort = this->clientinfo["localport"].toInt();
		l1->subdomain = this->clientinfo["subdomain"].toString();
		connect(l1->httpClient, SIGNAL(encrypted()), l1, SLOT(startTransfer()));
                connect(l1->httpClient, SIGNAL(readyRead()), l1, SLOT(startReadFromRemote()));
		connect(l1->httpClient, SIGNAL(disconnected()), l1, SLOT(disconnectedFromRemote()));
		l1->tryToConnect();
	}
}

void httpConnector::tryToConnect(){
	if(httpClient->state()==0){
		httpClient->connectToHostEncrypted(this->clientinfo["server_address"].toString(), this->clientinfo["server_port"].toInt());
	}
}

void httpConnector::disconnectedFromServer(){
	this->timer1->start(5000);
	this->heartbeatTimer->deleteLater();
}

void httpConnector::heartbeat(){
        JsonObject clientinfo;
        clientinfo["command"] = "heartbeat";
	clientinfo["subdomain"] = this->clientinfo["subdomain"].toString();
        QByteArray clientdata = QtJson::serialize(clientinfo);
        this->httpClient->write(clientdata, clientdata.length());
}
