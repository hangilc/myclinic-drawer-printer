# myclinic-drawer-printer

A printing library for Windows OS.

## Test

```
> node test-raw-print.js
```

I confirmed that the above command prints successfully with node.js 0.12.7 (64bit/32bit) and 6.9.1 (64bit/32bit).

## API

```
printPages(pages, setting)
printerDialog(optDefaultSetting)
setSettingDir(path)
settingExists(name, cb)
listSettings(cb)
saveSettings(name, setting, done)
readSetting(name, cb)
parseSetting(setting)
deleteSetting(name, done)

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
```

## License
This software is released under the MIT License, see [LICENSE.txt](LICENSE.txt).
