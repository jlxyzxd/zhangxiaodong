#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "pushbuttonitemdelegate.h"
#include "qmessagebox.h"
#include "delegate/ReadOnlyDelegate.h"
#include "delegate/LineEditDelegate.h"
#include "delegate/SpinBoxDelegate.h"
#include "delegate/ComboBoxDelegate.h"
#include "delegate/ProgressBarDelegate.h"
#include "delegate/IconDelegate.h"
#include "delegate/DateTimeEditDelegate.h"
#include "delegate/ButtonDelegate.h"
#include "qdesktopwidget.h"

#define SETCALIBRATIONCOUNT11 2
#define SETCALIBRATIONCOUNT12 6
#define SETCALIBRATIONCOUNT13 4
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(822, 507);
    this->setMinimumSize(822, 507);//(850, 680)

    ui->lineEdit_Port->setText("502");
    ui->pushButton_Send->setEnabled(false);

    server = new QTcpServer();
	socket = NULL;

    connect(server,&QTcpServer::newConnection,this,&MainWindow::server_New_Connect);

	db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("agc");
    db.open("root", "admin");

	connect(timerSend, SIGNAL(timeout()), this, SLOT(onTimeout()));
	m_bRecData = false;
	//初始化tableview
	InitTableView();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboBox_COM->addItem(info.portName());
    }
    ui->comboBox_COM->setCurrentIndex(2);
    ui->comboBox_Botelv->addItem("115200");
    ui->comboBox_Botelv->addItem("9600");
    tableTree = ui->treeView_bmsCalibration;
    tableTree->setStyleSheet("QTreeView::item { border: 1px solid #d9d9d9; border-top-color: transparent; border-bottom-color: transparent; }");
    initTreeData(tableTree);
    ReadOnlyDelegate* readOnly = new ReadOnlyDelegate(tableTree);
    LineEditDelegate* lineEdit = new LineEditDelegate(tableTree);
    SpinBoxDelegate* spinBox = new SpinBoxDelegate(tableTree);
    ComboBoxDelegate* comboBox = new ComboBoxDelegate(tableTree);
    comboBox->setItems(QStringList()<<""<<"");
    ProgressBarDelegate* progressBar = new ProgressBarDelegate(tableTree);
    IconDelegate* icon = new IconDelegate(tableTree);
    DateTimeEditDelegate* dateTime = new DateTimeEditDelegate(tableTree);
    ButtonDelegate* readBtn = new ButtonDelegate(tableTree);

    QIcon myicon;
    myicon.addFile(tr("./res/complete.png"));
    readBtn->setIcon(myicon);
    connect(readBtn, &ButtonDelegate::clicked, this, &MainWindow::OnDelegatePushButtonClicked);
    ButtonDelegate* writeBtn = new ButtonDelegate(tableTree);

    myicon.addFile(tr("./res/download_normal.png"));
    writeBtn->setIcon(myicon);
    connect(writeBtn, &ButtonDelegate::clicked, this, &MainWindow::OnDelegatePushButtonClicked);

    tableTree->setItemsExpandable(true);
    tableTree->setItemDelegateForColumn(0, readOnly);
    tableTree->setItemDelegateForColumn(1, lineEdit);
    tableTree->setItemDelegateForColumn(2, readOnly);
    tableTree->setItemDelegateForColumn(3, readBtn);
    tableTree->setItemDelegateForColumn(4, writeBtn);
    tableTree->setItemDelegateForColumn(5, readOnly);
    tableTree->setItemDelegateForColumn(6, readOnly);

    tableTree->expandAll();
    tableTree->showNormal();
}

