
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#endif

#include "Runtime/Slate/Public/Framework/Commands/UICommandList.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"
#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"
#include "Runtime/Slate/Public/Widgets/Views/STableRow.h"


#include "UEPySlate.h"

FReply UPythonSlateDelegate::OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", py_ue_new_uscriptstruct(FGeometry::StaticStruct(), (uint8 *)&geometry), py_ue_new_uscriptstruct(FPointerEvent::StaticStruct(), (uint8 *)&pointer_event));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}

	if (ret == Py_False) {
		Py_DECREF(ret);
		return FReply::Unhandled();
	}
	Py_DECREF(ret);
	return FReply::Handled();
}

FReply UPythonSlateDelegate::OnClicked() {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}

	if (ret == Py_False) {
		Py_DECREF(ret);
		return FReply::Unhandled();
	}
	Py_DECREF(ret);
	return FReply::Handled();
}

#if WITH_EDITOR
void UPythonSlateDelegate::OnAssetDoubleClicked(const FAssetData& AssetData) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_fassetdata((FAssetData *)&AssetData));
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void UPythonSlateDelegate::OnAssetSelected(const FAssetData& AssetData) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_fassetdata((FAssetData *)&AssetData));
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

TSharedPtr<SWidget> UPythonSlateDelegate::OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets) {
	FScopePythonGIL gil;

	PyObject *py_list = PyList_New(0);
	for (FAssetData asset : SelectedAssets) {
		PyList_Append(py_list, py_ue_new_fassetdata(&asset));
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_list);
	Py_DECREF(py_list);
	if (!ret) {
		unreal_engine_py_log_error();
		return nullptr;
	}

	ue_PySWidget *s_widget = py_ue_is_swidget(ret);
	if (!s_widget) {
		Py_DECREF(ret);
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return nullptr;
	}
	TSharedPtr<SWidget> value = s_widget->s_widget_owned;
	Py_DECREF(ret);
	return value;
}
#endif

void UPythonSlateDelegate::SimpleExecuteAction() {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void UPythonSlateDelegate::ExecuteAction(PyObject *py_obj) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_obj);
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

TSharedRef<SDockTab> UPythonSlateDelegate::SpawnPythonTab(const FSpawnTabArgs &args) {
	TSharedRef<SDockTab> dock_tab = SNew(SDockTab).TabRole(ETabRole::NomadTab);
	PyObject *py_dock = (PyObject *)ue_py_get_swidget(dock_tab);
	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_dock);
	if (!ret) {
		unreal_engine_py_log_error();
	}
	else {
		Py_DECREF(ret);
	}
	return dock_tab;
}

TSharedRef<ITableRow> UPythonSlateDelegate::GenerateRow(TSharedPtr<FPythonItem> InItem, const TSharedRef<STableViewBase>& OwnerTable) {
	PyObject *ret = PyObject_CallFunction(py_callable, (char*)"O", InItem.Get()->py_object);
	if (!ret) {
		unreal_engine_py_log_error();
		return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable);
	}
	ue_PySWidget *s_widget = py_ue_is_swidget(ret);
	if (!s_widget) {
		UE_LOG(LogPython, Error, TEXT("python callable did not return a SDockTab object"));
		return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable);
	}

	return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable).Content()[s_widget->s_widget_owned];
}

void UPythonSlateDelegate::GetChildren(TSharedPtr<FPythonItem> InItem, TArray<TSharedPtr<FPythonItem>>& OutChildren) {
	PyObject *ret = PyObject_CallFunction(py_callable, (char*)"O", InItem.Get()->py_object);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	PyObject *py_iterable = PyObject_GetIter(ret);
	if (!py_iterable || !PyIter_Check(py_iterable)) {
		UE_LOG(LogPython, Error, TEXT("returned value is not iterable"));
		Py_XDECREF(py_iterable);
		Py_DECREF(ret);
	}

	while (PyObject *item = PyIter_Next(py_iterable)) {
		Py_INCREF(item);
		OutChildren.Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}
	Py_DECREF(py_iterable);
	Py_DECREF(ret);
}

static std::map<SWidget *, ue_PySWidget *> *py_slate_mapping;

ue_PySWidget *ue_py_get_swidget(TSharedPtr<SWidget> s_widget) {
	ue_PySWidget *ret = nullptr;
	auto it = py_slate_mapping->find(s_widget.Get());
	// not found, it means it is an SWidget not generated from python
	if (it == py_slate_mapping->end()) {
		if (s_widget->GetType() == FName("SWindow")) {
			ret = py_ue_new_swidget<ue_PySWindow>(s_widget.Get(), &ue_PySWindowType);
		}
		if (s_widget->GetType() == FName("SDockTab")) {
			ret = py_ue_new_swidget<ue_PySDockTab>(s_widget.Get(), &ue_PySDockTabType);
		}
		else {
			ret = py_ue_new_swidget<ue_PySWidget>(s_widget.Get(), &ue_PySWidgetType);
		}
	}
	else {
		ret = it->second;
	}
	Py_INCREF(ret);
	return ret;
}

void ue_py_register_swidget(SWidget *s_widget, ue_PySWidget *py_s_widget) {
	(*py_slate_mapping)[s_widget] = py_s_widget;
}

