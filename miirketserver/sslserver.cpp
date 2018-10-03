/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include <QSslSocket>
#include <QFile>
#include "sslserver.h"

SslServer::SslServer(QObject *parent) : QTcpServer(parent)
{
	QFile keyFile("/path/to/server/private.key");
	keyFile.open(QIODevice::ReadOnly);
	key = QSslKey(keyFile.readAll(), QSsl::Rsa);
	keyFile.close();

	QFile certFile("/path/to/server/certificate.crt");
	certFile.open(QIODevice::ReadOnly);
	cert = QSslCertificate(certFile.readAll());
	certFile.close();
}

void SslServer::incomingConnection(int socketDescriptor)
{
	QSslSocket *sslSocket = new QSslSocket(this);

	connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
	sslSocket->setSocketDescriptor(socketDescriptor);
	sslSocket->setPrivateKey(key);
	sslSocket->setLocalCertificate(cert);
	// add certificate chain
	sslSocket->addCaCertificates("/path/to/intermediateCA.crt");
	sslSocket->addCaCertificates("/path/to/rootCA.crt");
	sslSocket->startServerEncryption();

	addPendingConnection(sslSocket);
}

void SslServer::sslErrors(const QList<QSslError> &errors)
{
	qDebug()<<"ERROR";
	foreach (const QSslError &error, errors)
		qDebug() << error.errorString();
}
