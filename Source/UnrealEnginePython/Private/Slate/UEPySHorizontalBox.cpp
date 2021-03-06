
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySHorizontalBox.h"


#define GET_s_horizontal_box SHorizontalBox *s_horizontal_box = (SHorizontalBox *)self->s_box_panel.s_panel.s_widget.s_widget


static PyObject *ue_PySHorizontalBox_str(ue_PySHorizontalBox *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SHorizontalBox '%p'>",
		self->s_box_panel.s_panel.s_widget.s_widget);
}

static PyObject *py_ue_shorizontal_box_add_slot(ue_PySHorizontalBox *self, PyObject * args, PyObject *kwargs) {
	PyObject *py_content;
	int h_align = 0;
	float max_width = 0;
	float padding = 0;
	int v_align = 0;
	float fill_width = 0;
	PyObject *py_auto_width = nullptr;

	char *kwlist[] = { (char *)"widget",
		(char *)"h_align",
		(char *)"max_width",
		(char *)"padding",
		(char *)"v_align",
		(char *)"fill_width",
		(char *)"auto_width",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iffifO:add_slot", kwlist,
		&py_content,
		&h_align,
		&max_width,
		&padding,
		&v_align,
		&fill_width,
		&py_auto_width)) {
		return NULL;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget) {
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	// TODO: decrement reference when destroying parent
	Py_INCREF(py_swidget);

	GET_s_horizontal_box;

	SHorizontalBox::FSlot &fslot = s_horizontal_box->AddSlot();
	fslot.AttachWidget(py_swidget->s_widget->AsShared());
	fslot.HAlign((EHorizontalAlignment)h_align);
	if (max_width != 0)
		fslot.MaxWidth(max_width);
	if (fill_width)
		fslot.FillWidth(fill_width);
	fslot.Padding(padding);
	fslot.VAlign((EVerticalAlignment)v_align);
	if (py_auto_width && PyObject_IsTrue(py_auto_width))
		fslot.AutoWidth();

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_shorizontal_box_num_slots(ue_PySHorizontalBox *self, PyObject * args) {
	GET_s_horizontal_box;
	return PyLong_FromLong(s_horizontal_box->NumSlots());
}

static PyMethodDef ue_PySHorizontalBox_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_shorizontal_box_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ "num_slots", (PyCFunction)py_ue_shorizontal_box_num_slots, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySHorizontalBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SHorizontalBox", /* tp_name */
	sizeof(ue_PySHorizontalBox), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	(reprfunc)ue_PySHorizontalBox_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SHorizontalBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySHorizontalBox_methods,             /* tp_methods */
};

static int ue_py_shorizontal_box_init(ue_PySHorizontalBox *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SHorizontalBox, s_box_panel.s_panel.s_widget);
	return 0;
}


void ue_python_init_shorizontal_box(PyObject *ue_module) {
	ue_PySHorizontalBoxType.tp_new = PyType_GenericNew;

	ue_PySHorizontalBoxType.tp_init = (initproc)ue_py_shorizontal_box_init;

	ue_PySHorizontalBoxType.tp_base = &ue_PySBoxPanelType;

	if (PyType_Ready(&ue_PySHorizontalBoxType) < 0)
		return;

	Py_INCREF(&ue_PySHorizontalBoxType);
	PyModule_AddObject(ue_module, "SHorizontalBox", (PyObject *)&ue_PySHorizontalBoxType);
}
