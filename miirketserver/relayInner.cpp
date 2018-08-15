/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "relayInner.h"

relayInner::relayInner(QTcpSocket *newClient){
    this->sshClient = newClient;
	qDebug()<< "I DONT KNOW THE ROUTE YET";
	connect(this->sshClient, SIGNAL(readyRead()), this, SLOT(startReadFromSshClient()));
}

relayInner::~relayInner(){
	if(this->sshServer!=NULL) this->sshServer->deleteLater();
	qDebug()<<"INNER RELAY DELETED";
}

void relayInner::incomingChannel(QTcpSocket *incomingChannel){
    this->sshServer = incomingChannel;
	qDebug()<< "I KNOW THE ROUTE TO SSH SERVER";
	connect(incomingChannel, SIGNAL(readyRead()), this, SLOT(startReadFromSshServer()));

	this->sshServer->write(this->remotebuffer, storedBytes);
	delete this->remotebuffer;
}

void relayInner::startReadFromSshClient(){

	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());

	if(this->sshServer==NULL){
        qDebug()<<"GOT DATA FROM CLIENT BUT THERE IS NO ROUTE TO SSH SERVER";
        qint64 bytesToRead=tcpclient->bytesAvailable();
        storedBytes = bytesToRead;
		remotebuffer = new char[bytesToRead+1];
        tcpclient->read(this->remotebuffer,bytesToRead);
    }else{
        this->sshServer->write(tcpclient->readAll());
	}
}

void relayInner::startReadFromSshServer(){
	QTcpSocket *sshserver = qobject_cast<QTcpSocket *>(sender());
	this->sshClient->write(sshserver->readAll());
}

void relayInner::disconnectedFromSshClient(){
	qDebug()<<"DISCONNECTED FROM SSH CLIENT";
	this->sshClient->deleteLater();
	this->deleteLater();
}
