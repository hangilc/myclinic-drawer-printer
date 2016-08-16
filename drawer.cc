#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <windows.h>

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

static void report_error(Isolate *isolate, const char *msg){
	isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, msg)));
}

void createWindow(const v8::FunctionCallbackInfo<Value>& args){
	// createWidnow()
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	HWND hwnd = create_window();
	if( hwnd == NULL ){
		report_error(isolate, "create_window failed");
		return;
	}
	args.GetReturnValue().Set(Int32::New(isolate, (int)hwnd));
}

void disposeWindow(const v8::FunctionCallbackInfo<Value>& args){
	// disposeWindow(hwnd)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong argument"))
		);
		return;
	}
	HWND hwnd = (HWND)args[0]->Int32Value();
	BOOL ok = dispose_window(hwnd);
	args.GetReturnValue().Set(Boolean::New(isolate, ok));
}

void getDc(const v8::FunctionCallbackInfo<Value>& args){
	// getDc(hwnd)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong argument"))
		);
		return;
	}
	HWND hwnd = (HWND)args[0]->Int32Value();
	HDC hdc = GetDC(hwnd);
	args.GetReturnValue().Set(Int32::New(isolate, (int)hdc));
}

void releaseDc(const v8::FunctionCallbackInfo<Value>& args){
	// releaseDc(hwnd, hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 2 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}
	if( !args[0]->IsInt32() || !args[1]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HWND hwnd = (HWND)args[0]->Int32Value();
	HDC hdc = (HDC)args[1]->Int32Value();
	BOOL ok = ReleaseDC(hwnd, hdc);
	args.GetReturnValue().Set(ok);
}

void measureText(const v8::FunctionCallbackInfo<Value>& args){
	// measureText(hdc, string) => { cx:..., cy:... }
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 2 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong argument"))
		);
		return;
	}
	if( !args[1]->IsString() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong argument"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	String::Value textValue(args[1]);
	SIZE mes;
	BOOL ok = GetTextExtentPoint32W(hdc, (LPCWSTR)*textValue, textValue.length(), &mes);
	if( !ok ){
		report_error(isolate, "GetTextExtentPoint32W failed");
		return;
	}
	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "cx"), Int32::New(isolate, mes.cx));
	obj->Set(String::NewFromUtf8(isolate, "cy"), Int32::New(isolate, mes.cy));
	args.GetReturnValue().Set(obj);
}

void createFont(const v8::FunctionCallbackInfo<Value>& args){
	// createFont(fontname, size, weight?, italic?) ==> HANDLE
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 2 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}
	if( !args[0]->IsString() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "invalid font name"))
		);
		return;
	}
	if( !args[1]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "invalid font size"))
		);
		return;
	}
	if( args.Length() >= 3 && !args[2]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "invalid font weight"))
		);
		return;
	}
	if( args.Length() >= 4 && !args[3]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "invalid font italic"))
		);
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
		isolate->ThrowException(Exception::Error(String::NewFromUtf8(
			isolate, "Too long font name"
		)));
		return;
	}
	HFONT font = CreateFontIndirectW(&logfont);
	args.GetReturnValue().Set(Int32::New(isolate, (int)font));
}

void deleteObject(const v8::FunctionCallbackInfo<Value>& args){
	// deleteObject(obj)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong argument"))
		);
		return;
	}
	HANDLE object = (HANDLE)args[0]->Int32Value();
	BOOL ok = DeleteObject(object);
	args.GetReturnValue().Set(ok);
}

void getDpiOfHdc(const v8::FunctionCallbackInfo<Value>& args){
	// getDpiOfHdc(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int dpix = GetDeviceCaps(hdc, LOGPIXELSX);
	int dpiy = GetDeviceCaps(hdc, LOGPIXELSY);
	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "dpix"), Int32::New(isolate, dpix));
	obj->Set(String::NewFromUtf8(isolate, "dpiy"), Int32::New(isolate, dpiy));
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

