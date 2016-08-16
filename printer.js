"use strict";

var drawer = require("bindings")("drawer");

function mmToPixel(dpi, mm){
	var inch = mm/25.4;
	return Math.floor(dpi * inch);
};

function DrawerPrinter(hdc){
	var dpi;
	this.hdc = hdc;
	dpi = drawer.getDpiOfHdc(hdc);
	this.dpix = dpi.dpix;
	this.dpiy = dpi.dpiy;
	this.fontDict = {};
	this.penDict = {};
	this.debug = false;
    this.dx = 0;
    this.dy = 0;
	drawer.setBkMode(hdc, drawer.bkModeTransparent);
}

module.exports = DrawerPrinter;

DrawerPrinter.prototype.dispose = function(){
	var name, ok;
	for(name in this.fontDict){
		ok = drawer.deleteObject(this.fontDict[name]);
		if( !ok ){
			console.log("deleteObject failed (font)", name);
		}
	}	
	for(name in this.penDict){
		ok = drawer.deleteObject(this.penDict[name]);
		if( !ok ){
			console.log("deleteObject failed (pen)", name);
		}
	}	
};

DrawerPrinter.prototype.print = function(pages){
	var i, n = pages.length, page;
	drawer.beginPrint(this.hdc);
	for(i=0;i<n;i++){
		page = pages[i];
		this.printPage(page);
	}
	drawer.endPrint(this.hdc);
}

DrawerPrinter.prototype.printPage = function(ops){
	var i, n = ops.length, op;
	drawer.startPage(this.hdc);
	for(i=0;i<n;i++){
		op = ops[i];
		this.dispatch(op);
	}
	drawer.endPage(this.hdc);
}

DrawerPrinter.prototype.dispatch = function(op){
	switch(op[0]){
		case "move_to": this.moveTo(op); break;
		case "line_to": this.lineTo(op); break;
		case "create_font": this.createFont(op); break;
		case "set_font": this.setFont(op); break;
		case "set_text_color": this.setTextColor(op); break;
		case "create_pen": this.createPen(op); break;
		case "set_pen": this.setPen(op); break;
		case "draw_chars": this.drawChars(op); break;
		default: console.log("unknonw op code:", op[0]); break;
	}
};

DrawerPrinter.prototype.moveTo = function(op){
	var mmX = Number(op[1]) + this.dx;
	var mmY = Number(op[2]) + this.dy;
	if( isNaN(mmX) || isNaN(mmY) ){
		console.log("moveTo failed because of bad arg:", op[1], op[2]);
		throw new Error("invalid number to moveTo");
	}
	var x = mmToPixel(this.dpix, mmX);
	var y = mmToPixel(this.dpiy, mmY);
	var ret;
	ret = drawer.moveTo(this.hdc, x, y);
	if( !ret ){
		console.log("moveTo", " failed", x, y);
		throw new Error("moveTo failed");
	}
	if( this.debug ){
		console.log("moveTo", "ok", x, y);
	}
};

DrawerPrinter.prototype.lineTo = function(op){
	var mmX = Number(op[1]) + this.dx;
	var mmY = Number(op[2]) + this.dy;
	if( isNaN(mmX) || isNaN(mmY) ){
		console.log("lineTo", "failed", "bad arg", op[1], op[2]);
		throw new Error("lineTo failed");
	}
	var x = mmToPixel(this.dpix, mmX);
	var y = mmToPixel(this.dpiy, mmY);
	var ret;
	ret = drawer.lineTo(this.hdc, x, y);
	if( !ret ){
		console.log("lineTo", "failed", x, y);
		throw new Error("lineTo failed");
	}
	if( this.debug ){
		console.log("lineTo", "ok", x, y);
	}
};

DrawerPrinter.prototype.createFont = function(op){
	var name = "" + op[1], fontName, fontSize, weight, italic, font;
	var ok;
	if( name in this.fontDict ){
		ok = drawer.deleteObject(this.fontDict[name]);
		if( !ok ){
			console.log("deleteObject failed (createFont)");
		}
		this.fontDict[name] = undefined;
	}
	fontName = "" + op[2];
	fontSize = mmToPixel(this.dpiy, Number(op[3]));
	weight = op[4];
	if( weight === undefined ){
		weight = 0;
	} else {
		weight = weight ? drawer.FW_BOLD : 0;
	}
	italic = op[5];
	if( italic === undefined ){
		italic = 0;
	} else {
		italic = italic ? 1: 0;
	}
	font = drawer.createFont(fontName, fontSize, weight, italic);
	if( !font ){
		console.log("createFont", "failed", fontName, fontSize, weight, italic);
		throw new Error("createFont failed");
	}
	this.fontDict[name] = font;
	if( this.debug ){
		console.log("createFont", "ok", name);
	}
};

