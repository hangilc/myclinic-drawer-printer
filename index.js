"use strict";

var drawer = require("bindings")("drawer");

/*
drawer.createWindow() ==> hwnd
drawer.disposeWindow(hwnd) ==> bool (ok)
drawer.getDc(hwnd) ==> hdc
drawer.releaseDc(hwnd, hdc) ==> bool (ok)
drawer.measureText(hdc, string) => { cx:..., cy:... }
drawer.createFont(fontname, size, weight?, italic?) ==> HANDLE
drawer.deleteObject(obj) ==> bool (ok)
drawer.getDpiOfHdc(hdc) ==> { dpix:..., dpiy:... }
drawer.printerDialog(devmode?, devnames?) ==> { devmode:..., devnames:... }
drawer.parseDevmode(devmode) ==> { deviceName:..., ... }
drawer.parseDevnames(devnames) ==> { driver:..., ... }
drawer.createDc(devmode, devnames) ==> hdc
drawer.deleteDc(hdc) ==> bool (ok)
drawer.beginPrint(hdc) ==> (throws exception if it fails)
drawer.endPrint(hdc) ==> (throws exception if it fails)
drawer.abortPrint(hdc) ==> (throws exception if it fails)
drawer.startPage(hdc) ==> (throws exception if it fails)
drawer.endPage(hdc) ==> (throws exception if it fails)
drawer.moveTo(hdc, x, y) ==> (throws exception if it fails)
drawer.lineTo(hdc, x, y) ==> (throws exception if it fails)
drawer.textOut(hdc, x, y, text) ==> (throws exception if it fails)
drawer.selectObject(hdc, handle) ==> (throws exception if it fails)
drawer.setTextColor(hdc, r, g, b) ==> (throws exception if it fails)
drawer.createPen(width, r, g, b) ==> (throws exception if it fails)
drawer.setBkMode(hdc, mode) ==> (throws exception if it fails)
*/


