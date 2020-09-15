#include "procedural_geometry_panel.h"
#include "engine/core/editor/editor.h"
#include "engine/core/editor/qt/QWidgets.h"
#include "engine/core/base/class_method_bind.h"
#include "engine/core/util/PathUtil.h"
#include "engine/core/util/StringUtil.h"
#include "engine/core/io/MemoryReader.h"
#include "engine/core/util/Buffer.h"
#include "engine/core/io/IO.h"
#include "engine/core/render/base/image/Image.h"
#include "engine/core/render/base/atla/TextureAtlas.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	ProceduralGeometryPanel::ProceduralGeometryPanel(Object* obj)
	{
		m_proceduralGeometry = ECHO_DOWN_CAST<ProceduralGeometry*>(obj);

		m_ui = EditorApi.qLoadUi("engine/modules/procedural/editor/procedural_geometry_panel.ui");

		QSplitter* splitter = (QSplitter*)EditorApi.qFindChild(m_ui, "m_splitter");
		if (splitter)
		{
			splitter->setStretchFactor(0, 0);
			splitter->setStretchFactor(1, 1);
		}

		// Tool button icons
		EditorApi.qToolButtonSetIcon(EditorApi.qFindChild(m_ui, "m_import"), "engine/core/render/base/editor/icon/import.png");

		// connect signal slots
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_graphicsView"), QSIGNAL(customContextMenuRequested(const QPoint&)), this, createMethodBind(&ProceduralGeometryPanel::onRightClickGraphicsView));
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_nodeTreeWidget"), QSIGNAL(itemClicked(QTreeWidgetItem*, int)), this, createMethodBind(&ProceduralGeometryPanel::onSelectItem));
		EditorApi.qConnectWidget(EditorApi.qFindChild(m_ui, "m_nodeTreeWidget"), QSIGNAL(itemChanged(QTreeWidgetItem*, int)), this, createMethodBind(&ProceduralGeometryPanel::onChangedAtlaName));

		// create QGraphicsScene
		m_graphicsScene = EditorApi.qGraphicsSceneNew();
		EditorApi.qGraphicsViewSetScene(EditorApi.qFindChild(m_ui, "m_graphicsView"), m_graphicsScene);

		// background
		m_backgroundGridSmall.set(EditorApi.qFindChild(m_ui, "m_graphicsView"), m_graphicsScene);
		m_backgroundGridBig.set(EditorApi.qFindChild(m_ui, "m_graphicsView"), m_graphicsScene);

		// pg nodes painter
		m_pgNodesPainter.set(EditorApi.qFindChild(m_ui, "m_graphicsView"), m_graphicsScene);
	}

	ProceduralGeometryPanel::~ProceduralGeometryPanel()
	{
		clearImageItemAndBorder();
	}

	void ProceduralGeometryPanel::update()
	{
		refreshUiDisplay();
	}

	void ProceduralGeometryPanel::onNewAtla()
	{
	}

	void ProceduralGeometryPanel::onRightClickGraphicsView()
	{
		if (!m_menuNew)
		{
			m_menuNew = EchoNew(QMenu(m_ui));

			Echo::StringArray pgNodeClasses;
			Echo::Class::getChildClasses(pgNodeClasses, "PGNode", true);
			for (String& className : pgNodeClasses)
			{
				QAction* newAction = new QAction;
				newAction->setText(Echo::StringUtil::Replace(className, "PG", "").c_str());
				newAction->setData(className.c_str());
				m_menuNew->addAction(newAction);

				EditorApi.qConnectAction(newAction, QSIGNAL(triggered()), this, createMethodBind(&ProceduralGeometryPanel::onNewPGNode));
			}
		}

		m_menuNew->exec(QCursor::pos());
	}

	void ProceduralGeometryPanel::onNewPGNode()
	{
		//QAction* pAction = qobject_cast<QAction*>(QObject::sender());
	}

	void ProceduralGeometryPanel::onSplit()
	{
	}

	void ProceduralGeometryPanel::refreshUiDisplay()
	{
		drawBackground();

		// pg nodes painter
		m_pgNodesPainter.update(m_proceduralGeometry);
	}

	void ProceduralGeometryPanel::drawBackground()
	{
		m_backgroundStyle.m_backgroundColor.setRGBA(77, 77, 77, 255);
		m_backgroundStyle.m_fineGridColor.setRGBA(84, 84, 84, 255);
		m_backgroundStyle.m_coarseGridColor.setRGBA(64, 64, 64, 255);

		EditorApi.qGraphicsViewSetBackgroundBrush(EditorApi.qFindChild(m_ui, "m_graphicsView"), m_backgroundStyle.m_backgroundColor);

		m_backgroundGridSmall.update(15, m_backgroundStyle.m_fineGridColor);
		m_backgroundGridBig.update(150, m_backgroundStyle.m_coarseGridColor);
	}

	void ProceduralGeometryPanel::refreshAtlaList()
	{
	}

	void ProceduralGeometryPanel::clearImageItemAndBorder()
	{
	}

	void ProceduralGeometryPanel::refreshImageDisplay()
	{

	}

	void ProceduralGeometryPanel::onSelectItem()
	{

	}

	void ProceduralGeometryPanel::onChangedAtlaName()
	{

	}
#endif
}