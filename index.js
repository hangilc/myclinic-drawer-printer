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

function scaleDrawerOps(ops, scale){
	var scaled = [];
	var n = ops.length;
	for(var i=0;i<n;i++){
		var op = ops[i];
		switch(op[0]){
			case "move_to": {
				var x = op[1];
				var y = op[2];
				scaled.push(["move_to", x * scale, y * scale]);
				break;
			}
			case "line_to": {
				var x = op[1];
				var y = op[2];
				scaled.push(["line_to", x * scale, y * scale]);
				break;
			}
			case "create_font": {
				var name = op[1];
				var font = op[2];
				var size = op[3];
				var weight = op[4];
				var italic = op[5];
				scaled.push(["create_font", name, font, size * scale, weight, italic]);
				break;
			}
			case "draw_chars": {
				var text = op[1];
				var xs = op[2];
				var ys = op[3];
				var scaled_xs, scaled_ys;
				if( xs.length ){
					scaled_xs = xs.map(function(x){ return x * scale; });
				} else {
					scaled_xs = xs * scale;
				}
				if( ys.length ){
					scaled_ys = ys.map(function(y){ return y * scale; });
				} else {
					scaled_ys = ys * scale;
				}
				scaled.push(["draw_chars", text, scaled_xs, scaled_ys]);
				break;
			}
			case "create_pen": {
				var name = op[1];
				var r = op[2];
				var g = op[3];
				var b = op[4];
				var w = op[5];
				scaled.push(["create_pen", name, r, g, b, w * scale]);
				break;
			}
			default: scaled.push(op);
		}
	}
	return scaled;
}

function shiftDrawerOps(ops, dx, dy){
	var shifted = [];
	var n = ops.length;
	for(var i=0;i<n;i++){
		var op = ops[i];
		switch(op[0]){
			case "move_to": {
				var x = op[1];
				var y = op[2];
				shifted.push(["move_to", x + dx, y + dy]);
				break;
			}
			case "line_to": {
				var x = op[1];
				var y = op[2];
				shifted.push(["line_to", x + dx, y + dy]);
				break;
			}
			case "draw_chars": {
				var text = op[1];
				var xs = op[2];
				var ys = op[3];
				var shifted_xs, shifted_ys;
				if( xs.length ){
					shifted_xs = xs.map(function(x){ return x + dx; });
				} else {
					shifted_xs = xs + dx;
				}
				if( ys.length ){
					shifted_ys = ys.map(function(y){ return y + dy; });
				} else {
					shifted_ys = ys + dy;
				}
				shifted.push(["draw_chars", text, shifted_xs, shifted_ys]);
				break;
			}
			default: shifted.push(op);
		}
	}
	return shifted;
}


function modifyOps(ops, aux){
	if( aux.scale ){
		ops = scaleDrawerOps(ops, aux.scale);
	}
	var dx = 0;
	var dy = 0;
	if( aux.dx ){
		dx = +aux.dx;
	}
	if( aux.dy ){
		dy = +aux.dy;
	}
	if( dx != 0 || dy != 0 ){
		ops = shiftDrawerOps(ops, dx, dy);
	}
	return ops;
}

exports.printPages = function(pages, setting){
	if( setting.aux ){
		pages = pages.map(function(ops){ return modifyOps(ops, setting.aux); });
	}
	var hdc = api.createDc(setting.devmode, setting.devnames);
	if( hdc === 0 ){
		return "cannot create hdc";
	}
	try{
		var printer = new Printer(hdc);
		printer.print(pages);
		printer.dispose();
		api.deleteDc(hdc);
		return null;
	} catch(ex){
		api.deleteDc(hdc);
		console.log(ex);
		return ex;
	}
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

