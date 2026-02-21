#include <QApplication>
#include "../Editor/EditorWindow.h"
#include <iostream>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    
    rex::EditorWindow window;
    
    // Modern Dark Theme Stylesheet (Inspired by Unreal/Unity)
    app.setStyleSheet(R"(
        QMainWindow, QDockWidget {
            background-color: #1e1e1e;
            color: #d4d4d4;
        }
        QMenuBar, QMenuBar::item {
            background-color: #252526;
            color: #d4d4d4;
        }
        QMenuBar::item:selected {
            background-color: #3e3e42;
        }
        QDockWidget::title {
            background-color: #2d2d2d;
            text-align: center;
        }
        QListWidget, QTreeView, QTextEdit {
            background-color: #1e1e1e;
            border: 1px solid #3e3e42;
            color: #d4d4d4;
        }
        QLineEdit {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 2px;
        }
        QPushButton {
            background-color: #0e639c;
            color: white;
            border: none;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
        QToolBar {
            background-color: #252526;
            border-bottom: 1px solid #3e3e42;
        }
    )");

    window.show();
    
    return app.exec();
}
