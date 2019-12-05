#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <QItemSelection>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QTreeView;
class TreeModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow (QWidget* parent = nullptr);
    ~MainWindow ();

private slots:
    void insertRow ();
    void insertChild ();
    void onSelectionChanged (QItemSelection selected, QItemSelection deselected);

private:
    // data
    Ui::MainWindow* ui_;
    TreeModel*      tree_model_;
};
#endif // MAINWINDOW_HPP
