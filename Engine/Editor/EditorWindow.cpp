#include "EditorWindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QToolBar>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QDialogButtonBox>

namespace rex {

EditorWindow::EditorWindow() {
    setWindowTitle("Rex Engine Professional Editor");
    resize(1920, 1080);

    setMenuBar(new QMenuBar(this));
    setStatusBar(new QStatusBar(this));

    // Unreal-like dark theme
    setStyleSheet(R"(
        QMainWindow { background: #1f1f1f; color: #e0e0e0; }
        QMenuBar { background: #1b1b1b; color: #e0e0e0; }
        QMenuBar::item:selected { background: #303030; }
        QMenu { background: #1b1b1b; color: #e0e0e0; }
        QDockWidget::title { background: #242424; padding: 4px; }
        QLineEdit, QListWidget, QTabWidget::pane { background: #202020; color: #e0e0e0; border: 1px solid #2b2b2b; }
        QPushButton { background: #2b2b2b; color: #e0e0e0; border: 1px solid #3a3a3a; padding: 4px 8px; }
        QPushButton:hover { background: #3a3a3a; }
        QToolBar { background: #1b1b1b; border: none; }
        QStatusBar { background: #1b1b1b; color: #cfcfcf; }
    )");

    QTabWidget* centerTabs = new QTabWidget(this);
    m_sceneViewport = new Viewport(this);
    m_sceneViewport->setObjectName("SceneViewport");
    m_sceneViewport->setShowGrid(true);
    m_gameViewport = new Viewport(this);
    m_gameViewport->setObjectName("GameViewport");
    m_gameViewport->setShowGrid(false);

    centerTabs->addTab(m_sceneViewport, "Scene");
    centerTabs->addTab(m_gameViewport, "Game");
    setCentralWidget(centerTabs);

    createToolbar();
    createDocks();
    setupMenus();

    // Spawn a default cube so Hierarchy stays in sync with the scene
    if (m_sceneViewport && m_hierarchyList) {
        EntityId e = m_sceneViewport->addCubeEntity();
        if (e != INVALID_ENTITY) {
            auto* item = new QListWidgetItem(QString("Entity %1 (Cube)").arg(e));
            item->setData(Qt::UserRole, QVariant::fromValue<uint32_t>(e));
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            m_hierarchyList->addItem(item);
        }
    }
    if (m_gameViewport) {
        m_gameViewport->addCubeEntity();
    }

    statusBar()->showMessage("Rex Engine Ready");
}

EditorWindow::~EditorWindow() = default;

void EditorWindow::setupMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Load Model", this, &EditorWindow::onLoadModel);
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", this, &QWidget::close);

    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("Editor Settings", this, &EditorWindow::onOpenSettings);

    QMenu* modelMenu = menuBar()->addMenu("&Model");
    modelMenu->addAction("Add Cube", [this]() {
        if (!m_sceneViewport || !m_hierarchyList)
            return;

        EntityId e = m_sceneViewport->addCubeEntity();
        if (e != INVALID_ENTITY) {
            auto* item = new QListWidgetItem(QString("Entity %1 (Cube)").arg(e));
            item->setData(Qt::UserRole, QVariant::fromValue<uint32_t>(e));
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            m_hierarchyList->addItem(item);
        }
    });
}

void EditorWindow::createToolbar() {
    QToolBar* tb = addToolBar("Main");
    tb->addAction("Select");
    tb->addAction("Move", this, [this]() {
        if (m_sceneViewport) m_sceneViewport->setGizmoMode(Viewport::GizmoMode::Translate);
    });
    tb->addAction("Rotate", this, [this]() {
        if (m_sceneViewport) m_sceneViewport->setGizmoMode(Viewport::GizmoMode::Rotate);
    });
    tb->addAction("Scale", this, [this]() {
        if (m_sceneViewport) m_sceneViewport->setGizmoMode(Viewport::GizmoMode::Scale);
    });
    tb->addAction("Delete", this, &EditorWindow::onDeleteEntity);
}

void EditorWindow::createDocks() {
    QDockWidget* hierarchyDock = new QDockWidget("Hierarchy", this);

    QWidget* hierarchyWidget = new QWidget(hierarchyDock);
    QVBoxLayout* hierarchyLayout = new QVBoxLayout(hierarchyWidget);
    hierarchyLayout->setContentsMargins(6, 6, 6, 6);

    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_hierarchySearch = new QLineEdit();
    m_hierarchySearch->setPlaceholderText("Search...");
    connect(m_hierarchySearch, &QLineEdit::textChanged,
            this, &EditorWindow::onHierarchySearch);
    QPushButton* addCubeBtn = new QPushButton("Add Cube");
    connect(addCubeBtn, &QPushButton::clicked, [this]() {
        if (!m_sceneViewport || !m_hierarchyList)
            return;

        EntityId e = m_sceneViewport->addCubeEntity();
        if (e != INVALID_ENTITY) {
            auto* item = new QListWidgetItem(QString("Entity %1 (Cube)").arg(e));
            item->setData(Qt::UserRole, QVariant::fromValue<uint32_t>(e));
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            m_hierarchyList->addItem(item);
        }
    });
    m_deleteBtn = new QPushButton("Delete");
    connect(m_deleteBtn, &QPushButton::clicked, this, &EditorWindow::onDeleteEntity);

    searchLayout->addWidget(m_hierarchySearch, 1);
    searchLayout->addWidget(addCubeBtn);
    searchLayout->addWidget(m_deleteBtn);

    m_hierarchyList = new QListWidget();
    connect(m_hierarchyList, &QListWidget::itemClicked,
            this, &EditorWindow::onEntitySelected);

    hierarchyLayout->addLayout(searchLayout);
    hierarchyLayout->addWidget(m_hierarchyList);
    hierarchyDock->setWidget(hierarchyWidget);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);

    // Panels (Tabbed)
    QDockWidget* detailsDock = new QDockWidget("Details", this);
    QWidget* detailsWidget = new QWidget(detailsDock);
    QVBoxLayout* rootLayout = new QVBoxLayout(detailsWidget);
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

    rootLayout->addLayout(form);
    rootLayout->addWidget(applyBtn);
    rootLayout->addStretch();

    detailsDock->setWidget(detailsWidget);
    addDockWidget(Qt::RightDockWidgetArea, detailsDock);

    // Bottom docks (Content + Output Log)
    QDockWidget* bottomDock = new QDockWidget("Content / Output", this);
    QTabWidget* bottomTabs = new QTabWidget(bottomDock);
    QWidget* contentTab = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentTab);
    contentLayout->addWidget(new QLabel("Content Browser (TODO)"));
    contentLayout->addStretch();
    QWidget* outputTab = new QWidget();
    QVBoxLayout* outputLayout = new QVBoxLayout(outputTab);
    outputLayout->addWidget(new QLabel("Output Log (TODO)"));
    outputLayout->addStretch();
    bottomTabs->addTab(contentTab, "Content Browser");
    bottomTabs->addTab(outputTab, "Output Log");
    bottomDock->setWidget(bottomTabs);
    addDockWidget(Qt::BottomDockWidgetArea, bottomDock);
}

void EditorWindow::onEntitySelected(QListWidgetItem* item) {
    if (!item || !m_sceneViewport)
        return;
    m_selectedEntity = item->data(Qt::UserRole).toUInt();
    m_sceneViewport->setSelectedEntity(m_selectedEntity);
    refreshInspector();
}

void EditorWindow::onTransformChanged() {
    if (!m_sceneViewport || m_selectedEntity == INVALID_ENTITY)
        return;

    bool okX = false, okY = false, okZ = false;
    float x = m_posX->text().toFloat(&okX);
    float y = m_posY->text().toFloat(&okY);
    float z = m_posZ->text().toFloat(&okZ);
    if (!okX || !okY || !okZ)
        return;

    m_sceneViewport->setSelectedPosition(Vec3{x, y, z});
}

void EditorWindow::onLoadModel() {
    QFileDialog::getOpenFileName(
        this, "Import Model", "", "3D Models (*.obj *.fbx)");
}

void EditorWindow::onHierarchySearch(const QString& text) {
    if (!m_hierarchyList)
        return;
    for (int i = 0; i < m_hierarchyList->count(); ++i) {
        auto* item = m_hierarchyList->item(i);
        bool match = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

void EditorWindow::onDeleteEntity() {
    if (!m_hierarchyList || !m_sceneViewport)
        return;
    auto* item = m_hierarchyList->currentItem();
    if (!item)
        return;

    EntityId id = item->data(Qt::UserRole).toUInt();
    if (id == INVALID_ENTITY)
        return;

    m_sceneViewport->destroyEntity(id);
    delete item;
    if (m_selectedEntity == id) {
        m_selectedEntity = INVALID_ENTITY;
        m_sceneViewport->setSelectedEntity(INVALID_ENTITY);
        refreshInspector();
    }
}

void EditorWindow::refreshInspector() {
    if (!m_sceneViewport || m_selectedEntity == INVALID_ENTITY) {
        m_posX->setText("0");
        m_posY->setText("0");
        m_posZ->setText("0");
        return;
    }

    auto* t = m_sceneViewport->getSelectedTransform();
    if (!t)
        return;

    m_posX->setText(QString::number(t->position.x));
    m_posY->setText(QString::number(t->position.y));
    m_posZ->setText(QString::number(t->position.z));
}

void EditorWindow::onOpenSettings() {
    QDialog dlg(this);
    dlg.setWindowTitle("Editor Settings");
    QVBoxLayout* layout = new QVBoxLayout(&dlg);
    QFormLayout* form = new QFormLayout();

    auto* sensitivity = new QDoubleSpinBox();
    sensitivity->setRange(0.01, 1.0);
    sensitivity->setSingleStep(0.01);
    sensitivity->setValue(m_sceneViewport ? m_sceneViewport->getMouseSensitivity() : 0.08);

    auto* moveSpeed = new QDoubleSpinBox();
    moveSpeed->setRange(0.01, 2.0);
    moveSpeed->setSingleStep(0.01);
    moveSpeed->setValue(m_sceneViewport ? m_sceneViewport->getMoveSpeed() : 0.1);

    form->addRow("Mouse Sensitivity", sensitivity);
    form->addRow("Move Speed", moveSpeed);
    layout->addLayout(form);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);

    if (dlg.exec() == QDialog::Accepted && m_sceneViewport) {
        m_sceneViewport->setMouseSensitivity(float(sensitivity->value()));
        m_sceneViewport->setMoveSpeed(float(moveSpeed->value()));
    }
}

}
