/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "connectorInner.h"

connectorInner::connectorInner(){
	client = new QSslSocket();
	localConnected = false;

	connect(this->client, SIGNAL(connected()), this, SLOT(startTransfer()));
	connect(this->client, SIGNAL(readyRead()), this, SLOT(startReadFromRemote()));
	connect(this->client, SIGNAL(disconnected()), this, SLOT(disconnectedFromRemote()));

	localClient = new QTcpSocket();

	connect(localClient, SIGNAL(readyRead()), this, SLOT(startReadFromLocal()));
}

connectorInner::~connectorInner(){
	client->deleteLater();
	localClient->deleteLater();
	sshClient->deleteLater();
	qDebug()<<"INNER CONNECTOR DELETED";
}

void connectorInner::startTransfer(){
	this->clientinfo["command"] = "newchannel";

	QByteArray clientdata = QtJson::serialize(this->clientinfo);
	this->client->write(clientdata, clientdata.length());
}

void connectorInner::startReadFromRemote(){
	if(!this->localConnected){
		this->localConnected = true;
		this->localClient->connectToHost(QHostAddress::LocalHost, this->clientinfo["localport"].toInt());
    }

	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	this->sshClient = tcpclient;
	this->localClient->write(tcpclient->readAll());
}

void connectorInner::startReadFromLocal(){
	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	this->sshClient->write(tcpclient->readAll());
}

void connectorInner::tryToConnect(){
	this->client->connectToHostEncrypted(this->clientinfo["server_address"].toString(), this->clientinfo["server_port"].toInt());
}

void connectorInner::disconnectedFromRemote(){
	emit deleteMe();
	qDebug()<<"DISCONNECTED FROM CLIENT";
}