void ue_python_init_slate(PyObject *module) {

	py_slate_mapping = new std::map<SWidget *, ue_PySWidget *>();

	ue_python_init_swidget(module);
	ue_python_init_scompound_widget(module);
	ue_python_init_swindow(module);
	ue_python_init_sborder(module);
	ue_python_init_sbutton(module);
	ue_python_init_sleaf_widget(module);
	ue_python_init_stext_block(module);
	ue_python_init_seditable_text_box(module);
	ue_python_init_smulti_line_editable_text(module);
	ue_python_init_spanel(module);
	ue_python_init_sgrid_panel(module);
	ue_python_init_sbox_panel(module);
	ue_python_init_shorizontal_box(module);
	ue_python_init_svertical_box(module);
	ue_python_init_sviewport(module);
	ue_python_init_simage(module);
	ue_python_init_sdock_tab(module);
	ue_python_init_stable_view_base(module);
	ue_python_init_slist_view(module);
	ue_python_init_spython_list_view(module);
	ue_python_init_stree_view(module);
	ue_python_init_spython_tree_view(module);
	ue_python_init_ssplitter(module);
	ue_python_init_sheader_row(module);


#if WITH_EDITOR
	ue_python_init_seditor_viewport(module);
	ue_python_init_spython_editor_viewport(module);
	ue_python_init_sgraph_editor(module);
	ue_python_init_spython_shelf(module);
#endif

	ue_python_init_ftab_spawner_entry(module);
	ue_python_init_fmenu_builder(module);
}

PyObject *py_unreal_engine_get_editor_window(PyObject *self, PyObject *args) {

	if (!FGlobalTabmanager::Get()->GetRootWindow().IsValid()) {
		return PyErr_Format(PyExc_Exception, "no RootWindow found");
	}

	return (PyObject *)ue_py_get_swidget(FGlobalTabmanager::Get()->GetRootWindow());
}

// TODO: better understand the extender system
class FPythonSlateCommands : public TCommands<FPythonSlateCommands>
{
public:

	FPythonSlateCommands()
		: TCommands<FPythonSlateCommands>(TEXT("UnrealEnginePython"), NSLOCTEXT("Contexts", "UnrealEnginePython", "UnrealEnginePython"), NAME_None, "EditorStyle")
	{
		UE_LOG(LogPython, Warning, TEXT("BULDING COMMAND"));

	}

	void Setup(char *command_name, PyObject *py_object) {
		py_callable = py_object;
		Py_INCREF(py_callable);

		name = FString(command_name);
	}

	// TCommands<> interface
	virtual void RegisterCommands() override {
		UE_LOG(LogPython, Warning, TEXT("REGISTERING COMMAND"));
		commands = MakeShareable(new FUICommandList);

		extender = MakeShareable(new FExtender());

		UI_COMMAND_Function(this, command, nullptr, *name, *name, TCHAR_TO_UTF8(*name), *name, *name, EUserInterfaceActionType::Button, FInputGesture());
		commands->MapAction(command, FExecuteAction::CreateRaw(this, &FPythonSlateCommands::Callback), FCanExecuteAction());

		extender->AddMenuExtension("WindowLayout", EExtensionHook::After, commands, FMenuExtensionDelegate::CreateRaw(this, &FPythonSlateCommands::Builder));

#if WITH_EDITOR
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(extender);
#endif
	}

	void Callback() {
		FScopePythonGIL gil;
		PyObject *ret = PyObject_CallObject(py_callable, nullptr);
		if (!ret) {
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	void Builder(FMenuBuilder &Builder) {
		UE_LOG(LogPython, Warning, TEXT("BULDING MENU %s"), *command->GetCommandName().ToString());
		Builder.AddMenuEntry(command);
	}

	TSharedPtr<FExtender> GetExtender() {
		return extender;
	}

	TSharedPtr<FUICommandList> GetCommands() {
		return commands;
	}
private:
	TSharedPtr<FUICommandList> commands;
	TSharedPtr<FUICommandInfo> command;
	TSharedPtr<FExtender> extender;
	PyObject *py_callable;

	FString name;
};

PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args) {

	char *command_name;
	PyObject *py_callable;
	int interface_type = EUserInterfaceActionType::Button;

	char *menu_bar = nullptr;

	if (!PyArg_ParseTuple(args, "sO|s:add_menu_extension", &command_name, &py_callable, &menu_bar)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	TSharedRef<FPythonSlateCommands> commands = MakeShareable(new FPythonSlateCommands());

	commands->Setup(command_name, py_callable);

	commands->RegisterCommands();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_register_nomad_tab_spawner(PyObject * self, PyObject * args) {

	char *name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:register_nomad_tab_spawner", &name, &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	FOnSpawnTab spawn_tab;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	spawn_tab.BindUObject(py_delegate, &UPythonSlateDelegate::SpawnPythonTab);

	FTabSpawnerEntry *spawner_entry = &FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UTF8_TO_TCHAR(name), spawn_tab)
		// TODO: more generic way to set teh group
#if WITH_EDITOR
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
#endif
		;

	PyObject *ret = py_ue_new_ftab_spawner_entry(spawner_entry);
	Py_INCREF(ret);
	return ret;
}

PyObject *py_unreal_engine_unregister_nomad_tab_spawner(PyObject * self, PyObject * args) {

	char *name;
	if (!PyArg_ParseTuple(args, "s:unregister_nomad_tab_spawner", &name)) {
		return NULL;
	}

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UTF8_TO_TCHAR(name));

	Py_INCREF(Py_None);
	return Py_None;
}
