# -*- coding: utf-8 -*-

"""
Module implementing SAK.
"""

from PyQt4 import QtGui
from PyQt4.QtGui import QApplication, QFont, QStandardItemModel, QTableView, QTreeView, QMainWindow, QStandardItem, QHeaderView
from PyQt4.QtCore import Qt, QVariant, pyqtSignature
import sys
import os
import types
from Ui_sak import Ui_MainWindow
import subprocess


class SAK(QMainWindow, Ui_MainWindow):
    """
    Class documentation goes here.
    """
    def __init__(self, parent = None):
        """
        Constructor
        """
        QMainWindow.__init__(self, parent)
        self.setupUi(self)
        
        self.init_cmd_table()

    def init_cmd_table(self):
        model = QStandardItemModel()
        model.setColumnCount(3)
        model.setHeaderData(0, Qt.Horizontal, "Command")
        model.setHeaderData(1, Qt.Horizontal, "Tag")
        model.setHeaderData(2, Qt.Horizontal, "Description")

        import json
        print sys.path[0]
        os.chdir(sys.path[0])
        f = file('command.txt')
        cmds = json.load(f)
        f.close()
        
        for cmd in cmds:
            l = []
            for field in cmd:
                if type(field) is types.ListType:
                    f = '\n'.join(field)
                else:
                    f = field
                l.append(QStandardItem(f))
            model.appendRow(l)
        self.tableView.setModel(model)   
        self.tableView.resizeRowsToContents()
        
        self.model = model

    @pyqtSignature("QModelIndex")
    def on_tableView_doubleClicked(self, index):
        """
        Slot documentation goes here.
        """
        cmd = str(self.model.item(index.row(), 0).text().toAscii())
        res = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)
        lines = res.stdout.readlines()
        line = "\n".join(lines)
        self.textEdit_result.setText(line)

        
if __name__=="__main__":
    app = QtGui.QApplication(sys.argv)
    sak = SAK()
    sak.show()
    sys.exit(app.exec_())
    

