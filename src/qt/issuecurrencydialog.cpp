#include "issuecurrencydialog.h"
#include "ui_issuecurrencydialog.h"

#include "main.h"
#include "walletmodel.h"
#include "bitcoinunits.h"

#include <QMessageBox>
#include <QDebug>

IssueCurrencyDialog::IssueCurrencyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IssueCurrencyDialog)
{
    ui->setupUi(this);
    ui->spend->setEnabled(false);
    ui->spinBox_issue->setValue(1000000);
}

IssueCurrencyDialog::~IssueCurrencyDialog()
{
    delete ui;
}

void IssueCurrencyDialog::setModel(WalletModel *model)
{
    this->model = model;
}

void IssueCurrencyDialog::on_buttonBox_accepted()
{
    CurrencyInfo currency;
    currency.fPOI = true;
    currency.name = ui->lineEdit_name->text().toStdString();
    currency.unit = ui->lineEdit_unit->text().toStdString();
    currency.description = ui->lineEdit_description->text().toStdString();
    currency.issuedividespend = ui->spinBox_IssueSpend->value();

    printf("to value()\n");
    qint64 amount = (QString::number(ui->spend->value()) + "000000").toLongLong();

    if (model->isReadyToIssue()) {
        if (model->toCheckCurrency(currency)){
            if(getPowerToIssueCurrency(amount, currency)) {
                QMessageBox::information(this, tr("Issue Currency"), tr("Check currency ok, the currency will be issued in the background."));
//                if (!model->toIssueCurency(currency, amount)) {
//                    QMessageBox::warning(this, tr("Issue Currency"), tr("Issue currency failed."));
//                }
//                else {
//                    QMessageBox::warning(this, tr("Issue Currency"), tr("Issue currency success."));
//                }
            }
            else {
                QMessageBox::warning(this, tr("Issue Currency"), tr("Get power to issue currency failed."));
            }
        }
        else {
            QMessageBox::warning(this, tr("Check Currency"), tr("Checked currency failed."));
        }
    }
    else {
        QMessageBox::warning(this, tr("Issue Currency"), tr("Issue currency failed. To check: if nodes empty, if block best, if wallet locked."));
    }
}

bool IssueCurrencyDialog::getPowerToIssueCurrency(qint64 amount, const CurrencyInfo &currency)
{
    if(!model)
        return false;
    qint64 balance = model->getBalance(0);
    qint64 value = QString("1000000000000").toLongLong();
    if (balance < value)
    {
        printf("getPowerToIssueCurrency: DZB balance less than 1000000.\n");
        return false;
    }

    QList<SendCoinsRecipient> recipients;
    SendCoinsRecipient recipient;

    recipient.address = "CMMCKi6khMpXPziXGxTA2h7nqGtPbT3Kp4";
    recipient.label = "Get power to issue currency.";
    recipient.amount = amount;
    recipients.append(recipient);

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if (!ctx.isValid())
        return false;

    bool fsendOk = false;
    WalletModel::SendCoinsReturn sendstatus = model->sendCoins(recipients, currency);
    switch(sendstatus.status)
    {
    case WalletModel::InvalidAddress:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("The recepient address is not valid, please recheck."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::InvalidAmount:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("The amount to pay must be at least one cent (0.01)."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountExceedsBalance:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("Amount exceeds your balance"),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("Total exceeds your balance when the %1 transaction fee is included").
            arg(BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, sendstatus.fee)),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::DuplicateAddress:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("Duplicate address found, can only send to each address once in one send operation"),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCreationFailed:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("Error: Transaction creation failed  "),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCommitFailed:
        QMessageBox::warning(this, tr("Send Shares"),
            tr("Error: The transaction was rejected.  This might happen if some of the shares in your portfolio were already spent, such as if you used a copy of wallet.dat and shares were spent in the copy but not marked as spent here."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::Aborted: // User aborted, nothing to do
        break;
    case WalletModel::OK:
        fsendOk = true;
        break;
    }
    return fsendOk;
}

void IssueCurrencyDialog::on_spinBox_IssueSpend_valueChanged(int arg1)
{
    int value = ui->spinBox_issue->value() / arg1;
    ui->spend->setValue(value);
}

void IssueCurrencyDialog::on_spinBox_issue_valueChanged(int arg1)
{
    int value = arg1 / ui->spinBox_IssueSpend->value();
    ui->spend->setValue(value);
}
