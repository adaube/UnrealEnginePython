
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySWidget.h"

static PyObject *ue_PySWidget_str(ue_PySWidget *self) {
	return PyUnicode_FromFormat("<unreal_engine.SWidget '%p' (%s)>",
		self->s_widget, TCHAR_TO_UTF8(*self->s_widget->GetTypeAsString()));
}

static PyObject *py_ue_swidget_get_children(ue_PySWidget *self, PyObject * args) {
	FChildren *children = self->s_widget->GetChildren();
	PyObject *py_list = PyList_New(0);
	for (int32 i = 0; i < children->Num(); i++) {
		TSharedRef<SWidget> widget = children->GetChildAt(i);
		PyObject *item = (PyObject *)ue_py_get_swidget(widget);
		PyList_Append(py_list, item);
		Py_DECREF(item);
	}
	return py_list;
}

static PyObject *py_ue_swidget_set_tooltip_text(ue_PySWidget *self, PyObject * args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_tooltip_text", &text)) {
		return NULL;
	}

	self->s_widget->SetToolTipText(FText::FromString(UTF8_TO_TCHAR(text)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_set_cursor(ue_PySWidget *self, PyObject * args) {
	int cursor;
	if (!PyArg_ParseTuple(args, "i:set_cursor", &cursor)) {
		return NULL;
	}

	self->s_widget->SetCursor((EMouseCursor::Type)cursor);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_set_enabled(ue_PySWidget *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_enabled", &py_bool)) {
		return NULL;
	}

	self->s_widget->SetEnabled(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_button_down(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_button_down", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseButtonDown(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_button_up(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_button_up", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseButtonUp(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_double_click(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_double_click", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseDoubleClick(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_move(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_move", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseMove(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}



static PyObject *py_ue_swidget_has_keyboard_focus(ue_PySWidget *self, PyObject * args) {

	if (self->s_widget->HasKeyboardFocus()) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_swidget_get_type(ue_PySWidget *self, PyObject * args) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->s_widget->GetTypeAsString()));
}



static PyMethodDef ue_PySWidget_methods[] = {
	{ "get_children", (PyCFunction)py_ue_swidget_get_children, METH_VARARGS, "" },
	{ "get_type", (PyCFunction)py_ue_swidget_get_type, METH_VARARGS, "" },
	{ "set_tooltip_text", (PyCFunction)py_ue_swidget_set_tooltip_text, METH_VARARGS, "" },
	{ "set_cursor", (PyCFunction)py_ue_swidget_set_cursor, METH_VARARGS, "" },
	{ "set_enabled", (PyCFunction)py_ue_swidget_set_enabled, METH_VARARGS, "" },
	{ "has_keyboard_focus", (PyCFunction)py_ue_swidget_has_keyboard_focus, METH_VARARGS, "" },
	{ "bind_on_mouse_button_down", (PyCFunction)py_ue_swidget_bind_on_mouse_button_down, METH_VARARGS, "" },
	{ "bind_on_mouse_button_up", (PyCFunction)py_ue_swidget_bind_on_mouse_button_down, METH_VARARGS, "" },
	{ "bind_on_mouse_double_click", (PyCFunction)py_ue_swidget_bind_on_mouse_double_click, METH_VARARGS, "" },
	{ "bind_on_mouse_move", (PyCFunction)py_ue_swidget_bind_on_mouse_move, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySWidgetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SWidget", /* tp_name */
	sizeof(ue_PySWidget), /* tp_basicsize */
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
	(reprfunc)ue_PySWidget_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SWidget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySWidget_methods,             /* tp_methods */
};

void ue_python_init_swidget(PyObject *ue_module) {
	ue_PySWidgetType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PySWidgetType) < 0)
		return;

	Py_INCREF(&ue_PySWidgetType);
	PyModule_AddObject(ue_module, "SWidget", (PyObject *)&ue_PySWidgetType);
}

ue_PySWidget *py_ue_is_swidget(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySWidgetType))
		return nullptr;
	return (ue_PySWidget *)obj;
}

