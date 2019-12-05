#include "main_window.hpp"
#include "ui_main_window.h"
#include "tree_model.hpp"

#include <QTreeView>

#include <QItemSelectionModel>
#include <QDebug>
#define DEB qDebug()

MainWindow::MainWindow (QWidget* parent)
    : QMainWindow{parent}
    , ui_{new Ui::MainWindow}
    , tree_model_{new TreeModel { 2, this }} {
    ui_->setupUi(this);
    ui_->tree_view_->setModel(tree_model_);
    ui_->tree_view_->expandAll();
    connect(ui_->insert_row_button, &QPushButton::clicked, this, &MainWindow::insertRow);
    connect(ui_->insert_child_button, &QPushButton::clicked, this, &MainWindow::insertChild);
// connect(ui_->tree_view_->selectionModel(), &QItemSelectionModel::selectionChanged,
// this, &MainWindow::onSelectionChanged);
}

MainWindow::~MainWindow () {
    delete ui_;
}

void MainWindow::insertRow () {
    auto index = ui_->tree_view_->selectionModel()->currentIndex();
    tree_model_->insertRows(index.row() + 1, 1, tree_model_->parent(index));
}

void MainWindow::insertChild () {
    auto index = ui_->tree_view_->selectionModel()->currentIndex();
    int  row   = tree_model_->rowCount(index);
    tree_model_->insertRows(row, 1, index);
    ui_->tree_view_->expandAll();
}

void MainWindow::onSelectionChanged (QItemSelection selected, QItemSelection deselected) {
    auto index = ui_->tree_view_->currentIndex();
    DEB << index << index.data();
    int rows = tree_model_->rowCount(index);
    for (int i = 0; i < rows; ++i) {
        auto child = tree_model_->index(i, 0, index);
        DEB << "   " << i << child.data();
    }
    DEB << " ----- ";
}
