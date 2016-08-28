"use strict";

var path = require("path");
var fs = require("fs");
var consts = require("./drawer-consts");
var conti = require("conti");
var drawer = require("bindings")("drawer");

var gSettingDir = "./printer-settings";

function settingDir(){
    return gSettingDir;
}

function devmode_setting_path(name){
    return settingDir() + "/" + name + ".devmode";
}

function devnames_setting_path(name){
    return settingDir() + "/" + name + ".devnames";
}

function aux_setting_path(name){
    return settingDir() + "/" + name + ".json";
}

function saveSetting(name, context, done){
	conti.execPara([
		function(done){
	        writeToFile(devmode_setting_path(name), context.devmode, done);
		},
		function(done){
	        writeToFile(devnames_setting_path(name), context.devnames, done);
		},
		function(done){
	        writeToFile(aux_setting_path(name), JSON.stringify(context.aux || {}), done);
		}
	], done);
}

function deleteSetting(name, done){
	conti.execPara([
		function(done){
	        deleteFileIfExists(devmode_setting_path(name), done);
		},
		function(done){
	        deleteFileIfExists(devnames_setting_path(name), done);
		},
		function(done){
	        deleteFileIfExists(aux_setting_path(name), done);
		}
	], done)
}

function writeToFile(path, buffer, done){
    var stream = fs.createWriteStream(path);
    stream.on("error", function(err){
        done(err);
    });
    stream.on("finish", function(){
        done();
    });
    stream.end(buffer);
}

function readFromFile(path, cb){
    var stream = fs.createReadStream(path);
    var data = [];
    stream.on("error", function(err){
        cb(err);
    })
    stream.on("data", function(chunk){
        data.push(chunk);
    });
    stream.on("end", function(){
        cb(undefined, Buffer.concat(data));
    });
}

function deleteFileIfExists(path, done){
    fs.unlink(path, function(err){
        if( err ){
            if( err.code === "ENOENT" ){
                done(undefined, false);
            } else {
                done(err);
            }
        } else {
            done(undefined, true);
        }
    })
}

function readSetting(name, cb){
    var setting = {};
    conti.exec([
    	function(done){
    		readFromFile(devmode_setting_path(name), function(err, result){
    			if( err ){
    				done(err);
    				return;
    			}
    			setting.devmode = result;
    			done();
    		})
    	},
    	function(done){
    		readFromFile(devnames_setting_path(name), function(err, result){
    			if( err ){
    				done(err);
    				return;
    			}
    			setting.devnames = result;
    			done();
    		})
    	},
    	function(done){
    		readFromFile(aux_setting_path(name), function(err, result){
    			if( err ){
    				if( err.code === "ENOENT" ){
    					done();
    					return;
    				}
    				done(err);
    				return;
    			}
                var aux = JSON.parse(result.toString("utf-8"));
                setting.aux = aux;
                // for(key in aux){
                //     setting[key] = aux[key];
                // }
    			done();
    		})
    	},
	], function(err){
		if( err ){
			cb(err);
			return;
		}
		cb(undefined, setting);
	})
}

function parseDevmode(devmode){
    var devmodeDetail = drawer.parseDevmode(devmode);
    devmodeDetail.orientation = consts.orientationToLabel(devmodeDetail.orientation);
    devmodeDetail.paperSize = consts.paperSizeToLabel(devmodeDetail.paperSize);
    devmodeDetail.printQuality = consts.qualityToLabel(devmodeDetail.printQuality);
    devmodeDetail.defaultSource = consts.sourceToLabel(devmodeDetail.defaultSource);
    return devmodeDetail;
}

function parseSetting(setting){
	var retval = {
		devmode: parseDevmode(setting.devmode),
		devnames: drawer.parseDevnames(setting.devnames),
		aux: setting.aux
	};
	// Object.keys(setting).forEach(function(key){
	// 	if( key === "devmode" ){
	// 		retval.devmode = parseDevmode(setting.devmode);
	// 	} else if( key === "devnames" ){
	// 		retval.devnames = drawer.parseDevnames(setting.devnames);
	// 	} else {
	// 		retval[key] = setting[key];
	// 	}
	// });
	return retval;
}

function settingExists(name, cb){
	fs.stat(devmode_setting_path(name), function(err){
		if( err && err.code === "ENOENT" ){
			cb(undefined, false);
		} else if( err ){
			cb(err);
		} else {
			cb(undefined, true);
		}
	});	
}

function listSettings(cb){
	var settings = [];
     fs.readdir(settingDir(), function(err, files){
        if( err ){
        	console.log(err);
        	cb(err);
            return;
        }
        files.forEach(function(f){
            var ext = path.extname(f);
            var basename;
            if( ext === ".devmode" ){
                basename = path.basename(f, ext);
                settings.push(basename);
            }
        })
        cb(undefined, settings);
    });
}

exports.setSettingDir = function(path){
	gSettingDir = path;
};

exports.settingExists = function(name, cb){
	settingExists(name, cb);
};

exports.saveSetting = function(name, setting, done){
	saveSetting(name, setting, done);
}

exports.parseSetting = function(setting){
	return parseSetting(setting);
}

exports.readSetting = function(name, cb){
	readSetting(name, cb);
};

exports.deleteSetting = function(name, done){
	deleteSetting(name, done);
};

exports.listSettings = function(cb){
	return listSettings(cb);
};

