#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QDebug>
#include "QSqlDatabase"
#include "QSqlQuery"
#include "QSqlRecord"
#include "QStringListModel"
#include "QMessageBox"
#include "QTimer"
#include "QStandardItem"
#include <QMutex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Listen_clicked();

    void on_pushButton_Send_clicked();
	void on_pushButton_Query_clicked();
	void on_pushButton_intertimerSend_clicked();
	void on_pushButton_intertimerStopSend_clicked();

    void server_New_Connect();

    void socket_Read_Data();

    void socket_Disconnected();
	void onTimeout();

private:
    Ui::MainWindow *ui;
    QTcpServer* server;
    QTcpSocket* socket;
	QSqlDatabase db;
	QStringList queryList1;
	QStringList queryList2;
	QStringList queryList3;
	QStringList queryList4;
	QStringList queryList5;
	QTimer *timerSend = new QTimer(this);
	int m_iCurrentSendIndex = 0;
	QString GetModbusSenddata(QString inputstrData);
	QString GetTCPModbusSenddata(quint8* inputstrData);
	void StringToHex(QString str, QByteArray &senddata);
	quint16 crc16ForModbus(const QByteArray &data);
	QStringListModel* pMode;
	QByteArray recbuf;
	bool m_bRecData;
	QMutex m_RevMutex;
	union doubleTobyte
	{
		double f;
		quint8 buf[8];
	};
	bool InitTableView();
	QStandardItemModel *AGCData_model = new QStandardItemModel();
};

#endif // MAINWINDOW_H