DrawerPrinter.prototype.setFont = function(op){
	var name = op[1], font;
	if( !(name in this.fontDict) ){
		console.log("setFont", "failed", "not such font", name);
		throw new Error("unknown font:" + name);
	}
	font = this.fontDict[name];
	var ret;
	ret = drawer.selectObject(this.hdc, font);
	if( !ret ){
		console.log("setFont", "failed", name);
		throw new Error("setFont failed");
	}
	if( this.debug ){
		console.log("setFont", "ok", name);
	}
};

DrawerPrinter.prototype.setTextColor = function(op){
	var r = Math.floor(Number(op[1]));
	var g = Math.floor(Number(op[2]));
	var b = Math.floor(Number(op[3]));
	var ret;
	ret = drawer.setTextColor(this.hdc, r, g, b);
	if( !ret ){
		console.log("setTextColor", "failed", r, g, b);
		throw new Error("setTextColor failed");
	}
	if( this.debug ){
		console.log("setTextColor", "ok", r, g, b);
	}
};

DrawerPrinter.prototype.createPen = function(op){
	var name = "" + op[1];
	var ok;
	if( name in this.penDict ){
		ok = drawer.deleteObject(this.penDict[name]);
		if( !ok ){
			console.log("deleteObject failed (createPen)");
		}
		this.penDict[name] = undefined;
	}
	var r = Math.floor(Number(op[2]));
	var g = Math.floor(Number(op[3]));
	var b = Math.floor(Number(op[4]));
	var width = mmToPixel(this.dpiy, Number(op[5]));
	if( width < 0 ){
		width = 1;
	}
	var pen = drawer.createPen(width, r, g, b);
	if( !pen ){
		console.log("createPen", "failed", r, g, b, width);
		throw new Error("createPen failed");
	}
	this.penDict[name] = pen;
	if( this.debug ){
		console.log("createPen", "ok", r, g, b, width);
	}
};

DrawerPrinter.prototype.setPen = function(op){
	var name = "" + op[1];
	if( !(name in this.penDict) ){
		console.log("setPen", "failed", "unknown pen", name);
		throw new Error("setPen failed");
	}
	var pen = this.penDict[name];
	if( !pen ){
		console.log("setPen", "fail", name);
		throw new Error("setPen failed");
	}
	var ret;
	ret = drawer.selectObject(this.hdc, pen);
	if( !ret ){
		console.log("setPen", "failed", name);
		throw new Error("setPen failed");
	}
};

DrawerPrinter.prototype.drawChars = function(op){
	var str = op[1];
	var xx = op[2], x, ix;
	var yy = op[3], y, iy;
	var i, n, ch;
	var dpix = this.dpix, dpiy = this.dpiy;
    var dx = this.dx, dy = this.dy;
	var ret;
	if( !(typeof str === "string" || str instanceof String) ){
		console.log("drawChars", "failed", "invalid string", str);
		throw new Error("invalid str to drawChars");
	}
    n = str.length;
	if( typeof xx === "number" || xx instanceof Number ){
		x = Number(xx) + dx;
		if( isNaN(x) ){
			console.log("drawChars", "failed", "invalid x", x, op);
			throw new Error("invalid x to drawChars");
		}
		ix = mmToPixel(dpix, x);
	}
	if( typeof yy === "number" || yy instanceof Number ){
		y = Number(yy) + dy;
		if( isNaN(y) ){
			console.log("drawChars", "failed", "invalid y", y);
			throw new Error("invalid y to drawChars");
		}
		iy = mmToPixel(dpiy, y);
	}
	for(i=0;i<n;i++){
		ch = str[i];
		ret = drawer.textOut(this.hdc, getX(i), getY(i), ch);
		if( !ret ){
			console.log("drawChars", "failed", ch);
			throw new Error("drawChars failed");
		}
	}
	if( this.debug ){
		console.log("drawChars", "ok", str, xx, yy);
	}
	
	function getX(i){
		var x;
		if( ix === undefined ){
			x = Number(xx[i]) + dx;
			if( isNaN(x) ){
				console.log("drawChars", "failed", "invalid x", xx[i], i, xx, op);
				throw new Error("invalid x to drawChars");
			}
			return mmToPixel(dpix, x);
		} else {
			return ix;
		}
	}
	
	function getY(i){
		var y;
		if( iy === undefined ){
			y = Number(yy[i]) + dy;
			if( isNaN(y) ){
				console.log("drawChars", "failed", "invalid y", yy[i]);
				throw new Error("invalid y to drawChars");
			}
			return mmToPixel(dpiy, y);
		} else {
			return iy;
		}
	}
}

