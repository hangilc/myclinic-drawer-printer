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

drawer.printPages(pages);