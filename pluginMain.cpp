#include <maya/MFnPlugin.h>
#include <maya/MFileIO.h>
#include "drpCmd.h"
#include "Discord.h"
#include <stdlib.h>
#include <sstream>
#include <algorithm>


Discord * g_Discord;
MStringArray test;
MString mayaVer = MGlobal::mayaVersion();

void read(bool &value, bool &name, bool &project, bool &time) {

	std::ifstream file("C:/ProgramData/DRPMaya/settings.json");
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
			if (line[0] == '#' || line.empty()) {
				continue;
			}
			auto delimiterPos = line.find("=");
			auto setting = line.substr(0, delimiterPos);

			if (setting == "\"PresenceEnabled\"") {
				auto vl = line.substr(delimiterPos + 1);
				std::istringstream(vl) >> std::boolalpha >> value;
			}
			else if (setting == "\"DisplayFileName\"") {
				auto vl = line.substr(delimiterPos + 1);
				std::istringstream(vl) >> std::boolalpha >> name;
			}
			else if (setting == "\"DisplayProjectName\"") {
				auto vl = line.substr(delimiterPos + 1);
				std::istringstream(vl) >> std::boolalpha >> project;
			}
			else if (setting == "\"DisplayTime\"") {
				auto vl = line.substr(delimiterPos + 1);
				std::istringstream(vl) >> std::boolalpha >> time;
			}
		}
	}
}


void write(const char* value = "true", const char* name = "true", const char* project = "true", const char* time = "true") {
	std::ofstream file;
	file.open("C:/ProgramData/DRPMaya/settings.json");
	file << "\"PresenceEnabled\" = " << value << std::endl;
	file << "\"DisplayFileName\" = " << name << std::endl;
	file << "\"DisplayProjectName\" = " << project << std::endl;
	file << "\"DisplayTime\" = " << time << std::endl;
	file.close();
}

