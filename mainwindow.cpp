#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QtDebug>

#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // init UI signals
    QObject::connect(ui->pbReloadPorts, &QPushButton::clicked,
                     this, &MainWindow::loadPortList);

    QObject::connect(ui->pbOpenPort, &QPushButton::clicked,
                     this, &MainWindow::togglePort);

    QObject::connect(this, &MainWindow::portToggled,
                     this, &MainWindow::onPortToggled);

    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &MainWindow::selectPort);

    QObject::connect(ui->cbBaudRate,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &MainWindow::selectBaudRate);

    QObject::connect(&(this->serialPort), &QSerialPort::readyRead,
                     this, &MainWindow::onDataReady);

    QObject::connect(ui->spNumOfSamples, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
                     this, &MainWindow::onNumOfSamplesChanged);

    // init port signals
    QObject::connect(&(this->serialPort), SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(onPortError(QSerialPort::SerialPortError)));

    loadPortList();
    loadBaudRateList();
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText("9600"));

    // init plot
    numOfSamples = ui->spNumOfSamples->value();
    dataArray.resize(numOfSamples);
    dataX.resize(numOfSamples);
    for (int i = 0; i < dataX.size(); i++)
    {
        dataX[i] = i;
    }
    curve.setSamples(dataX, dataArray);
    curve.attach(ui->plot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPortList()
{
    QString currentSelection = ui->cbPortList->currentText();

    ui->cbPortList->clear();

    for (auto port : QSerialPortInfo::availablePorts())
    {
        ui->cbPortList->addItem(port.portName());
    }

    // find current selection in the new list, maybe it doesn't exist anymore?
    int currentSelectionIndex = ui->cbPortList->findText(currentSelection);
    if (currentSelectionIndex >= 0)
    {
        ui->cbPortList->setCurrentIndex(currentSelectionIndex);
    }
    else // our port doesn't exist anymore, close port if it's open
    {
        if (serialPort.isOpen()) togglePort();
    }
}

void MainWindow::loadBaudRateList()
{
    ui->cbBaudRate->clear();

    for (auto baudRate : QSerialPortInfo::standardBaudRates())
    {
        ui->cbBaudRate->addItem(QString::number(baudRate));
    }
}

void MainWindow::togglePort()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
        qDebug() << "Port closed, " << serialPort.portName();
        emit portToggled(false);
    }
    else
    {
        serialPort.setPortName(ui->cbPortList->currentText());

        // open port
        if (serialPort.open(QIODevice::ReadWrite))
        {
            qDebug() << "Port opened, " << serialPort.portName();
            emit portToggled(true);

            // set baud rate
            if (!serialPort.setBaudRate(ui->cbBaudRate->currentText().toInt()))
            {
                qDebug() << "Set baud rate failed during port opening: "
                         << serialPort.error();
            }
        }
        else
        {
            qDebug() << "Port open error: " << serialPort.error();
        }
    }
}

void MainWindow::selectPort(QString portName)
{
    // has selection actually changed
    if (portName != serialPort.portName())
    {
        // if another port is already open, close it by toggling
        if (serialPort.isOpen())
        {
            togglePort();

            // open new selection by toggling
            togglePort();
        }
    }
}

void MainWindow::selectBaudRate(QString baudRate)
{
    if (serialPort.isOpen())
    {
        if (!serialPort.setBaudRate(baudRate.toInt()))
        {
            qDebug() << "Set baud rate failed during select: "
                     << serialPort.error();
        }
        else
        {
            qDebug() << "Baud rate changed: " << serialPort.baudRate();
        }
    }
}

void MainWindow::onPortToggled(bool open)
{
    ui->pbOpenPort->setChecked(open);
}

void MainWindow::onDataReady()
{
    if (ui->actionStart->isChecked())
    {
        QByteArray data = serialPort.readAll();
        addData((unsigned char)(data[0]));
    }
}

void MainWindow::onPortError(QSerialPort::SerialPortError error)
{
    switch(error)
    {
        case QSerialPort::NoError :
            break;
        case QSerialPort::ResourceError :
            qDebug() << "Port error: resource unavaliable; most likely device removed.";
            if (serialPort.isOpen())
            {
                qDebug() << "Closing port on resource error: " << serialPort.portName();
                togglePort();
            }
            loadPortList();
            break;
        default:
            qDebug() << "Unhandled port error: " << error;
            break;
    }

}

void MainWindow::addData(double data)
{
    // shift data array and place new data at the end
    for (int i = 0; i < dataArray.size()-1; i++)
    {
        dataArray[i] = dataArray[i+1];
    }
    dataArray.last() = data;

    // update plot
    curve.setSamples(dataX, dataArray);
    ui->plot->replot();
}

void MainWindow::onNumOfSamplesChanged(int value)
{
    int oldNum = this->numOfSamples;
    numOfSamples = value;

    // resize data arrays
    if (numOfSamples < oldNum)
    {
        dataX.resize(numOfSamples);
        dataArray.remove(0, oldNum - numOfSamples);
    }
    else if(numOfSamples > oldNum)
    {
        dataX.resize(numOfSamples);
        for (int i = oldNum; i < numOfSamples; i++)
        {
            dataX[i] = i;
            dataArray.prepend(0);
        }
    }
}
