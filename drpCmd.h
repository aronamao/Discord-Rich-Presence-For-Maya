#pragma once
#include <maya/MPxCommand.h>
#include <maya/MSceneMessage.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MEventMessage.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MItDag.h>
#include <fstream>
#include <iostream>
#include <direct.h>

//protoype everything maya related
class drp : public MPxCommand {
public:
	drp();
	virtual ~drp();
	virtual MStatus doIt(const MArgList& argList);
	static void* creator();

private:
	MCallbackId m_callbackId;
};