MainWindow::~MainWindow()
{
    server->close();
    server->deleteLater();
    delete ui;
}
void MainWindow::initTreeData(QTreeView *tableTree)
{
    QString head = tr("校准项,星云设备值1,BMS值1,星云设备值2,BMS值2,单位,k值,b值,读取,写入,说明");
    QStringList xx = head.split(',');
    model->setHorizontalHeaderLabels(head.split(","));

    QStandardItem *item111 = new QStandardItem("电流校准");
    QString strItem11[SETCALIBRATIONCOUNT11];
    for(int i=0;i<SETCALIBRATIONCOUNT11;i++){
        if(0==i){
            strItem11[i] = QString("充电");
        }else{
            strItem11[i] = QString("放电");
        }
    }
    QString dwItem11[SETCALIBRATIONCOUNT11];
    for(int i=0;i<SETCALIBRATIONCOUNT11;i++){
        dwItem11[i] = QString("0.1A");
    }
    QString strItemRemark11[SETCALIBRATIONCOUNT11];//"k:1000对应1,b:单位：0.1A"
    for(int i=0;i<SETCALIBRATIONCOUNT11;i++){
        strItemRemark11[i] = QString("k:1000对应1");
    }
    for (int row = 0; row < SETCALIBRATIONCOUNT11; ++row) {
         QStandardItem *item1 = new QStandardItem(strItem11[row]);
         QStandardItem *item2 = new QStandardItem(QString(tr("")));
         QStandardItem *item3 = new QStandardItem(QString(tr("")));
         QStandardItem *item4 = new QStandardItem(QString(tr("")));
         QStandardItem *item5 = new QStandardItem(QString(tr("")));
         QStandardItem *item6 = new QStandardItem(dwItem11[row]);
         QStandardItem *item7 = new QStandardItem(QString(tr("")));
         QStandardItem *item8 = new QStandardItem(QString(tr("")));
         QStandardItem *item9 = new QStandardItem("读取");
         item9->setData(QString("item%1").arg(1110+row+1),1110+row+1);
         QStandardItem *item10 = new QStandardItem("写入");
         item10->setData(QString("item%1").arg(1110+row+1+40),1110+row+1+40);
         QStandardItem *item11 = new QStandardItem(strItemRemark11[row]);
         QList<QStandardItem*> list;
         list << item1 << item2 << item3 << item4 << item5 << item6 << item7<< item8<< item9<< item10<< item11;
         item111->appendRow(list);
    }
    QString strTemp;
    QString strItem112[SETCALIBRATIONCOUNT12];
    for(int i=0;i<SETCALIBRATIONCOUNT12;i++){
        strTemp = QString("单体%1校准").arg(i);
        strItem112[i] = strTemp;
    }

    QString dwItem112[SETCALIBRATIONCOUNT12];
    for(int i=0;i<SETCALIBRATIONCOUNT12;i++){
        dwItem112[i] = QString("mV");
    }
    QString strItemRemark112[SETCALIBRATIONCOUNT12];
    for(int i=0;i<SETCALIBRATIONCOUNT12;i++){
        strTemp = QString("单体%1校准,系数为1000").arg(i);
        strItemRemark112[i] = strTemp;
    }
    QStandardItem *item112 = new QStandardItem("电压校准");
    for (int row = 0; row < SETCALIBRATIONCOUNT12; ++row) {
         QStandardItem *item1 = new QStandardItem(strItem112[row]);
         QStandardItem *item2 = new QStandardItem(QString(tr("")));
         QStandardItem *item3 = new QStandardItem(QString(tr("")));
         QStandardItem *item4 = new QStandardItem(QString(tr("")));
         QStandardItem *item5 = new QStandardItem(QString(tr("")));
         QStandardItem *item6 = new QStandardItem(dwItem112[row]);
         QStandardItem *item7 = new QStandardItem(QString(tr("")));
         QStandardItem *item8 = new QStandardItem(QString(tr("")));
         QStandardItem *item9 = new QStandardItem("读取");
         item9->setData(QString("item%1").arg(1210+row+1),1210+row+1);
         QStandardItem *item10 = new QStandardItem("写入");
         item10->setData(QString("item%1").arg(1210+row+1+40),1210+row+1+40);
         QStandardItem *item11 = new QStandardItem(strItemRemark112[row]);
         QList<QStandardItem*> list;
         list << item1 << item2 << item3 << item4 << item5 << item6 << item7<< item8<< item9<< item10<< item11;
         item112->appendRow(list);
    }
    QString strItem113[SETCALIBRATIONCOUNT13];
    for(int i=0;i<SETCALIBRATIONCOUNT13;i++){
        strTemp = QString("温度%1校准偏移值").arg(i);
        strItem113[i] = strTemp;
    }
    QString dwItem113[SETCALIBRATIONCOUNT13];
    for(int i=0;i<SETCALIBRATIONCOUNT13;i++){
        dwItem113[i] = QString("0.1℃");
    }
    QStandardItem *item113 = new QStandardItem("温度校准参数配置");
    for (int row = 0; row < SETCALIBRATIONCOUNT13; ++row) {
         QStandardItem *item1 = new QStandardItem(strItem113[row]);
         QStandardItem *item2 = new QStandardItem(QString(tr("")));
         QStandardItem *item3 = new QStandardItem(QString(tr("")));
         QStandardItem *item4 = new QStandardItem(QString(tr("")));
         QStandardItem *item5 = new QStandardItem(QString(tr("")));
         QStandardItem *item6 = new QStandardItem(dwItem113[row]);
         QStandardItem *item7 = new QStandardItem(QString(tr("")));
         QStandardItem *item8 = new QStandardItem(QString(tr("")));
         QStandardItem *item9 = new QStandardItem("读取");
         item9->setData(QString("item%1").arg(1310+row+1),1310+row+1);
         QStandardItem *item10 = new QStandardItem("写入");
         item10->setData(QString("item%1").arg(1310+row+1+40),1310+row+1+40);
         QStandardItem *item11 = new QStandardItem(strItem113[row]);
         QList<QStandardItem*> list;
         list << item1 << item2 << item3 << item4 << item5 << item6 << item7<< item8<< item9<< item10<< item11;
         item113->appendRow(list);
    }
    model->appendRow(item111);
    model->appendRow(item112);
    model->appendRow(item113);
    tableTree->setModel(model);
    tableTree->setColumnWidth(0,320);
    tableTree->setColumnWidth(1,100);
    tableTree->setColumnWidth(2,100);
    tableTree->setColumnWidth(3,100);
    tableTree->setColumnWidth(4,100);
    tableTree->setColumnWidth(5,120);
    tableTree->setColumnWidth(6,100);
    tableTree->setColumnWidth(7,90);
    tableTree->setColumnWidth(8,90);
    tableTree->setColumnWidth(9,90);
    tableTree->setColumnWidth(10,90);
    tableTree->setColumnWidth(11,400);
    tableTree->setStyleSheet("QTreeView::item {height: 40px;}");
    tableTree->setSelectionBehavior(QTreeView::SelectRows);
    tableTree->setSelectionMode(QTreeView::SingleSelection);
    tableTree->setFocusPolicy(Qt::NoFocus);
}
void MainWindow::OnDelegatePushButtonClicked(QModelIndex index)
{

}
void MainWindow::on_pushButton_Listen_clicked()
{
    if(ui->pushButton_Listen->text() == tr("侦听"))
    {
        //从输入框获取端口号
        int port = ui->lineEdit_Port->text().toInt();

        //监听指定的端口
        if(!server->listen(QHostAddress::Any, port))
        {
            //若出错，则输出错误信息
            qDebug()<<server->errorString();
            return;
        }
        //修改按键文字
        ui->pushButton_Listen->setText("取消侦听");
        qDebug()<< "Listen succeessfully!";
		ui->lineEdit_Port->setEnabled(false);
    }
    else
    {
		if (socket!=NULL)
		{
			 //如果正在连接
			if(socket->state() == QAbstractSocket::ConnectedState)
			{
				//关闭连接
				socket->disconnectFromHost();
			}
			//取消侦听
			server->close();
			ui->lineEdit_Port->setEnabled(true);
		}
       
        //修改按键文字
        ui->pushButton_Listen->setText("侦听");
        //发送按键失能
        ui->pushButton_Send->setEnabled(false);
		
    }

}

