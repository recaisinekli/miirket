/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>

class SslServer : public QTcpServer
{
	Q_OBJECT

public:
	SslServer(QObject *parent = 0);

private:
	QSslKey key;
	QSslCertificate cert;

private slots:
	void sslErrors(const QList<QSslError> &errors);

protected:
	void incomingConnection(int socketDescriptor);
};

#endif // SSLSERVER_H
