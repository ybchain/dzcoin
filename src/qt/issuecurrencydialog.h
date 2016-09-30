#ifndef ISSUECURRENCYDIALOG_H
#define ISSUECURRENCYDIALOG_H

#include <QDialog>

class WalletModel;
class CurrencyInfo;

namespace Ui {
class IssueCurrencyDialog;
}

class IssueCurrencyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IssueCurrencyDialog(QWidget *parent = 0);
    ~IssueCurrencyDialog();

    void setModel(WalletModel *model);

private slots:
    void on_buttonBox_accepted();

    void on_spinBox_IssueSpend_valueChanged(int arg1);

    void on_spinBox_issue_valueChanged(int arg1);

private:
    Ui::IssueCurrencyDialog *ui;

    WalletModel *model;

    bool getPowerToIssueCurrency(qint64 amount, const CurrencyInfo &currency);

};

#endif // ISSUECURRENCYDIALOG_H
