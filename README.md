# myclinic-drawer-printer

A printing library for Windows OS.

## API

```
createWindow() ==> hwnd
disposeWindow(hwnd) ==> bool (ok)
getDc(hwnd) ==> hdc
releaseDc(hwnd, hdc) ==> bool (ok)
measureText(hdc, string) => { cx:..., cy:... }
createFont(fontname, size, weight?, italic?) ==> HANDLE
deleteObject(obj) ==> bool (ok)
getDpiOfHdc(hdc) ==> { dpix:..., dpiy:... }
printerDialog(devmode?, devnames?) ==> { devmode:..., devnames:... }
parseDevmode(devmode) ==> { deviceName:..., ... }
parseDevnames(devnames) ==> { driver:..., ... }
createDc(devmode, devnames) ==> hdc
deleteDc(hdc) ==> bool (ok)
beginPrint(hdc) ==> (throws exception if it fails)
endPrint(hdc) ==> (throws exception if it fails)
abortPrint(hdc) ==> (throws exception if it fails)
startPage(hdc) ==> (throws exception if it fails)
endPage(hdc) ==> (throws exception if it fails)
moveTo(hdc, x, y) ==> (throws exception if it fails)
lineTo(hdc, x, y) ==> (throws exception if it fails)
textOut(hdc, x, y, text) ==> (throws exception if it fails)
selectObject(hdc, handle) ==> (throws exception if it fails)
setTextColor(hdc, r, g, b) ==> (throws exception if it fails)
createPen(width, r, g, b) ==> (throws exception if it fails)
setBkMode(hdc, mode) ==> (throws exception if it fails)
```

## License
This software is released under the MIT License, see [LICENSE.txt](LICENSE.txt).
