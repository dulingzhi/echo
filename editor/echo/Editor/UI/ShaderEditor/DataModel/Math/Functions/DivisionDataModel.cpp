#include "DivisionDataModel.h"
#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>
#include "ShaderScene.h"
#include "OperationRules.h"

namespace DataFlowProgramming
{
    DivisionDataModel::DivisionDataModel()
    {
        m_inputDataTypes = 
        {
            {"any", "A"},
            {"any", "B"},
        };

        m_inputs.resize(m_inputDataTypes.size());

        m_outputs.resize(1);
        m_outputs[0] = std::make_shared<DataInvalid>(this);
        m_outputs[0]->setVariableName(getVariableName());
    }

    QJsonObject DivisionDataModel::save() const
    {
        QJsonObject modelJson = NodeDataModel::save();
        return modelJson;
    }

    void DivisionDataModel::restore(QJsonObject const &p)
    {
    }

    void DivisionDataModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
    {
		m_inputs[portIndex] = std::dynamic_pointer_cast<ShaderData>(nodeData);
		if (m_inputs[0] && m_inputs[1])
		{
			m_outputs[0] = OperationRules::instance().NewDivisionOutput(DataAny::getInternalData(m_inputs[0])->type().id, DataAny::getInternalData(m_inputs[1])->type().id, this);
			m_outputs[0]->setVariableName(getVariableName());
		}
		else
		{
			m_outputs[0] = std::make_shared<DataInvalid>(this);
			m_outputs[0]->setVariableName(getVariableName());
		}

        Q_EMIT dataUpdated(0);
    }

    bool DivisionDataModel::generateCode(Echo::ShaderCompiler& compiler)
    {
        if (m_inputs[0] && m_inputs[1])
        {
            compiler.addCode(Echo::StringUtil::Format("\t%s %s = %s / %s;\n",
                m_outputs[0]->type().id.c_str(),
                m_outputs[0]->getVariableName().c_str(),
                DataAny::getInternalData(m_inputs[0])->getVariableName().c_str(),
                DataAny::getInternalData(m_inputs[1])->getVariableName().c_str()));
        }

        return true;
    }
}
