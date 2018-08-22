/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "relayTop.h"

void relayTop::incomingChannel(QTcpSocket *newChannel){
        waitingInnerRelay->incomingChannel(newChannel);
}

void relayTop::acceptConnectionFromSshClient(){
        QTcpServer *sshrelay = qobject_cast<QTcpServer *>(sender());
        QTcpSocket *newClient = sshrelay->nextPendingConnection();
        qDebug()<<"NEW CONNECTION FROM SSH CLIENT";

        relayInner *newInnerRelay = new relayInner(newClient);
	connect(newClient, SIGNAL(disconnected()), newInnerRelay, SLOT(disconnectedFromSshClient()));
        waitingInnerRelay = newInnerRelay;
        emit requestNewChannel();
}

void relayTop::disconnectedFromRegisteredSshServer(){
	if(this->sshRelay!=NULL) this->sshRelay->close();
        if(this->sshRelay!=NULL) this->sshRelay->deleteLater();
	if(this->sshServer!=NULL) this->sshServer->deleteLater();
	//if(this->waitingInnerRelay!=nullptr) this->waitingInnerRelay->deleteLater();
	qDebug()<<"DISCONNECTED FROM REGISTERED SSH SERVER";
}
