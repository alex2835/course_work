
#define CHECK_INPUT 10

struct vec2i
{
	int x, y;

	vec2i(int x, int y) : x(x), y(y) {}
};

struct Screen_img : public Image
{
	vec2i left;
	vec2i right;
	bool on_screen;
	bool clicked;

	Screen_img(std::wstring name) : Image(name.c_str()), clicked(false), on_screen(false), left({0, 0}), right({0, 0}) {}

	bool if_click(vec2i cl_pos)
	{
		return clicked = (on_screen && cl_pos.x > left.x&& cl_pos.y > left.y&& cl_pos.x < right.x && cl_pos.y < right.y);
	}
};


void draw_screen_image(Canvas& surface, Screen_img& image,
	float fpos_x, float fpos_y, float fwidth, float fheight)
{
	image.on_screen = true;
	image.left = vec2i(fpos_x * surface.width, fpos_y * surface.height);
	image.right= vec2i((fpos_x + fwidth) * surface.width, (fpos_y + fheight) * surface.height);

	draw_image(surface, image, fpos_x, fpos_y, fwidth, fheight);
}

struct Main_window : public Window
{
	std::vector<Screen_img> images;

	Main_window(std::wstring dir_path)
	{
		images.reserve(10);
		for (const auto& entry : fs::directory_iterator(dir_path))
		{
			std::filesystem::path path = entry.path();
			images.push_back(std::move(Screen_img(path.c_str())));
		}

		init(L"main window", 800, 600, DEF_STYLE | WS_VSCROLL, NULL, NULL, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
		{
			args arg = arguments.get(hwnd);
			Main_window* window = (Main_window*)arg[0];
			if (!window) return DefWindowProc(hwnd, msg, wParam, lParam);

			switch (msg)
			{
				case WM_SIZE:
				{
					window->canvas.resize(hwnd);
				}break;
				case WM_PAINT:
				{
					PAINTSTRUCT plug;
					BeginPaint(hwnd, &plug);

					draw_filled_rect(window->canvas, 0, 0, window->canvas.width, window->canvas.height, Color(255, 255, 255));


					for (auto& image : window->images)
							image.on_screen = false;
					
					for (int i = 0; i < window->images.size(); i++)
					{
						int x = i % 5;
						int y = i / 5;
						draw_screen_image(window->canvas, window->images[y * 5 + x], 0.04f + x * 0.19f, 0.8f - 0.2f * y, 0.15f, 0.15f);
					}

					window->render_canvas();
					EndPaint(hwnd, &plug);
					return 0;
				}break;
				case WM_LBUTTONDOWN:
				{
					int xPos = LOWORD(lParam);
					int yPos = window->canvas.height - HIWORD(lParam);

					for (auto& img : window->images)
						img.if_click({ xPos, yPos });

				}break;
				case CHECK_INPUT:
				{
					for (auto& image : window->images)
					{
						if (image.clicked)
						{
							new Image_window(image);
							image.clicked = false;
						}
					}
					return 0;
				}
				case WM_CLOSE:
				{
					running = false;
				}break;
			}

			return DefWindowProc(hwnd, msg, wParam, lParam);
		});
	}
};