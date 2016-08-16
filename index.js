"use strict";

var api = require("bindings")("drawer");
var Printer = require("./printer");

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

exports.printPages = function(pages, optSetting){
	var hdc;
	if( !optSetting ){
		var setting = api.printerDialog();
		if( !setting ){
			return;
		}
		hdc = api.createDc(setting.devmode, setting.devnames);
	}
	var printer = new Printer(hdc);
	printer.print(pages);
	printer.dispose();
	api.deleteDc(hdc);
}