void MainWindow::on_pushButton_Send_clicked()
{
    qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
    //获取文本框内容并以ASCII码形式发送
  //  socket->write(ui->textEdit_Send->toPlainText().toLatin1());
	QString str = ui->textEdit_Send->toPlainText();
	QByteArray senddata;
	StringToHex(str, senddata);
	socket->write(senddata);
    socket->flush();
}

void MainWindow::on_pushButton_Query_clicked()
{
}

void MainWindow::on_pushButton_intertimerSend_clicked()
{
	QString strTime = ui->lineEdit_interTime->text();
	if (strTime.toFloat()<0.0)
	{
		QMessageBox::information(NULL, "提示", "时间间隔必须>0.01!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	else if (queryList1.count()<1)
	{
		QMessageBox::information(NULL, "提示", "请先读取发送数据！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	ui->pushButton_intertimerSend->setEnabled(false);
	ui->lineEdit_interTime->setEnabled(false);
	ui->pushButton_intertimerStopSend->setEnabled(true);
	timerSend->start((int)(strTime.toFloat()*1000));
}

void MainWindow::on_pushButton_intertimerStopSend_clicked()
{
	ui->pushButton_intertimerSend->setEnabled(true);
	ui->lineEdit_interTime->setEnabled(true);
	ui->pushButton_intertimerStopSend->setEnabled(false);
	if (timerSend->isActive())
	{
		timerSend->stop();
	}
}
void MainWindow::server_New_Connect()
{
    //获取客户端连接
    socket = server->nextPendingConnection();
    //连接QTcpSocket的信号槽，以读取新数据
    QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);
    //发送按键使能
    ui->pushButton_Send->setEnabled(true);

    qDebug() << "A Client connect!";
}

void MainWindow::socket_Read_Data()
{
 /*   QByteArray buffer;
    //读取缓冲区数据
    buffer = socket->readAll();
    if(!buffer.isEmpty())
    {
        QString str = ui->textEdit_Recv->toPlainText();
		str = tr(buffer);
		QByteArray senddata;
		StringToHex(str, senddata);
        //刷新显示
		ui->textEdit_Recv->setText(buffer);
    }*/
	m_RevMutex.lock();
	char* buf;
	recbuf.clear();
	recbuf = socket->readAll();
//	if (recbuf.count()>5)
	{
		ui->textEdit_Recv->clear();
		ui->textEdit_Recv->setText(recbuf.toHex());
		m_bRecData = true;
		m_RevMutex.unlock();
	//	buf = ui->textEdit_Recv->toPlainText.toLatin1().data();
	}
	
}

void MainWindow::socket_Disconnected()
{
    //发送按键失能
    ui->pushButton_Send->setEnabled(false);
    qDebug() << "Disconnected!";
}
void MainWindow::onTimeout()
{
	qDebug() << "Enter timeout processing function\n";
	if (timerSend->isActive())
	{
	//	timerSend->stop();

		if (m_iCurrentSendIndex<queryList3.count())
		{
			if (m_bRecData == true)
			{
				//发送数据
				QString str = queryList3.at(m_iCurrentSendIndex);
				QString strSend = str;
			
				double dSendData = strSend.toDouble();
				doubleTobyte dd;
				dd.buf[0] = 0xed;
				dd.buf[1] = 0x91;
				dd.buf[2] = 0x3f;
				dd.buf[3] = 0x7c;
				dd.buf[4] = 0xce;
				dd.buf[5] = 0x35;
				dd.buf[6] = 0x40;
				dd.buf[7] = 0x5b;
//				str = "111.222";
				dd.f = strSend.toDouble();
				quint8 quSendbuf[8];
				quSendbuf[0] = dd.buf[7];
				quSendbuf[1] = dd.buf[6];
				quSendbuf[2] = dd.buf[5];
				quSendbuf[3] = dd.buf[4];
				quSendbuf[4] = dd.buf[3];
				quSendbuf[5] = dd.buf[2];
				quSendbuf[6] = dd.buf[1];
				quSendbuf[7] = dd.buf[0];
				QString strResult = GetTCPModbusSenddata(quSendbuf);
				ui->textEdit_Send->setText(strResult);
				on_pushButton_Send_clicked();
				//			QMessageBox::information(NULL, "提示", strResult, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			}
			else
			{
				qDebug() << "wait modbus read...";
			}
			
		}
		else
		{
			timerSend->stop();
			QMessageBox::information(NULL, "提示", "数据已发送完成！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		}
		
	}
}

QString MainWindow::GetModbusSenddata(QString inputstrData)
{
	int iCurrentGW = 1;
	QString strSend,strResult;
	strSend.sprintf("%02X", iCurrentGW);
	strSend += "060004";
	QString crc;
	crc.sprintf("%04X", inputstrData.toInt());
	strSend += crc.right(4);
	QByteArray buff;
	buff.resize(6);
	StringToHex(strSend, buff);
	auto data = QByteArray::fromHex(strSend.toLocal8Bit());
	quint16 crc16;
	crc16 = crc16ForModbus(data);
	crc = QString::number(crc16, 16).toUpper();

	strResult = strSend;
	strResult += crc.right(2);
	strResult += crc.left(2);
	
	return strResult;
}
char ConvertHexChar(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return (-1);
}


void MainWindow::StringToHex(QString str, QByteArray &senddata)
{
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.length();
	senddata.resize(len / 2);
	char lstr, hstr;
	for (int i = 0; i < len;)
	{
		//char lstr,
		hstr = str[i].toLatin1();
		if (hstr == ' ')
		{
			i++;
			continue;
		}
		i++;
		if (i >= len)
			break;
		lstr = str[i].toLatin1();
		hexdata = ConvertHexChar(hstr);
		lowhexdata = ConvertHexChar(lstr);
		if ((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata * 16 + lowhexdata;
		i++;
		senddata[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	senddata.resize(hexdatalen);
}

quint16 MainWindow::crc16ForModbus(const QByteArray &data)
{
	static const quint16 crc16Table[] =
	{
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
	};
	quint8 buf;
	quint16 crc16 = 0xFFFF;

	for (auto i = 0; i < data.size(); ++i)
	{
		buf = data.at(i) ^ crc16;
		crc16 >>= 8;
		crc16 ^= crc16Table[buf];
	}

	return crc16;
}
QString MainWindow::GetTCPModbusSenddata(quint8* inputstrData)
{
	m_RevMutex.lock();
	int iCurrentGW = 1;
	QString strSend, strResult;
	strSend.sprintf("%02X", recbuf.at(0));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", recbuf.at(1));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", recbuf.at(2));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", recbuf.at(3));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", recbuf.at(4));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", 0x0b);
	strResult += strSend.right(2);
	strSend.sprintf("%02X", recbuf.at(6));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", recbuf.at(7));
	strResult += strSend.right(2);
	strSend.sprintf("%02X", 8);
	strResult += strSend.right(2);
	QString crc;
	crc.sprintf("%02X", inputstrData[0]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[1]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[2]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[3]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[4]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[5]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[6]);
	strResult += crc.right(2);
	crc.sprintf("%02X", inputstrData[7]);
	strResult += crc.right(2);
	m_RevMutex.unlock();

	return strResult;
}
bool MainWindow::InitTableView()
{
	return true;
}

void MainWindow::on_pushButton_StartCV_clicked()
{
    char* buf;
    QString strRecv,strTemp;
    int iStrLen = 0;
    ui->textEdit_Recv->setText("");
    buf = ui->textEdit_Recv->toPlainText().toLatin1().data();
    strRecv = buf;
    iStrLen = strRecv.length();
    if ((iStrLen>22)&&(strRecv.left(16)=="('item':'01VV01'")){
        strTemp = strRecv.right(6);
        strTemp = strTemp.left(4);
    }
}


void MainWindow::on_btn_avalibCOM_clicked()
{
    ui->comboBox_COM->clear();

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboBox_COM->addItem(info.portName());
    }
}


void MainWindow::on_btn_openCOM_2_clicked()
{
    QString str = ui->btn_openCOM->text();
    if(str==QString("打开串口"))
     {
        if (serial.isOpen())
        {
            serial.close();
        }

        QString strIP,strPort,strBotelv;
        QByteArray btIP,btCOM;
        btCOM = ui->comboBox_COM->currentText().toLocal8Bit();
        strBotelv = ui->comboBox_Botelv->currentText();

        m_tcpModbus=modbus_new_rtu(btCOM.data(),strBotelv.toInt(),'N',8,1);
        if (m_tcpModbus == NULL)
        {
            QMessageBox::about(NULL, QStringLiteral("警告"), "Unable to allocate libmodbus contex\n");
            return ;
        }

        QString strAddrID = "1";
        modbus_set_slave(m_tcpModbus, strAddrID.toInt());

        modbus_connect(m_tcpModbus);

         ui->comboBox_COM->setEnabled(false);
         ui->comboBox_Botelv->setEnabled(false);
         ui->btn_openCOM->setText(QString("关闭串口"));

     }
     else
     {
        modbus_close(m_tcpModbus);
        modbus_free(m_tcpModbus);
        m_tcpModbus = NULL;

         ui->comboBox_COM->setEnabled(true);
         ui->comboBox_Botelv->setEnabled(true);
         ui->btn_openCOM->setText(QString("打开串口"));
     }
}


