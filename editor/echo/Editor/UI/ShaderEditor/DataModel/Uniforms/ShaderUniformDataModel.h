#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>
#include <nodeeditor/NodeDataModel>
#include <iostream>
#include <string>
#include "QColorSelect.h"
#include "ShaderDataModel.h"
#include <engine/core/render/base/editor/shader/compiler/shader_compiler.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

namespace DataFlowProgramming
{
    class ShaderUniformDataModel : public ShaderDataModel
    {
      Q_OBJECT

    public:
        ShaderUniformDataModel();
        virtual ~ShaderUniformDataModel() {}

		// caption
		virtual QString caption() const override;

        // caption visible
        virtual bool captionVisible() const override { return m_uniformConfig->isExport(); }

        // variable name
        virtual Echo::String getVariableName() const override;

        // show menu
        virtual void showMenu(const QPointF& pos) override;

        // uniform config
        void saveUniformConfig(QJsonObject& p) const;
		void restoreUniformConfig(QJsonObject const& p);

        // get default value
        virtual bool getDefaultValue(Echo::StringArray& uniformNames, Echo::VariantArray& uniformValues)=0;

    public:
        // slot
        virtual bool onNodePressed();

	protected:
		Echo::ShaderNodeUniform*    m_uniformConfig = nullptr;
		QMenu*                      m_menu = nullptr;
		QAction*                    m_setAsParameter = nullptr;
		QAction*                    m_setAsConstant = nullptr;
    };
}

