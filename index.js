"use strict";

var api = require("bindings")("drawer");
var Printer = require("./printer");
var DrawerSetting = require("./setting");

exports.api = api;

/*
api.createWindow() ==> hwnd
api.disposeWindow(hwnd) ==> bool (ok)
api.getDc(hwnd) ==> hdc
api.releaseDc(hwnd, hdc) ==> bool (ok)
api.measureText(hdc, string) => { cx:..., cy:... }
api.createFont(fontname, size, weight?, italic?) ==> HANDLE
api.deleteObject(obj) ==> bool (ok)
api.getDpiOfHdc(hdc) ==> { dpix:..., dpiy:... }
api.printerDialog(devmode?, devnames?) ==> { devmode:..., devnames:... }
api.parseDevmode(devmode) ==> { deviceName:..., ... }
api.parseDevnames(devnames) ==> { driver:..., ... }
api.createDc(devmode, devnames) ==> hdc
api.deleteDc(hdc) ==> bool (ok)
api.beginPrint(hdc) ==> (throws exception if it fails)
api.endPrint(hdc) ==> (throws exception if it fails)
api.abortPrint(hdc) ==> (throws exception if it fails)
api.startPage(hdc) ==> (throws exception if it fails)
api.endPage(hdc) ==> (throws exception if it fails)
api.moveTo(hdc, x, y) ==> (throws exception if it fails)
api.lineTo(hdc, x, y) ==> (throws exception if it fails)
api.textOut(hdc, x, y, text) ==> (throws exception if it fails)
api.selectObject(hdc, handle) ==> (throws exception if it fails)
api.setTextColor(hdc, r, g, b) ==> (throws exception if it fails)
api.createPen(width, r, g, b) ==> (throws exception if it fails)
api.setBkMode(hdc, mode) ==> (throws exception if it fails)
*/

exports.printPages = function(pages, setting){
	var hdc = api.createDc(setting.devmode, setting.devnames);
	var printer = new Printer(hdc);
	printer.print(pages);
	printer.dispose();
	api.deleteDc(hdc);
};

exports.setSettingDir = function(path){
	DrawerSetting.setSettingDir(path);
};

exports.printerDialog = function(optDefaultSetting){
	if( optDefaultSetting ){
		return api.printerDialog(optDefaultSetting.devmode, optDefaultSetting.devnames);
	} else{
		return api.printerDialog();
	}
};

exports.settingExists = function(name, cb){
	DrawerSetting.settingExists(name, cb);
};

exports.listSettings = function(cb){
	DrawerSetting.listSettings(cb);
};

exports.saveSetting = function(name, setting, done){
	DrawerSetting.saveSetting(name, setting, done);
};

exports.readSetting = function(name, cb){
	DrawerSetting.readSetting(name, cb);
};

exports.parseSetting = function(setting){
	return DrawerSetting.parseSetting(setting);
};

exports.deleteSetting = function(name, done){
	DrawerSetting.deleteSetting(name, done);
};

