"use strict";

var drawer = require("./index");
var conti = require("conti");

var settingName = "テスト設定"
drawer.setSettingDir("./printer-settings");
conti.exec([
	function(done){
		drawer.settingExists(settingName, function(err, exists){
			if( err ){
				done(err);
				return;
			}
			if( !exists ){
				var setting = drawer.printerDialog();
				if( setting ){
					drawer.saveSetting(settingName, setting, done);
				} else {
					done("test setting not canceled");
				}
			} else {
				done();
			}
		})
	},
	function(done){
		drawer.listSettings(function(err, result){
			if( err ){
				done(err);
				return;
			}
			console.log("list", result);
			done();
		});
	},
	function(done){
		drawer.readSetting(settingName, function(err, result){
			if( err ){
				done(err);
				return;
			}
			console.log(result);
			var newSetting = drawer.printerDialog(result);
			if( newSetting ){
				drawer.saveSetting(settingName, newSetting, done);
			} else {
				done();
			}
		})
	},
	function(done){
		drawer.readSetting(settingName, function(err, result){
			if( err ){
				done(err);
				return;
			}
			drawer.printerDialog(result);
			console.log(JSON.stringify(drawer.parseSetting(result), 4));
			done();
		})
	},
	function(done){
		drawer.deleteSetting(settingName, done);
	}
], function(err){
	if( err ){
		console.log(err);
		return;
	}
	console.log("done");
})

