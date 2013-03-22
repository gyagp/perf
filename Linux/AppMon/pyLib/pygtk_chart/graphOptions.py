#!/usr/bin/env python
#
#       graph_options.py
#       
#       Copyright 2009 Sven Festersen <sven@sven-laptop>
#       
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.
import gtk
import pygtk
import gobject
from pygtk_chart import line_chart

def to_gdkColor(r, g, b):
    return gtk.gdk.Color(int(65535 * r), int(65535 * g), int(65535 * b))
    
def from_gdkColor(c):
    return (c.red / 65535.0, c.green / 65535.0, c.blue / 65535.0)

class Summary():
    def setProcName(self, procName):
        self.procName = procName
    def setCPU(self, cpu, cpuProc):
        self.cpuSys = cpu
        self.cpuProc = cpuProc

    def setMEM(self, mem, memPure, memProc):
        self.mem = mem
        self.memPure = memPure
        self.memProc = memProc

    def setWU(self, wakeup):
        self.wakeup = wakeup

    def setCState(self, cAvg=[]):
        self.cAvg = cAvg

class GraphControl(gtk.Table):
    
    def __init__(self, summary, type, graphs=[]):
        gtk.Table.__init__(self, 14, 2)
        self.set_row_spacings(2)
        self.set_col_spacings(6)
        self.set_border_width(6)
        self.graphs = graphs
        self.summary = summary
        self.type = type
        self.selected = None
        self._init_combo()
        self._init_draw_options()
        self._init_show_values()
        self._init_type()
        self._load_from_selected()
        self._init_summary()
    def _init_summary(self):
        self.attach(gtk.HSeparator(), 0, 2, 9, 10, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        sumLabel = gtk.Label("DATA Summary")
        self.attach(sumLabel, 0, 2, 11, 12, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK) 
        sum = self.summary
        if (self.type == "CPU"):
            sysCPU0 = "System average: %s" % (sum.cpuSys)
            labelSysCPU0 = gtk.Label(sysCPU0)
            labelSysCPU0.set_alignment(0.0, 0.5)
            self.attach(labelSysCPU0, 0, 2, 13, 14, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
            self.attach(gtk.HSeparator(), 0, 1, 15, 16, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
            labelProcCPU = gtk.Label("Process average:")
            labelProcCPU.set_alignment(0.0, 0.5)
            self.attach(labelProcCPU, 0, 2, 17, 18, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
            for i in range(0, len(sum.cpuProc)):
                labelCPU = gtk.Label(sum.procName[i] + " = " + str(sum.cpuProc[i]))
                labelCPU.set_alignment(0.0, 0.5)
                self.attach(labelCPU, 0, 2, 19+2*i, 20+2*i, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        elif(self.type == "MEM"):
            labelProcMEM = gtk.Label("Process average:")
            labelProcMEM.set_alignment(0.0, 0.5)
            self.attach(labelProcMEM, 0, 2, 13, 14, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
            for i in range(0, len(sum.memProc)):
                labelMEM = gtk.Label(sum.procName[i] + " = " + str(sum.memProc[i]))
                labelMEM.set_alignment(0.0, 0.5)
                self.attach(labelMEM, 0, 2, 15+2*i, 16+2*i, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        elif(self.type == "WU"):
            labelWU0 = gtk.Label("System average: %s" % (sum.wakeup))    
            labelWU0.set_alignment(0.0, 0.5)
            self.attach(labelWU0, 0, 2, 13, 14, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        elif(self.type == "CC"):
            labelCC0 = gtk.Label("System average:")    
            labelCC0.set_alignment(0.0, 0.5)
            self.attach(labelCC0, 0, 2, 13, 14, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
            for i in range(0, len(sum.cAvg)):
                cStr = "C%d = %s" % (i, sum.cAvg[i])
                labelC = gtk.Label(cStr)
                labelC.set_alignment(0.0, 0.5)
                self.attach(labelC, 0, 2, 15+2*i, 16+2*i, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        elif(self.type == "MEMA"):
            labelMEMA0 = gtk.Label("System average: %s" % (sum.memPure))
            labelMEMA0.set_alignment(0.0, 0.5)
            self.attach(labelMEMA0, 0, 2, 13, 14, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
                
    def _init_combo(self):
        store = gtk.ListStore(gobject.TYPE_PYOBJECT, str)
        self.graph_combo = gtk.ComboBox(store)
        cell = gtk.CellRendererText()
        self.graph_combo.pack_start(cell, True)
        self.graph_combo.add_attribute(cell, 'text', 1)
        
        
        for graph in self.graphs:
            store.set(store.append(None), 0, graph, 1, graph.get_title())
        self.graph_combo.set_active(0)
        self.graph_combo.connect("changed", self._cb_combo)
        label = gtk.Label("Select graph:")
        label.set_alignment(0.0, 0.5)
        self.attach(label, 0, 1, 0, 1, xoptions=gtk.FILL, yoptions=gtk.SHRINK)
        self.attach(self.graph_combo, 1, 2, 0, 1, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        self.attach(gtk.HSeparator(), 0, 2, 1, 2, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        self._cb_combo(self.graph_combo)
        
    def _init_draw_options(self):
        self.checkbox_visible = gtk.CheckButton("Visible")
        self.checkbox_visible.connect("toggled", self._cb_visible_toggled)
        self.attach(self.checkbox_visible, 0, 2, 2, 3, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        
    def _init_type(self):
        self.combo_type = gtk.combo_box_new_text()
        self.combo_type.append_text("Points")
        self.combo_type.append_text("Lines")
        self.combo_type.append_text("Lines & Points")
        self.combo_type.connect("changed", self._cb_graph_type_changed)
        label = gtk.Label("Graph type:")
        label.set_alignment(0.0, 0.5)
        self.attach(label, 0, 1, 7, 8, xoptions=gtk.FILL, yoptions=gtk.SHRINK)
        self.attach(self.combo_type, 1, 2, 7, 8, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.SHRINK)
        
    def _init_show_values(self):
        self.checkbox_show_values = gtk.CheckButton("Show y values at datapoints.")
        self.checkbox_show_values.connect("toggled", self._cb_show_values_toggled)
        self.attach(self.checkbox_show_values, 0, 2, 5, 6, xoptions=gtk.FILL|gtk.EXPAND, yoptions=gtk.SHRINK)
        
    def _load_from_selected(self):
        try:
            self.checkbox_visible.set_active(self.selected.get_visible())
            self.checkbox_show_values.set_active(self.selected.get_show_values())
            self.combo_type.set_active(self.selected.get_type() - 1)
                
            store = gtk.ListStore(gobject.TYPE_PYOBJECT, str)
            i = 0
            active = 0
            for graph in self.graphs:
                if graph == self.selected: continue
                store.set(store.append(None), 0, graph, 1, graph.get_title())
                if graph == fill_to:
                    active = i
                i += 1
            self.combo_fill_graph.set_model(store)
            self.combo_fill_graph.set_active(active)
            
            self.fill_color_chooser.set_color(to_gdkColor(*self.selected.get_color()))
        except:
            pass
        
    def _cb_combo(self, widget):
        iter = widget.get_active_iter()
        model = widget.get_model()
        graph = model.get_value(iter, 0)
        self.selected = graph 
        print "Selected", graph.get_property("name")
        self._load_from_selected()
        
    def _cb_visible_toggled(self, button):
        self.selected.set_visible(button.get_active())
        
    def _cb_graph_type_changed(self, combo):
        self.selected.set_type(combo.get_active() + 1)
        
    def _cb_show_values_toggled(self, button):
        self.selected.set_show_values(button.get_active())
