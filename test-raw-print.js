var drawer = require("./build/Release/drawer.node");
var setting = drawer.printerDialog();
if( setting ){
	var hdc = drawer.createDc(setting.devmode, setting.devnames);
	var dpi = drawer.getDpiOfHdc(hdc);
	var dpix = dpi.dpix;
	var dpiy = dpi.dpiy;
	drawer.beginPrint(hdc);
	drawer.startPage(hdc);
	drawer.moveTo(hdc, 1.0*dpix, 1.0*dpiy);
	drawer.lineTo(hdc, 3.0*dpix, 1.5*dpiy);
	drawer.textOut(hdc, 1.0*dpix, 2.0*dpiy, "Hello, world!");
	var helvetica = drawer.createFont("Helvetica", 0.5*dpiy);
	drawer.selectObject(hdc, helvetica);
	drawer.textOut(hdc, 1.0*dpix, 3.0*dpiy, "Hello, Helvetica");
	drawer.endPage(hdc);
	drawer.endPrint(hdc);
	drawer.deleteDc(hdc);
}
