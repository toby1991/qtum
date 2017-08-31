#include "abifunctionfield.h"
#include "abiparamsfield.h"
#include "contractabi.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringListModel>

#include <iostream>
ABIFunctionField::ABIFunctionField(FunctionType type, QWidget *parent) :
    QWidget(parent),
    m_contractABI(0),
    m_comboBoxFunc(new QComboBox(this)),
    m_paramsField(new QStackedWidget(this)),
    m_functionType(type)
{
    // Setup layouts
    m_comboBoxFunc->setMinimumWidth(170);
    m_paramsField->setStyleSheet(".QStackedWidget { border: none; }");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setSpacing(30);
    topLayout->setContentsMargins(0, 0, 0, 0);

    m_labelFunction = new QLabel(tr("Function:"));
    m_labelFunction->setMinimumWidth(110);
    topLayout->addWidget(m_labelFunction);

    topLayout->addWidget(m_comboBoxFunc);
    topLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed));

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_paramsField);
    mainLayout->addStretch(1);
    connect(m_comboBoxFunc, SIGNAL(currentIndexChanged(int)), m_paramsField, SLOT(setCurrentIndex(int)));

    m_comboBoxFunc->setVisible(false);
    m_labelFunction->setVisible(false);
}

void ABIFunctionField::updateABIFunctionField()
{
    if(m_contractABI != NULL)
    {
        // Populate the control with functions
        std::vector<FunctionABI> functions = m_contractABI->functions;
        QStringList functionList;
        QStringListModel *functionModel = new QStringListModel(this);
        for (std::vector<FunctionABI>::const_iterator func = functions.begin() ; func != functions.end(); ++func)
        {
            const FunctionABI &function = *func;
            if((m_functionType == Constructor && function.type != "constructor") ||
                    (m_functionType == Function && function.type == "constructor"))
            {
                continue;
            }

            ABIParamsField *abiParamsField = new ABIParamsField(this);
            abiParamsField->updateParamsField(function);

            m_paramsField->addWidget(abiParamsField);
            QString funcName = QString::fromStdString(function.name);
            QString funcSelector = QString::fromStdString(function.selector());
            functionList.append(QString(funcName + "(" + funcSelector + ")"));

            m_abiFunctionList.append(&function);
        }
        functionModel->setStringList(functionList);
        m_comboBoxFunc->setModel(functionModel);

        if(m_functionType == Function)
        {
            bool visible = m_abiFunctionList.size() > 0;
            m_comboBoxFunc->setVisible(visible);
            m_labelFunction->setVisible(visible);
        }
    }
}

void ABIFunctionField::setContractABI(ContractABI *contractABI)
{
    m_contractABI = contractABI;
    updateABIFunctionField();
}

QString ABIFunctionField::getParamValue(int paramID)
{
    return ((ABIParamsField*)m_paramsField->currentWidget())->getParamValue(paramID);
}

QStringList ABIFunctionField::getParamsValues()
{
    return ((ABIParamsField*)m_paramsField->currentWidget())->getParamsValues();
}

const FunctionABI *ABIFunctionField::getSelectedFunction() const
{
    // Get the currently selected function
    int currentFunc = m_comboBoxFunc->currentIndex();
    if(currentFunc == -1)
        return NULL;

    return m_abiFunctionList[currentFunc];
}

