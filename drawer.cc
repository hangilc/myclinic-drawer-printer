#include <nan.h>

using namespace v8;

static WCHAR *windowClassName = L"DRAWERWINDOW";

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL initWindowClass(void){
	WNDCLASSW wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpszClassName = windowClassName;
	if( !RegisterClassW(&wndClass) ){
		return false;
	} else {
		return true;
	}
}

static HWND create_window(){
	return CreateWindowW(windowClassName, L"Dummy Window", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, GetModuleHandle(NULL), NULL);
}

static BOOL dispose_window(HWND hwnd){
	return DestroyWindow(hwnd);
}

static void parse_devnames(DEVNAMES *devnames, WCHAR **driver, WCHAR **device, WCHAR **output)
{
	*driver = (WCHAR *)(((WCHAR *)devnames) + devnames->wDriverOffset);
	*device = (WCHAR *)(((WCHAR *)devnames) + devnames->wDeviceOffset);
	*output = (WCHAR *)(((WCHAR *)devnames) + devnames->wOutputOffset);
}

void createWindow(const Nan::FunctionCallbackInfo<Value>& args){
	// createWidnow()
	HWND hwnd = create_window();
	if( hwnd == NULL ){
		printf("%d\n", GetLastError());
		Nan::ThrowTypeError("create_window failed");
		return;
	}
	args.GetReturnValue().Set(Nan::New((int)hwnd));
}

void disposeWindow(const Nan::FunctionCallbackInfo<Value>& args){
	// disposeWindow(hwnd)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong argument");
		return;
	}
	HWND hwnd = (HWND)args[0]->Int32Value();
	BOOL ok = dispose_window(hwnd);
	args.GetReturnValue().Set(Nan::New(ok));
}

void getDc(const Nan::FunctionCallbackInfo<Value>& args){
	// getDc(hwnd)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong argument");
		return;
	}
	HWND hwnd = (HWND)args[0]->Int32Value();
	HDC hdc = GetDC(hwnd);
	args.GetReturnValue().Set(Nan::New((int)hdc));
}

