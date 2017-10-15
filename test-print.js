"use strict";

var drawer = require("./index");
var pages = [
	[
		["move_to", 10, 10],
		["line_to", 40, 20],
		["create_font", "mincho6", "MS Mincho", 6, 0, 0],
		["set_font", "mincho6"],
		["draw_chars", "こんにちは世界", [10, 16, 22, 28, 34, 40, 46], 30],
		["set_text_color", 0, 244, 0],
		["draw_chars", "こんにちは世界", [10, 16, 22, 28, 34, 40, 46], 50],
		["create_pen", "blue", 0, 0, 255, 1],
		["set_pen", "blue"],
		["move_to", 10, 20],
		["line_to", 40, 10]
	]
];

var setting = drawer.printerDialog();

function scaleDrawerOps(ops, scale){
	var scaled = [];
	var n = ops.length;
	for(i=0;i<n;i++){
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
	for(i=0;i<n;i++){
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

if( setting ){
	setting.aux = {
		dx: 30,
		dy: 20,
		scale: 0.5
	};
	// if( setting.aux && setting.aux.scale ){
	// 	for(var i=0;i<pages.length;i++){
	// 		pages[i] = scaleDrawerOps(pages[i], setting.aux.scale);
	// 	}
	// }
	// var dx = 0;
	// var dy = 0;
	// if( setting.aux.dx ){
	// 	dx = +setting.aux.dx;
	// }
	// if( setting.aux.dy ){
	// 	dy = +setting.aux.dy;
	// }
	// if( dx != 0 || dy != 0 ){
	// 	for(var i=0;i<pages.length;i++){
	// 		pages[i] = shiftDrawerOps(pages[i], dx, dy);
	// 	}
	// }
	drawer.printPages(pages, setting);
}
