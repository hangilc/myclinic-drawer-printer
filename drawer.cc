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
}

NODE_MODULE(drawer, Init)