void printerDialog(const v8::FunctionCallbackInfo<Value>& args){
	// printerDialog(devmode?, devnames?)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	HWND hwnd = create_window();
	if( hwnd == NULL ){
		report_error(isolate, "create_window failed");
		return;
	}
	DEVMODEW *devmodePtr = NULL;
	int devmodeLength = 0;
	DEVNAMES *devnamesPtr = NULL;
	int devnamesLength = 0;
	if( args.Length() >= 1 ){
		if( !args[0]->IsObject() ){
			isolate->ThrowException(
				Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
			);
			return;
		}
		Local<Object> obj = args[0]->ToObject();
		devmodePtr = (DEVMODEW *)node::Buffer::Data(obj);
		devmodeLength = node::Buffer::Length(obj);
	}
	if( args.Length() >= 2 ){
		if( !args[1]->IsObject() ){
			isolate->ThrowException(
				Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
			);
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
		Local<Object> devmodeBuffer = node::Buffer::New(isolate, (char *)devmodePtr, devmodeLength);
		GlobalUnlock(pd.hDevMode);
		GlobalFree(pd.hDevMode);
		DEVNAMES *devnamesPtr = (DEVNAMES *)GlobalLock(pd.hDevNames);
		WCHAR *outputPtr = ((WCHAR *)devnamesPtr) + devnamesPtr->wOutputOffset;
		int outputLen = wcslen(outputPtr);
		int devnamesLength = (devnamesPtr->wOutputOffset + outputLen + 1) * 2;
		Local<Object> devnamesBuffer = node::Buffer::New(isolate, (char *)devnamesPtr, devnamesLength);
		GlobalUnlock(pd.hDevNames);
		GlobalFree(pd.hDevNames);
		Local<Object> obj = Object::New(isolate);
		obj->Set(String::NewFromUtf8(isolate, "devmode"), devmodeBuffer);
		obj->Set(String::NewFromUtf8(isolate, "devnames"), devnamesBuffer);
		args.GetReturnValue().Set(obj);
	} else {
		args.GetReturnValue().Set(false);
	}
}

void parseDevmode(const v8::FunctionCallbackInfo<Value>& args){
	// parseDevmode(devmode)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsObject() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	Local<Object> devmodeBuffer = args[0]->ToObject();
	DEVMODEW *devmodePtr = (DEVMODEW *)node::Buffer::Data(devmodeBuffer);
	DWORD fields = devmodePtr->dmFields;
	Local<String> deviceName = String::NewFromTwoByte(isolate, (const uint16_t *)devmodePtr->dmDeviceName);
	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "deviceName"), deviceName);
	obj->Set(String::NewFromUtf8(isolate, "orientation"), Int32::New(isolate, devmodePtr->dmOrientation));
	obj->Set(String::NewFromUtf8(isolate, "paperSize"), Int32::New(isolate, devmodePtr->dmPaperSize));
	obj->Set(String::NewFromUtf8(isolate, "copies"), Int32::New(isolate, devmodePtr->dmCopies));
	obj->Set(String::NewFromUtf8(isolate, "printQuality"), Int32::New(isolate, devmodePtr->dmPrintQuality));
	obj->Set(String::NewFromUtf8(isolate, "defaultSource"), Int32::New(isolate, devmodePtr->dmDefaultSource));
	args.GetReturnValue().Set(obj);
}

void parseDevnames(const v8::FunctionCallbackInfo<Value>& args){
	// parseDevnames(devnames)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsObject() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	Local<Object> devnamesBuffer = args[0]->ToObject();
	DEVNAMES *data = (DEVNAMES *)node::Buffer::Data(devnamesBuffer);
	WCHAR *driver, *device, *output;
	parse_devnames(data, &driver, &device, &output);
	Local<String> driverString = String::NewFromTwoByte(isolate, (const uint16_t *)driver);
	Local<String> deviceString = String::NewFromTwoByte(isolate, (const uint16_t *)device);
	Local<String> outputString = String::NewFromTwoByte(isolate, (const uint16_t *)output);
	Local<Object> obj = Object::New(isolate);
	obj->Set(String::NewFromUtf8(isolate, "driver"), driverString);
	obj->Set(String::NewFromUtf8(isolate, "device"), deviceString);
	obj->Set(String::NewFromUtf8(isolate, "output"), outputString);
	args.GetReturnValue().Set(obj);
}

