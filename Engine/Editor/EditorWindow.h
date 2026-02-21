#pragma once
#include <QMainWindow>
#include <QListWidgetItem>
#include <QPushButton>

#include "Viewport.h"

namespace rex {

class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow();
    ~EditorWindow();

private slots:
    void onEntitySelected(QListWidgetItem* item);
    void onHierarchySearch(const QString& text);
    void onDeleteEntity();
    void onLoadModel();
    void onTransformChanged();
    void onOpenSettings();

private:
    void setupMenus();
    void createToolbar();
    void createDocks();
    void refreshInspector();

    Viewport*    m_sceneViewport = nullptr;
    Viewport*    m_gameViewport = nullptr;
    QListWidget* m_hierarchyList = nullptr;
    QLineEdit*   m_hierarchySearch = nullptr;
    QPushButton* m_deleteBtn = nullptr;

    QLineEdit* m_posX = nullptr;
    QLineEdit* m_posY = nullptr;
    QLineEdit* m_posZ = nullptr;

    EntityId m_selectedEntity = INVALID_ENTITY;
    bool     m_updatingFromSelection = false;
};

}
