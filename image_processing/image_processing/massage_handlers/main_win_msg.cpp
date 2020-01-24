

// globals to chat between callback function
Window* main_window_ptr;
Image* image_ptr;

#define BUTTON_HIST 1

// main window callback from os
LRESULT CALLBACK main_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = 0;
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == BUTTON_HIST && hist_window_ptr == nullptr)
				hist_window_ptr = new Window(L"hist", L"histogram", 600, 400, DEF_STYLE, hist_callback, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), main_window_ptr->handle, &hist_window_ptr);
		
		}break;
	
		case WM_CREATE:
		case WM_SIZE:
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			main_window_ptr->canvas.resize(rect.right - rect.left, rect.bottom - rect.top);
		} break;
		case WM_DESTROY:
		case WM_CLOSE:
		{
			running = false;
		}break;
		case WM_PAINT:
		{
			PAINTSTRUCT decoy;
			BeginPaint(main_window_ptr->handle, &decoy);
		
			draw_image(main_window_ptr->canvas, *image_ptr, 0, 0, main_window_ptr->canvas.width, main_window_ptr->canvas.height);
			main_window_ptr->render_canvas(main_window_ptr->canvas);
		
			EndPaint(main_window_ptr->handle, &decoy);
		}break;
		default:
		{
			res = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}


	return res;
}

// main window massage handle, called from game loop
void main_process_msg(Window& window, Key_Input& keys, Mouse_Input& mouse)
{
	MSG msg;
	while (PeekMessage(&msg, window.handle, 0, 0, PM_REMOVE))
	{

		switch (msg.message)
		{
		case WM_MOUSEMOVE:
		{
			int x = LOWORD(msg.lParam);
			int y = HIWORD(msg.lParam);
			mouse.pos_x = float(x - window.canvas.width / 2) / window.canvas.height;
			mouse.pos_y = float(window.canvas.height - y - window.canvas.height / 2) / window.canvas.height;
		}break;
		case WM_LBUTTONDOWN:
		{
			mouse.buttons[LBUTTON].changed = mouse.buttons[LBUTTON].is_down ^ 1;
			mouse.buttons[LBUTTON].is_down = true;
		}break;
		case WM_LBUTTONUP:
		{
			mouse.buttons[LBUTTON].changed = mouse.buttons[LBUTTON].is_down ^ 0;
			mouse.buttons[LBUTTON].is_down = false;
		}break;
		case WM_KEYUP:
		case WM_KEYDOWN:
		{
			uint32_t vk_code = (uint32_t)msg.wParam;
			bool is_down = ((msg.lParam & (1 << 31)) == 0);

			switch (vk_code)
			{
				case VK_LEFT:
				{
					keys.buttons[BUTTON_LEFT].changed = keys.buttons[BUTTON_LEFT].is_down ^ is_down;
					keys.buttons[BUTTON_LEFT].is_down = is_down;
				}break;
				case VK_RIGHT:
				{
					keys.buttons[BUTTON_RIGHT].changed = keys.buttons[BUTTON_RIGHT].is_down ^ is_down;
					keys.buttons[BUTTON_RIGHT].is_down = is_down;
				}break;
				case VK_UP:
				{
					keys.buttons[BUTTON_UP].changed = keys.buttons[BUTTON_UP].is_down ^ is_down;
					keys.buttons[BUTTON_UP].is_down = is_down;
				}break;
				case VK_DOWN:
				{
					keys.buttons[BUTTON_DOWN].changed = keys.buttons[BUTTON_DOWN].is_down ^ is_down;
					keys.buttons[BUTTON_DOWN].is_down = is_down;
				}break;
				case VK_ESCAPE:
				{
					running = false;
				}break;
			}
		}break;
		}
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}