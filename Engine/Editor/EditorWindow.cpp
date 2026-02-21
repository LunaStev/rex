#include "EditorWindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QToolBar>

namespace rex {

EditorWindow::EditorWindow() {
    setWindowTitle("Rex Engine Professional Editor");
    resize(1920, 1080);

    setMenuBar(new QMenuBar(this));
    setStatusBar(new QStatusBar(this));

    m_viewport = new Viewport(this);
    setCentralWidget(m_viewport);

    createToolbar();
    createDocks();
    setupMenus();

    statusBar()->showMessage("Rex Engine Ready");
}

EditorWindow::~EditorWindow() = default;

void EditorWindow::setupMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Load Model", this, &EditorWindow::onLoadModel);
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", this, &QWidget::close);

    QMenu* modelMenu = menuBar()->addMenu("&Model");
    modelMenu->addAction("Add Cube", [this]() {
        if (!m_viewport || !m_hierarchyList)
            return;

        EntityId e = m_viewport->addCubeEntity();
        if (e != INVALID_ENTITY)
            m_hierarchyList->addItem(QString("Entity %1 (Cube)").arg(e));
    });
}

void EditorWindow::createToolbar() {
    QToolBar* tb = addToolBar("Main");
    tb->addAction("Select");
    tb->addAction("Move");
}

void EditorWindow::createDocks() {
    QDockWidget* hierarchyDock = new QDockWidget("Hierarchy", this);
    m_hierarchyList = new QListWidget(hierarchyDock);
    connect(m_hierarchyList, &QListWidget::itemClicked,
            this, &EditorWindow::onEntitySelected);
    hierarchyDock->setWidget(m_hierarchyList);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);

    // Inspector
    QDockWidget* propDock = new QDockWidget("Inspector", this);
    QWidget* propWidget = new QWidget(propDock);

    // ✅ 루트 레이아웃은 하나
    QVBoxLayout* rootLayout = new QVBoxLayout(propWidget);

    // ✅ form은 QWidget parent 없이 생성
    QFormLayout* form = new QFormLayout();
    m_posX = new QLineEdit("0");
    m_posY = new QLineEdit("0");
    m_posZ = new QLineEdit("0");

    form->addRow("Pos X", m_posX);
    form->addRow("Pos Y", m_posY);
    form->addRow("Pos Z", m_posZ);

    QPushButton* applyBtn = new QPushButton("Apply");
    connect(applyBtn, &QPushButton::clicked,
            this, &EditorWindow::onTransformChanged);

    // 레이아웃 구성
    rootLayout->addLayout(form);
    rootLayout->addWidget(applyBtn);
    rootLayout->addStretch();

    propDock->setWidget(propWidget);
    addDockWidget(Qt::RightDockWidgetArea, propDock);
}

void EditorWindow::onEntitySelected(QListWidgetItem* item) {
    Q_UNUSED(item);
    // (선택 로직은 다음 단계에서 구현)
}

void EditorWindow::onTransformChanged() {
    // (Transform 편집은 다음 단계)
}

void EditorWindow::onLoadModel() {
    QFileDialog::getOpenFileName(
        this, "Import Model", "", "3D Models (*.obj *.fbx)");
}

}