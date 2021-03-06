#! /usr/bin/env node
"use strict";

var drawer = require("myclinic-drawer-printer");
var conti = require("conti");
var readline = require("readline");
var fs = require("fs");

var args = process.argv.slice(2);
if( args.length === 0 ){
	printUsage();
	process.exit(1);
}

function printUsage(){
	console.log("usage: drawer-print COMMAND args...");
	console.log("COMMAND is ");
	console.log("  init -- creates 'printer-settings' directory");
	console.log("  list -- to list the current settings");
	console.log("  create NAME -- create a printer setting");
	console.log("  show [NAME, ...] -- prints details of settings");
	console.log("  edit NAME -- modify setting");
	console.log("  delete NAME -- delete a setting");
	console.log("  print PAGES [SETTING]  -- print PAGES with optional SETTING")
	console.log("  help -- print this message");
}

var command = args.shift();
switch(command){
	case "init": doInit(args); break;
	case "list": doListSettings(); break;
	case "create": doCreate(args); break;
	case "show": doShow(args); break;
	case "edit": doEdit(args); break;
	case "delete": doDelete(args); break;
	case "print": doPrint(args); break;
	default: console.log("unknown command: " + command); process.exit(2);
}

function doInit(args){
	if( args.length !== 0 ){
		console.log("usage: drawer-print init");
		process.exit(2);
	}
	fs.mkdir("printer-settings", function(err){
		if( err ){
			if( err.code === 'EEXIST' ){
				console.log("printer-settings already exists (not created)");
			} else {
				console.log(err);
			}
			process.exit(3);
		} else {
			console.log("printer-settings directory created");
		}
	})
}

function doListSettings(){
	drawer.listSettings(function(err, result){
		if( err ){
			console.log(err);
			return;
		}
		result.forEach(function(name){
			console.log(name);
		})
	});
}

function doCreate(args){
	if( args.length !== 1 ){
		console.log("usage: dawer-print create NAME");
		process.exit(3);
	}
	var name = args[0];
	var setting = drawer.printerDialog();
	if( !setting ){
		return;
	}
	drawer.saveSetting(name, setting, function(err){
		if( err ){
			console.log("ERROR:", err);
			return;
		}
		console.log("Setting " + name + " has been created successfully.");
	});;
}

function doShow(args){
	if( args.length === 0 ){
		drawer.listSettings(function(err, list){
			if( err ){
				console.log(err);
				return;
			}
			show(list);
		})
	} else {
		show(args);
	}

	function show(list){
		conti.forEach(list, function(name, done){
			drawer.readSetting(name, function(err, result){
				if( err ){
					console.log(err);
					done(); // proceed anyway
					return;
				}
				var detail = drawer.parseSetting(result);
				console.log(name, ":");
				console.log(JSON.stringify(detail, null, 4));
				console.log();
			})
		}, function(err){});
	}
}

function doEdit(args){
	if( args.length !== 1 ){
		console.log("usage: drawer-print edit NAME");
		process.exit(3);
	}
	var name = args[0];
	var setting;
	conti.exec([
		function(done){
			drawer.readSetting(name, function(err, result){
				if( err ){
					done(err);
					return;
				}
				setting = result;
				done();
			})
		},
		function(done){
			var newSetting = drawer.printerDialog(setting);
			if( newSetting ){
				drawer.saveSetting(name, newSetting, done);
			} else {
				done("canceled");
				return;
			}
		}
	], function(err){
		if( err ){
			console.log(err);
			process.exit(3);
		}
		console.log("Setting " + name + " has been modified successfully.");
	})
}

function doDelete(args){
	if( args.length !== 1 ){
		console.log("usage: drawer-print delete NAME");
		process.exit(1);
	}
	var name = args[0];
	var rl = readline.createInterface({
		input: process.stdin,
		output: process.stdout,
		prompt: ""
	});
	rl.question("Really delete the setting: " + name + "? (Y/N) ", function(ans){
		if( ans !== "Y" ){
			console.log("Not deleted");
			rl.close();
			return;
		}
		drawer.deleteSetting(name, function(err){
			if( err ){
				console.log("ERROR: " + err);
				process.exit(3);
			}
			console.log("Setting " + name + " has been deleted successfully.");
			rl.close();
		})
	})
}

function doPrint(args){
	if( !(args.length === 1 || args.length === 2) ){
		console.log("usage: drawer-print print PAGES [SETTING]");
		process.exit(2);
	}
	var pagesPath = args[0];
	var settingName = args[1];
	var pages;
	var setting;
	conti.exec([
		function(done){
			var pagesJson = fs.readFileSync(pagesPath, {encoding: "utf-8"});
			pages = JSON.parse(pagesJson);
			done();
		},
		function(done){
			if( settingName ){
				drawer.readSetting(settingName, function(err, result){
					if( err ){
						done(err);
						return;
					}
					setting = result;
					done();
				})
			} else {
				setting = drawer.printerDialog();
				if( !setting ){
					done("canceled");
					return;
				}
				done();
			}
		},
		function(done){
			drawer.printPages(pages, setting);
			done();
		}
	], function(err){
		if( err ){
			throw err;
		}
		console.log("Printing done");
	})
}



