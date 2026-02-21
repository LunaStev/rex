#pragma once
#include <QMainWindow>
#include <QListWidgetItem>

#include "Viewport.h"

namespace rex {

class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow();
    ~EditorWindow();

private slots:
    void onEntitySelected(QListWidgetItem* item);
    void onLoadModel();
    void onTransformChanged();

private:
    void setupMenus();
    void createToolbar();
    void createDocks();

    Viewport*    m_viewport = nullptr;
    QListWidget* m_hierarchyList = nullptr;

    QLineEdit* m_posX = nullptr;
    QLineEdit* m_posY = nullptr;
    QLineEdit* m_posZ = nullptr;

    EntityId m_selectedEntity = INVALID_ENTITY;
    bool     m_updatingFromSelection = false;
};

}