//function for calling the Discord Update
void update(int name,int project,int time) {
	//Check if name and/or project are uninitialized and grab the values from the config if that's the case
	if (name != 0 && name != 1 && project != 0 && project != 1){
		bool value;
		bool name;
		bool project;
		bool time;
		read(value, name, project, time);
	}
	// define the project name
	MString workspace;
	MStatus status = MGlobal::executeCommand(MString("workspace -q -active"), workspace);
	// if the name is supposed to be displayed generate it
	if (name == 1) {
		//grab the current file directory
		MString fileName = MFileIO::currentFile();
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		// split it into parts
		_splitpath_s(fileName.asChar(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		// define the extension and add everything together
		const char* Ext = ".ma";
		const char* state = "Working on ";
		char result[50];
		strcpy_s(result, state);
		strcat_s(result, fname);
		strcat_s(result, Ext);
		//pass the name,project and whether or not they are to be displayed
		g_Discord->Update(result, workspace.asChar(), name, project, time);
	}
	// if the name is not supposed to be displayed just pass NULL
	else {
		g_Discord->Update(NULL, workspace.asChar(), name, project, time);
	}
}

// just calling the initialization
void initialize() {
	const char* ID = NULL;
	if (mayaVer == "2018") {
		ID = "578912672628604948";
	}
	else if (mayaVer == "2019") {
		ID = "585149869098926108";
	}
	const char* version = mayaVer.asChar();
	g_Discord->Initialize(ID);
}
// just calling the Shutdown function
void shutDown() {
	g_Discord->ShutDown();
}

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	// add the command
	MFnPlugin fnPlugin(obj, "Arhas", "1.0", "Any");
	status = fnPlugin.registerCommand("drpEnable",drp::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	//add the menu at the top by using MEL Commands
	MString menu;
	MGlobal::executeCommand("menu -p MayaWindow -l \"Rich Presence\" RP",menu);
	
	bool value;
	bool name;
	bool project;
	bool time;

	//add Callback for 95% of file scenarios to update Discord
	MCallbackId projectChangeCallback = MEventMessage::addEventCallback("workspaceChanged",(MMessage::MBasicFunction)update,(&name,&project,&time));
	//add Callback for when you make a new file to update Discord
	MCallbackId newFileCallback = MSceneMessage::addCallback(MSceneMessage::kAfterNew, (MMessage::MBasicFunction)update, (&name, &project, &time));
	
	if (mayaVer == "2019") {
		MCallbackId openFileCallback = MSceneMessage::addCallback(MSceneMessage::kAfterOpen, (MMessage::MBasicFunction)update, (&name, &project, &time));
	}

	//create the directory and the config and set default values if non-existant
	if (CreateDirectory("C:/ProgramData/DRPMaya", NULL)) {
		const char* t_value = "true";
		const char* t_name = "true";
		const char* t_project = "true";
		const char* t_time = "true";
		write();
		value = 1;
		name = 1;
		project = 1;
		time = 1;

	}

	//if the directory already exists grab the old settings
	else if (ERROR_ALREADY_EXISTS == GetLastError()) {
		read(value, name, project, time);
	}
	
	//add menu items with a checkbox in the right state and have it call the drpEnable command if toggled
	if (value == 1) {
		initialize();
		update(name, project, time);
		MGlobal::executeCommand("menuItem -label Enable -cb 1 -c \"drpEnable\" Enable");
	}

	else {
		MGlobal::executeCommand("menuItem -label Enable -cb 0 -c \"drpEnable\" Enable");
	}

	if (name == 1) {
		MGlobal::executeCommand("menuItem -label \"Show file name\" -cb 1 -c \"drpEnable\"  Name");
	}
	else {
		MGlobal::executeCommand("menuItem -label \"Show file name\" -cb 0 -c \"drpEnable\"  Name");
	}
	if (project == 1) {
		MGlobal::executeCommand("menuItem -label \"Show project name\" -cb 1 -c \"drpEnable\" Project");
	}
	else {
		MGlobal::executeCommand("menuItem -label \"Show project name\" -cb 0 -c \"drpEnable\" Project");
	}
	if (time == 1) {
		MGlobal::executeCommand("menuItem -label \"Show time spent\" -cb 1 -c \"drpEnable\" Time");
	}
	else {
		MGlobal::executeCommand("menuItem -label \"Show time spent\" -cb 0 -c \"drpEnable\" Time");
	}
	return MS::kSuccess;
}
MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	//shut down DRP
	shutDown();

	//remove the command
	MFnPlugin fnPlugin(obj);
	status = fnPlugin.deregisterCommand("drpEnable");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	//remove the menu at the top
	MGlobal::executeCommand("deleteUI RP");

	//close the file if currently open
	std::ifstream file ("C:/ProgramData/DRPMaya/settings.json");
	if (file.is_open()) {
		file.close();
	}
	//delete the file and directory
	remove("C:/ProgramData/DRPMaya/settings.json");
	_rmdir("C:/ProgramData/DRPMaya");

	return MS::kSuccess;
}

MStatus drp::doIt(const MArgList& argList)
{
	MStatus status;
	int value;
	int name;
	int project;
	int time;

	//query the current state of the checkboxes and store them in the according variable
	MGlobal::executeCommand("menuItem -q -cb Enable", value);
	MGlobal::executeCommand("menuItem -q -cb Name", name);
	MGlobal::executeCommand("menuItem -q -cb Project", project);
	MGlobal::executeCommand("menuItem -q -cb Time", time);

	//if the value is being set to off it should shutdown DRP
	if (value == 0) {
		shutDown();
	}

	//if the value is being set to on it should start it and set it to the current values and store the current values in the config
	else {
		initialize();
		update(name,project,time);
		
		const char* f_value;
		const char* f_name;
		const char* f_project;
		const char* f_time;

		if (value == 1) {
			f_value = "true";
		}
		else {
			f_value = "false";
		}
		if (name == 1) {
			f_name = "true";
		}
		else {
			f_name = "false";
		}
		if (project == 1) {
			f_project = "true";
		}
		else {
			f_project = "false";
		}
		if (time == 1) {
			f_time = "true";
		}
		else {
			f_time = "false";
		}

		write(f_value, f_name, f_project, f_time);
	}

	return MS::kSuccess;
}