void createDc(const v8::FunctionCallbackInfo<Value>& args){
	// createDc(devmode, devnames)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 2 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsObject() || !args[1]->IsObject() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	DEVMODEW *devmodePtr = (DEVMODEW *)node::Buffer::Data(args[0]->ToObject());
	DEVNAMES *devnamesPtr = (DEVNAMES *)node::Buffer::Data(args[1]->ToObject());
	WCHAR *driver, *device, *output;
	parse_devnames(devnamesPtr, &driver, &device, &output);
	HDC hdc = CreateDCW(driver, device, NULL, devmodePtr);
	args.GetReturnValue().Set(Int32::New(isolate, (int)hdc));
}

void deleteDc(const v8::FunctionCallbackInfo<Value>& args){
	// deleteDc(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	BOOL ok = DeleteDC(hdc);
	args.GetReturnValue().Set(ok);
}

void beginPrint(const v8::FunctionCallbackInfo<Value>& args){
	// beginPrint(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	DOCINFOW docinfo;
	ZeroMemory(&docinfo, sizeof(docinfo));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = L"drawer printing";
	int ret = StartDocW(hdc, &docinfo);
	if( ret <= 0 ){
		report_error(isolate, "StartDoc failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void endPrint(const v8::FunctionCallbackInfo<Value>& args){
	// endPrint(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = EndDoc(hdc);
	if( ret <= 0 ){
		report_error(isolate, "EndDoc failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void abortPrint(const v8::FunctionCallbackInfo<Value>& args){
	// abortPrint(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = AbortDoc(hdc);
	if( ret <= 0 ){
		report_error(isolate, "AbortDoc failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void startPage(const v8::FunctionCallbackInfo<Value>& args){
	// startPage(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = StartPage(hdc);
	if( ret <= 0 ){
		report_error(isolate, "StartPage failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void endPage(const v8::FunctionCallbackInfo<Value>& args){
	// endPage(hdc)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 1 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int ret = EndPage(hdc);
	if( ret <= 0 ){
		report_error(isolate, "EndPage failed");
		return;
	}
	args.GetReturnValue().Set(ret);
}

void moveTo(const v8::FunctionCallbackInfo<Value>& args){
	// moveTo(hdc, x, y)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 3 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long x = args[1]->Int32Value();
	long y = args[2]->Int32Value();
	BOOL ok = MoveToEx(hdc, x, y, NULL);
	if( !ok ){
		report_error(isolate, "MoveToEx failed");
		return;
	}
	args.GetReturnValue().Set(ok);
}

void lineTo(const v8::FunctionCallbackInfo<Value>& args){
	// lineTo(hdc, x, y)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 3 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long x = args[1]->Int32Value();
	long y = args[2]->Int32Value();
	BOOL ok = LineTo(hdc, x, y);
	if( !ok ){
		report_error(isolate, "LineTo failed");
		return;
	}
	args.GetReturnValue().Set(ok);
}

void textOut(const v8::FunctionCallbackInfo<Value>& args){
	// textOut(hdc, x, y, text)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 4 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() || !args[3]->IsString() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long x = args[1]->Int32Value();
	long y = args[2]->Int32Value();
	String::Value textValue(args[3]);
	
	BOOL ok = TextOutW(hdc, x, y, (LPWSTR)*textValue, textValue.length());
	if( !ok ){
		report_error(isolate, "TextOutW failed");
		return;
	}
	args.GetReturnValue().Set(ok);
}

void selectObject(const v8::FunctionCallbackInfo<Value>& args){
	// selectObject(hdc, handle)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 2 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	HANDLE obj = (HANDLE)args[1]->Int32Value();
	HANDLE prev = SelectObject(hdc, obj);
	if( prev == NULL || prev == HGDI_ERROR ){
		report_error(isolate, "SelectObject failed");
		return;
	}
	args.GetReturnValue().Set((int)prev);
}

void setTextColor(const v8::FunctionCallbackInfo<Value>& args){
	// setTextColor(hdc, r, g, b)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 4 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() || !args[3]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	long r = args[1]->Int32Value();
	long g = args[2]->Int32Value();
	long b = args[3]->Int32Value();
	COLORREF ret = SetTextColor(hdc, RGB(r, g, b));
	if( ret == CLR_INVALID ){
		report_error(isolate, "SetTextColor failed");
		return;
	}
	args.GetReturnValue().Set(TRUE);
}

void createPen(const v8::FunctionCallbackInfo<Value>& args){
	// createPen(width, r, g, b)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 4 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32() || !args[3]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	long width = args[0]->Int32Value();
	long r = args[1]->Int32Value();
	long g = args[2]->Int32Value();
	long b = args[3]->Int32Value();
	HPEN pen = CreatePen(PS_SOLID, width, RGB(r, g, b));
	if( pen == NULL ){
		report_error(isolate, "CreatePen failed");
		return;
	}
	args.GetReturnValue().Set((int)pen);
}

void setBkMode(const v8::FunctionCallbackInfo<Value>& args){
	// setBkMode(hdc, mode)
	Isolate *isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if( args.Length() < 2 ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong number of arguments"))
		);
		return;
	}	
	if( !args[0]->IsInt32() || !args[1]->IsInt32() ){
		isolate->ThrowException(
			Exception::TypeError(String::NewFromUtf8(isolate, "wrong arguments"))
		);
		return;
	}
	HDC hdc = (HDC)args[0]->Int32Value();
	int mode = args[1]->Int32Value();
	int prev = SetBkMode(hdc, mode);
	if( prev == 0 ){
		report_error(isolate, "SetBkMode failed");
		return;
	}
	args.GetReturnValue().Set(prev);
}

void Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  if( !initWindowClass() ){
  	isolate->ThrowException(Exception::Error(
  		String::NewFromUtf8(isolate, "RegisterClass failed.")
  	));
  	return;
  }
  exports->Set(String::NewFromUtf8(isolate, "createWindow"),
      FunctionTemplate::New(isolate, createWindow)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "disposeWindow"),
      FunctionTemplate::New(isolate, disposeWindow)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "getDc"),
      FunctionTemplate::New(isolate, getDc)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "releaseDc"),
      FunctionTemplate::New(isolate, releaseDc)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "measureText"),
      FunctionTemplate::New(isolate, measureText)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "createFont"),
      FunctionTemplate::New(isolate, createFont)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "deleteObject"),
      FunctionTemplate::New(isolate, deleteObject)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "getDpiOfHdc"),
      FunctionTemplate::New(isolate, getDpiOfHdc)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "printerDialog"),
      FunctionTemplate::New(isolate, printerDialog)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "parseDevmode"),
      FunctionTemplate::New(isolate, parseDevmode)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "parseDevnames"),
      FunctionTemplate::New(isolate, parseDevnames)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "createDc"),
      FunctionTemplate::New(isolate, createDc)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "deleteDc"),
      FunctionTemplate::New(isolate, deleteDc)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "beginPrint"),
      FunctionTemplate::New(isolate, beginPrint)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "endPrint"),
      FunctionTemplate::New(isolate, endPrint)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "abortPrint"),
      FunctionTemplate::New(isolate, abortPrint)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "startPage"),
      FunctionTemplate::New(isolate, startPage)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "endPage"),
      FunctionTemplate::New(isolate, endPage)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "moveTo"),
      FunctionTemplate::New(isolate, moveTo)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "lineTo"),
      FunctionTemplate::New(isolate, lineTo)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "textOut"),
      FunctionTemplate::New(isolate, textOut)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "selectObject"),
      FunctionTemplate::New(isolate, selectObject)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "setTextColor"),
      FunctionTemplate::New(isolate, setTextColor)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "createPen"),
      FunctionTemplate::New(isolate, createPen)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "setBkMode"),
      FunctionTemplate::New(isolate, setBkMode)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "bkModeOpaque"), Int32::New(isolate, OPAQUE));
  exports->Set(String::NewFromUtf8(isolate, "bkModeTransparent"), Int32::New(isolate, TRANSPARENT));
  exports->Set(String::NewFromUtf8(isolate, "FW_DONTCARE"), Int32::New(isolate, FW_DONTCARE));
  exports->Set(String::NewFromUtf8(isolate, "FW_BOLD"), Int32::New(isolate, FW_BOLD));
}

NODE_MODULE(drawer, Init)