void releaseDc(const Nan::FunctionCallbackInfo<Value>& args){
	// releaseDc(hwnd, hdc)
	if( args.Length() < 2 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}
	if( !args[0]->IsInt32() || !args[1]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HWND hwnd = (HWND)args[0]->Int32Value();
	HDC hdc = (HDC)args[1]->Int32Value();
	BOOL ok = ReleaseDC(hwnd, hdc);
	args.GetReturnValue().Set(Nan::New(ok));
}

void measureText(const Nan::FunctionCallbackInfo<Value>& args){
	// measureText(hdc, string) => { cx:..., cy:... }
	if( args.Length() < 2 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong argument");
		return;
	}
	if( !args[1]->IsString() ){
		Nan::ThrowTypeError("wrong argument");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	String::Value textValue(args[1]);
	SIZE mes;
	BOOL ok = GetTextExtentPoint32W(hdc, (LPCWSTR)*textValue, textValue.length(), &mes);
	if( !ok ){
		Nan::ThrowTypeError("GetTextExtentPoint32W failed");
		return;
	}
	Local<Object> obj = Nan::New<Object>();
	obj->Set(Nan::New("cx").ToLocalChecked(), Nan::New(mes.cx));
	obj->Set(Nan::New("cy").ToLocalChecked(), Nan::New(mes.cy));
	args.GetReturnValue().Set(obj);
}

void createFont(const Nan::FunctionCallbackInfo<Value>& args){
	// createFont(fontname, size, weight?, italic?) ==> HANDLE
	if( args.Length() < 2 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}
	if( !args[0]->IsString() ){
		Nan::ThrowTypeError("invalid font name");
		return;
	}
	if( !args[1]->IsInt32() ){
		Nan::ThrowTypeError("invalid font size");
		return;
	}
	if( args.Length() >= 3 && !args[2]->IsInt32() ){
		Nan::ThrowTypeError("invalid font weight");
		return;
	}
	if( args.Length() >= 4 && !args[3]->IsInt32() ){
		Nan::ThrowTypeError("invalid font italic");
		return;
	}
	String::Value fontName(args[0]);
	long size = args[1]->Int32Value();
	long weight = args.Length() >= 3 ? args[2]->Int32Value() : 0;
	long italic = args.Length() >= 4 ? args[3]->Int32Value() : 0;
	LOGFONTW logfont;
	ZeroMemory(&logfont, sizeof(logfont));
	logfont.lfHeight = size;
	logfont.lfWeight = weight;
	logfont.lfItalic = static_cast<BYTE>(italic);
	logfont.lfCharSet = DEFAULT_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	if( wcscpy_s(logfont.lfFaceName, LF_FACESIZE, (const wchar_t *)*fontName) != 0 ){
		Nan::ThrowTypeError("Too long font name");
		return;
	}
	HFONT font = CreateFontIndirectW(&logfont);
	args.GetReturnValue().Set(Nan::New((int)(UINT_PTR)font));
}

void deleteObject(const Nan::FunctionCallbackInfo<Value>& args){
	// deleteObject(obj)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong argument");
		return;
	}
	HANDLE object = (HANDLE)args[0]->Int32Value();
	BOOL ok = DeleteObject(object);
	args.GetReturnValue().Set(ok);
}

void getDpiOfHdc(const Nan::FunctionCallbackInfo<Value>& args){
	// getDpiOfHdc(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int dpix = GetDeviceCaps(hdc, LOGPIXELSX);
	int dpiy = GetDeviceCaps(hdc, LOGPIXELSY);
	Local<Object> obj = Nan::New<v8::Object>();
	obj->Set(Nan::New("dpix").ToLocalChecked(), Nan::New(dpix));
	obj->Set(Nan::New("dpiy").ToLocalChecked(), Nan::New(dpiy));
	args.GetReturnValue().Set(obj);
}

static HANDLE alloc_handle(void *data, int len)
{
	HANDLE handle;
	void *ptr;

	handle = GlobalAlloc(GHND, len);
	ptr = GlobalLock(handle);
	memmove(ptr, data, len);
	GlobalUnlock(handle);
	return handle;
}

void printerDialog(const Nan::FunctionCallbackInfo<Value>& args){
	// printerDialog(devmode?, devnames?)
	HWND hwnd = create_window();
	if( hwnd == NULL ){
		Nan::ThrowTypeError("create_window failed");
		return;
	}
	DEVMODEW *devmodePtr = NULL;
	int devmodeLength = 0;
	DEVNAMES *devnamesPtr = NULL;
	int devnamesLength = 0;
	if( args.Length() >= 1 ){
		if( !args[0]->IsObject() ){
			Nan::ThrowTypeError("wrong arguments");
			return;
		}
		Local<Object> obj = args[0]->ToObject();
		devmodePtr = (DEVMODEW *)node::Buffer::Data(obj);
		devmodeLength = node::Buffer::Length(obj);
	}
	if( args.Length() >= 2 ){
		if( !args[1]->IsObject() ){
			Nan::ThrowTypeError("wrong arguments");
			return;
		}
		Local<Object> obj = args[1]->ToObject();
		devnamesPtr = (DEVNAMES *)node::Buffer::Data(obj);
		devnamesLength = node::Buffer::Length(obj);
	}
	PRINTDLGEXW pd;
	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = hwnd;
	pd.Flags = PD_NOPAGENUMS;
	pd.nCopies = 1;
	pd.nStartPage = START_PAGE_GENERAL;
	if( devmodePtr ){
		pd.hDevMode = alloc_handle(devmodePtr, devmodeLength);
	}
	if( devnamesPtr ){
		pd.hDevNames = alloc_handle(devnamesPtr, devnamesLength);
	}
	HRESULT res = PrintDlgExW(&pd);
	dispose_window(hwnd);
	if( res == S_OK && pd.dwResultAction != PD_RESULT_CANCEL ){
		DEVMODEW *devmodePtr = (DEVMODEW *)GlobalLock(pd.hDevMode);
		int devmodeLength = sizeof(DEVMODEW) + devmodePtr->dmDriverExtra;
		Local<Object> devmodeBuffer = Nan::CopyBuffer((char *)devmodePtr, devmodeLength).ToLocalChecked();
		GlobalUnlock(pd.hDevMode);
		GlobalFree(pd.hDevMode);
		DEVNAMES *devnamesPtr = (DEVNAMES *)GlobalLock(pd.hDevNames);
		WCHAR *outputPtr = ((WCHAR *)devnamesPtr) + devnamesPtr->wOutputOffset;
		int outputLen = wcslen(outputPtr);
		int devnamesLength = (devnamesPtr->wOutputOffset + outputLen + 1) * 2;
		Local<Object> devnamesBuffer = Nan::CopyBuffer((char *)devnamesPtr, devnamesLength).ToLocalChecked();
		GlobalUnlock(pd.hDevNames);
		GlobalFree(pd.hDevNames);
		Local<Object> obj = Nan::New<v8::Object>();
		obj->Set(Nan::New("devmode").ToLocalChecked(), devmodeBuffer);
		obj->Set(Nan::New("devnames").ToLocalChecked(), devnamesBuffer);
		args.GetReturnValue().Set(obj);
	} else {
		args.GetReturnValue().Set(false);
	}
}

void parseDevmode(const Nan::FunctionCallbackInfo<Value>& args){
	// parseDevmode(devmode)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsObject() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	Local<Object> devmodeBuffer = args[0]->ToObject();
	DEVMODEW *devmodePtr = (DEVMODEW *)node::Buffer::Data(devmodeBuffer);
	DWORD fields = devmodePtr->dmFields;
	const uint16_t *cDevName = (const uint16_t *)devmodePtr->dmDeviceName;
	Local<String> deviceName = Nan::New(cDevName, lstrlenW((LPCWSTR)cDevName)).ToLocalChecked();
	Local<Object> obj = Nan::New<v8::Object>();
	obj->Set(Nan::New("deviceName").ToLocalChecked(), deviceName);
	obj->Set(Nan::New("orientation").ToLocalChecked(), Nan::New(devmodePtr->dmOrientation));
	obj->Set(Nan::New("paperSize").ToLocalChecked(), Nan::New(devmodePtr->dmPaperSize));
	obj->Set(Nan::New("copies").ToLocalChecked(), Nan::New(devmodePtr->dmCopies));
	obj->Set(Nan::New("printQuality").ToLocalChecked(), Nan::New(devmodePtr->dmPrintQuality));
	obj->Set(Nan::New("defaultSource").ToLocalChecked(), Nan::New(devmodePtr->dmDefaultSource));
	args.GetReturnValue().Set(obj);
}

void parseDevnames(const Nan::FunctionCallbackInfo<Value>& args){
	// parseDevnames(devnames)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsObject() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	Local<Object> devnamesBuffer = args[0]->ToObject();
	DEVNAMES *data = (DEVNAMES *)node::Buffer::Data(devnamesBuffer);
	WCHAR *driver, *device, *output;
	parse_devnames(data, &driver, &device, &output);
	Local<String> driverString = Nan::New((const uint16_t *)driver, lstrlenW(driver)).ToLocalChecked();
	Local<String> deviceString = Nan::New((const uint16_t *)device, lstrlenW(device)).ToLocalChecked();
	Local<String> outputString = Nan::New((const uint16_t *)output, lstrlenW(output)).ToLocalChecked();
	Local<Object> obj = Nan::New<v8::Object>();
	obj->Set(Nan::New("driver").ToLocalChecked(), driverString);
	obj->Set(Nan::New("device").ToLocalChecked(), deviceString);
	obj->Set(Nan::New("output").ToLocalChecked(), outputString);
	args.GetReturnValue().Set(obj);
}

void createDc(const Nan::FunctionCallbackInfo<Value>& args){
	// createDc(devmode, devnames)
	if( args.Length() < 2 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsObject() || !args[1]->IsObject() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	DEVMODEW *devmodePtr = (DEVMODEW *)node::Buffer::Data(args[0]->ToObject());
	DEVNAMES *devnamesPtr = (DEVNAMES *)node::Buffer::Data(args[1]->ToObject());
	WCHAR *driver, *device, *output;
	parse_devnames(devnamesPtr, &driver, &device, &output);
	HDC hdc = CreateDCW(driver, device, NULL, devmodePtr);
	if( hdc == NULL ){
		Nan::ThrowTypeError("createDC failed");
		return;
	}
	args.GetReturnValue().Set(Nan::New((int)hdc));
}

void deleteDc(const Nan::FunctionCallbackInfo<Value>& args){
	// deleteDc(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	BOOL ok = DeleteDC(hdc);
	args.GetReturnValue().Set(ok);
}

void beginPrint(const Nan::FunctionCallbackInfo<Value>& args){
	// beginPrint(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	DOCINFOW docinfo;
	ZeroMemory(&docinfo, sizeof(docinfo));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = L"drawer printing";
	int ret = StartDocW(hdc, &docinfo);
	if( ret <= 0 ){
		Nan::ThrowTypeError("StartDoc failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void endPrint(const Nan::FunctionCallbackInfo<Value>& args){
	// endPrint(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = EndDoc(hdc);
	if( ret <= 0 ){
		Nan::ThrowTypeError("EndDoc failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void abortPrint(const Nan::FunctionCallbackInfo<Value>& args){
	// abortPrint(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = AbortDoc(hdc);
	if( ret <= 0 ){
		Nan::ThrowTypeError("AbortDoc failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void startPage(const Nan::FunctionCallbackInfo<Value>& args){
	// startPage(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = StartPage(hdc);
	if( ret <= 0 ){
		Nan::ThrowTypeError("StartPage failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void endPage(const Nan::FunctionCallbackInfo<Value>& args){
	// endPage(hdc)
	if( args.Length() < 1 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = EndPage(hdc);
	if( ret <= 0 ){
		Nan::ThrowTypeError("EndPage failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void moveTo(const Nan::FunctionCallbackInfo<Value>& args){
	// moveTo(hdc, x, y)
	if( args.Length() < 3 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long x = args[1]->Int32Value();
	long y = args[2]->Int32Value();
	BOOL ok = MoveToEx(hdc, x, y, NULL);
	if( !ok ){
		Nan::ThrowTypeError("MoveToEx failed");
		return;
	}
	args.GetReturnValue().Set(ok);
}

void lineTo(const Nan::FunctionCallbackInfo<Value>& args){
	// lineTo(hdc, x, y)
	if( args.Length() < 3 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long x = args[1]->Int32Value();
	long y = args[2]->Int32Value();
	BOOL ok = LineTo(hdc, x, y);
	if( !ok ){
		Nan::ThrowTypeError("LineTo failed");
		return;
	}
	args.GetReturnValue().Set(ok);
}

void textOut(const Nan::FunctionCallbackInfo<Value>& args){
	// textOut(hdc, x, y, text)
	if( args.Length() < 4 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() || !args[3]->IsString() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long x = args[1]->Int32Value();
	long y = args[2]->Int32Value();
	String::Value textValue(args[3]);
	
	BOOL ok = TextOutW(hdc, x, y, (LPWSTR)*textValue, textValue.length());
	if( !ok ){
		Nan::ThrowTypeError("TextOutW failed");
		return;
	}
	args.GetReturnValue().Set(ok);
}

void selectObject(const Nan::FunctionCallbackInfo<Value>& args){
	// selectObject(hdc, handle)
	if( args.Length() < 2 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	HANDLE obj = (HANDLE)args[1]->Int32Value();
	HANDLE prev = SelectObject(hdc, obj);
	if( prev == NULL || prev == HGDI_ERROR ){
		Nan::ThrowTypeError("SelectObject failed");
		return;
	}
	args.GetReturnValue().Set((int)prev);
}

void setTextColor(const Nan::FunctionCallbackInfo<Value>& args){
	// setTextColor(hdc, r, g, b)
	if( args.Length() < 4 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() || !args[3]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long r = args[1]->Int32Value();
	long g = args[2]->Int32Value();
	long b = args[3]->Int32Value();
	COLORREF ret = SetTextColor(hdc, RGB(r, g, b));
	if( ret == CLR_INVALID ){
		Nan::ThrowTypeError("SetTextColor failed");
		return;
	}
	args.GetReturnValue().Set(TRUE);
}

void createPen(const Nan::FunctionCallbackInfo<Value>& args){
	// createPen(width, r, g, b)
	if( args.Length() < 4 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() || !args[3]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	long width = args[0]->Int32Value();
	long r = args[1]->Int32Value();
	long g = args[2]->Int32Value();
	long b = args[3]->Int32Value();
	HPEN pen = CreatePen(PS_SOLID, width, RGB(r, g, b));
	if( pen == NULL ){
		Nan::ThrowTypeError("CreatePen failed");
		return;
	}
	args.GetReturnValue().Set((int)pen);
}

void setBkMode(const Nan::FunctionCallbackInfo<Value>& args){
	// setBkMode(hdc, mode)
	if( args.Length() < 2 ){
		Nan::ThrowTypeError("wrong number of arguments");
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() ){
		Nan::ThrowTypeError("wrong arguments");
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int mode = args[1]->Int32Value();
	int prev = SetBkMode(hdc, mode);
	if( prev == 0 ){
		Nan::ThrowTypeError("SetBkMode failed");
		return;
	}
	args.GetReturnValue().Set(prev);
}

void Init(v8::Local<v8::Object> exports){
	if( !initWindowClass() ){
		Nan::ThrowTypeError("initWindowClass failed");
		return;
	}
	exports->Set(Nan::New("createWindow").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(createWindow)->GetFunction());
	exports->Set(Nan::New("disposeWindow").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(disposeWindow)->GetFunction());
	exports->Set(Nan::New("getDc").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(getDc)->GetFunction());
	exports->Set(Nan::New("releaseDc").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(releaseDc)->GetFunction());
	exports->Set(Nan::New("measureText").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(measureText)->GetFunction());
	exports->Set(Nan::New("createFont").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(createFont)->GetFunction());
	exports->Set(Nan::New("deleteObject").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(deleteObject)->GetFunction());
	exports->Set(Nan::New("getDpiOfHdc").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(getDpiOfHdc)->GetFunction());
	exports->Set(Nan::New("printerDialog").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(printerDialog)->GetFunction());
	exports->Set(Nan::New("parseDevmode").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(parseDevmode)->GetFunction());
	exports->Set(Nan::New("parseDevnames").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(parseDevnames)->GetFunction());
	exports->Set(Nan::New("createDc").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(createDc)->GetFunction());
	exports->Set(Nan::New("deleteDc").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(deleteDc)->GetFunction());
	exports->Set(Nan::New("beginPrint").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(beginPrint)->GetFunction());
	exports->Set(Nan::New("endPrint").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(endPrint)->GetFunction());
	exports->Set(Nan::New("abortPrint").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(abortPrint)->GetFunction());
	exports->Set(Nan::New("startPage").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(startPage)->GetFunction());
	exports->Set(Nan::New("endPage").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(endPage)->GetFunction());
	exports->Set(Nan::New("moveTo").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(moveTo)->GetFunction());
	exports->Set(Nan::New("lineTo").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(lineTo)->GetFunction());
	exports->Set(Nan::New("textOut").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(textOut)->GetFunction());
	exports->Set(Nan::New("selectObject").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(selectObject)->GetFunction());
	exports->Set(Nan::New("setTextColor").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(setTextColor)->GetFunction());
	exports->Set(Nan::New("createPen").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(createPen)->GetFunction());
	exports->Set(Nan::New("setBkMode").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(setBkMode)->GetFunction());
	exports->Set(Nan::New("bkModeOpaque").ToLocalChecked(), Nan::New(OPAQUE));
	exports->Set(Nan::New("bkModeTransparent").ToLocalChecked(), Nan::New(TRANSPARENT));
	exports->Set(Nan::New("FW_DONTCARE").ToLocalChecked(), Nan::New(FW_DONTCARE));
	exports->Set(Nan::New("FW_BOLD").ToLocalChecked(), Nan::New(FW_BOLD));
}

NODE_MODULE(drawer, Init)


