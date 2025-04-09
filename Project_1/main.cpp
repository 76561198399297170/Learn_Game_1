#include <graphics.h>
#include <string>
#include <vector>

bool is_running = true;
bool is_start_game = false;

const int WINDOWS_HEIGHT = 720;
const int WINDOWS_WIDTH = 1280;

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "MSIMG32.LIB")

int score = 0;

IMAGE img_background/*, img_gamemenu*/;
IMAGE img_ice;

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });

	return;
}

class Button
{
public:
	Button(RECT region, LPCTSTR path_idle, LPCTSTR path_hovered, LPCTSTR path_pushed)
	{
		this->m_region = region;

		loadimage(&this->m_img_idle, path_idle);
		loadimage(&this->m_img_hovered, path_hovered);
		loadimage(&this->m_img_pushed, path_pushed);
	}

	~Button() = default;

public:
	virtual void onClick() = 0;

	bool checkCursorHit(int x, int y)
	{
		return this->m_region.left < x && this->m_region.right > x && this->m_region.top < y && this->m_region.bottom > y;
	}

	void processEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (this->m_status == Status::Idle && this->checkCursorHit(msg.x, msg.y)) this->m_status = Status::Hovered;
			else if (this->m_status == Status::Hovered && !this->checkCursorHit(msg.x, msg.y)) this->m_status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (this->m_status == Status::Hovered) this->m_status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (this->m_status == Status::Pushed)
			{
				if (this->checkCursorHit(msg.x, msg.y))
				{
					this->m_status = Status::Hovered;
					this->onClick();
				}
				else
				{
					this->m_status = Status::Idle;
				}
			}
			break;
		default:
			break;
		}
	}

	void draw()
	{
		switch (this->m_status)
		{
		case Status::Idle:
			putimage_alpha(this->m_region.left, this->m_region.top, &this->m_img_idle);
			break;
		case Status::Hovered:
			putimage_alpha(this->m_region.left, this->m_region.top, &this->m_img_hovered);
			break;
		case Status::Pushed:
			putimage_alpha(this->m_region.left, this->m_region.top, &this->m_img_pushed);
			break;
		default:
			break;
		}
	}

public:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

public:
	RECT m_region;

	IMAGE m_img_idle;
	IMAGE m_img_hovered;
	IMAGE m_img_pushed;

	Status m_status = Status::Idle;

};

class StartGameButton : public Button
{
public:
	StartGameButton(RECT region, LPCTSTR path_idle, LPCTSTR path_hovered, LPCTSTR path_pushed) : Button(region, path_idle, path_hovered, path_pushed) {}

	~StartGameButton() = default;

public:
	void onClick()
	{
		is_start_game = true;
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}

};

class QuitGameButton : public Button
{
public:
	QuitGameButton(RECT region, LPCTSTR path_idle, LPCTSTR path_hovered, LPCTSTR path_pushed) : Button(region, path_idle, path_hovered, path_pushed) {}

	~QuitGameButton() = default;

public:
	void onClick()
	{
		is_running = false;
	}

};

class Atlas
{
public:
	Atlas() {}

	Atlas(LPCTSTR path, int num)
	{
		TCHAR path_file[256];
		for (size_t i = 1; i <= num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);

			this->m_frame_list.push_back(frame);
		}
	}

	~Atlas()
	{
		for (int i = 0; i < this->m_frame_list.size(); i++)
		{
			delete this->m_frame_list[i];
		}
	}

public:
	std::vector<IMAGE* > m_frame_list;

};

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;

class Animation
{
public:
	Animation(Atlas* atlas, int interval)
	{
		this->m_interval_ms = interval;
		this->m_anim_atlas = atlas;
	}

	~Animation() = default;

public:
	void play(int x, int y, int delta, int step = 1)
	{
		this->m_timer += delta;
		if (this->m_timer >= this->m_interval_ms)
		{
			this->m_idx_frame = (this->m_idx_frame + step) % this->m_anim_atlas->m_frame_list.size();
			this->m_timer = 0;
		}

		putimage_alpha(x, y, this->m_anim_atlas->m_frame_list[this->m_idx_frame]);
	}

	IMAGE* getCurrentFrame()
	{
		return this->m_anim_atlas->m_frame_list[this->m_idx_frame];
	}

	void restart()
	{
		this->m_idx_frame = 0;
	}

private:
	int m_timer = 0;
	int m_idx_frame = 0;
	int m_interval_ms;

private:
	Atlas* m_anim_atlas;

};

class Bullet
{
public:
	Bullet() = default;

	~Bullet() = default;

public:
	void drawBullet()
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));

		fillcircle(this->m_pos.x, this->m_pos.y, this->REDIUS);
	}

public:
	POINT m_pos = { 0, 0 };

private:
	const int REDIUS = 10;

};

class Player
{
public:
	Player()
	{
		loadimage(&this->m_img_shadow, _T("./img/player_shadow.png"));

		this->m_anim_left_player = new Animation(atlas_player_left, 90);
		this->m_anim_right_player = new Animation(atlas_player_right, 90);
	}

	~Player()
	{
		delete this->m_anim_left_player;
		delete this->m_anim_right_player;
	}

public:
	void processEvent(const ExMessage& msg)
	{
		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
			case 'E':
				this->cold();
				break;
			case VK_UP:
			case 'W':
				this->is_move_up = true;
				break;
			case VK_DOWN:
			case 'S':
				this->is_move_down = true;
				break;
			case VK_LEFT:
			case 'A':
				this->is_move_left = true;
				break;
			case VK_RIGHT:
			case 'D':
				this->is_move_right = true;
				break;
			}
		}
		if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
			case 'W':
				this->is_move_up = false;
				break;
			case VK_DOWN:
			case 'S':
				this->is_move_down = false;
				break;
			case VK_LEFT:
			case 'A':
				this->is_move_left = false;
				break;
			case VK_RIGHT:
			case 'D':
				this->is_move_right = false;
				break;
			}
		}
	}

	void move()
	{
		if (this->is_frozen)
		{
			return;
		}

		int dir_x = this->is_move_right - this->is_move_left;
		int dir_y = this->is_move_down - this->is_move_up;
		double dir_len = sqrt(pow(dir_x, 2) + pow(dir_y, 2));

		if (dir_len != 0)
		{
			double normalized_x = dir_x / dir_len;
			double normalized_y = dir_y / dir_len;

			this->m_player_pos.x += (int)(this->PLAYER_SPEED * normalized_x);
			this->m_player_pos.y += (int)(this->PLAYER_SPEED * normalized_y);
		}
		this->m_player_pos.x = (this->m_player_pos.x < 0 ? 0 : (this->m_player_pos.x + this->PLAYER_WIDTH > WINDOWS_WIDTH ? WINDOWS_WIDTH - this->PLAYER_WIDTH : this->m_player_pos.x));
		this->m_player_pos.y = (this->m_player_pos.y < 0 ? 0 : (this->m_player_pos.y + this->PLAYER_HEIGHT > WINDOWS_HEIGHT ? WINDOWS_HEIGHT - this->PLAYER_HEIGHT : this->m_player_pos.y));
	}

	void draw(int delta)
	{
		int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH - this->SHADOW_WIDTH) / 4;
		int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT - 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		int step = 2;
		if (this->is_hurt)
		{
			this->m_sketch_time -= delta;
			if (this->m_sketch_time <= 0)
			{
				this->is_hurt = false;
				this->m_anim_left_player->restart();
				this->m_anim_right_player->restart();
			}
			step = (this->m_sketch_time <= 0 ? 2 : 1);
		}

		if (this->is_frozen)
		{
			this->m_frozen_time -= delta;
			if (this->m_frozen_time <= 0)
			{
				this->is_frozen = false;
			}
		}

		IMAGE img_current_frame;

		static bool facing_left = false;
		int dir_x = this->is_move_right - this->is_move_left;
		facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));

		if (this->is_frozen)
		{
			img_current_frame = IMAGE(facing_left ? *this->m_anim_left_player->getCurrentFrame() : *this->m_anim_right_player->getCurrentFrame());
			int width = img_current_frame.getwidth();
			int height = img_current_frame.getheight();

			DWORD* color_buffer_frame_img = GetImageBuffer(&img_current_frame);
			DWORD* color_buffer_ice_img = GetImageBuffer(&img_ice);

			this->m_height_light_y = ((this->FROZEN_TIME - this->m_frozen_time) / 45) * this->THICKNESS % height;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = y * width + x;
					DWORD color_frame_img = color_buffer_frame_img[idx];
					DWORD color_ice_img = color_buffer_ice_img[idx];
					if ((color_frame_img & 0xff000000) >> 24)
					{
						BYTE r = GetBValue(color_frame_img) * this->RATIO + GetBValue(color_ice_img) * (1 - this->RATIO);
						BYTE g = GetGValue(color_frame_img) * this->RATIO + GetGValue(color_ice_img) * (1 - this->RATIO);
						BYTE b = GetRValue(color_frame_img) * this->RATIO + GetRValue(color_ice_img) * (1 - this->RATIO);

						if ((y >= this->m_height_light_y && y <= this->m_height_light_y + this->THICKNESS)
							&& (r / 255.0f) * 0.2126f + (g / 255.0f) * 0.7152f + (b / 255.0f) * 0.0722f >= this->THRESHOLD)
						{
							color_buffer_frame_img[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);
							continue;
						}
						color_buffer_frame_img[idx] = BGR(RGB(r, g, b)) | (((DWORD)(BYTE)(255)) << 24);
					}
				}
			}
			putimage_alpha(this->m_player_pos.x, this->m_player_pos.y, &img_current_frame);
		}
		else
		{
			facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta, step) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta, step);
		}
	}

	void cold()
	{
		if (!this->is_frozen)
		{
			score += 5;
			this->m_height_light_y = 0;
			this->is_frozen = true;
			this->m_frozen_time = this->FROZEN_TIME;
		}
	}

	int hurt()
	{
		if (this->m_sketch_time <= 0)
		{
			this->is_frozen = false;
			this->is_hurt = true;
			this->m_sketch_time = this->HURT_TIME;

			return this->m_heart--;
		}

		return 1;
	}

	const POINT& getPosition() const
	{
		return this->m_player_pos;
	}

	const int getFrameWidth() const
	{
		return this->PLAYER_WIDTH;
	}

	const int getFrameHeight() const
	{
		return this->PLAYER_HEIGHT;
	}

private:
	const int HURT_TIME = 600;
	const int FROZEN_TIME = 3000;

	const int THICKNESS = 5;
	const float RATIO = 0.25f;//混叠比例
	const float THRESHOLD = /*0.84f*/0.76f;//高亮阈值

	const int PLAYER_SPEED = 3;

	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;

private:
	int m_heart = 1;

	POINT m_player_pos = { 500, 500 };

	IMAGE m_img_shadow;
	Animation* m_anim_left_player;
	Animation* m_anim_right_player;

	bool is_frozen = false;
	int m_frozen_time = 0;
	int m_height_light_y = 0;

	bool is_hurt = false;
	int m_sketch_time = 0;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&this->m_img_shadow, _T("./img/enemy_shadow.png"));

		this->m_anim_left_enemy = new Animation(atlas_enemy_left, 90);
		this->m_anim_right_enemy = new Animation(atlas_enemy_right, 90);

		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		{
		case SpawnEdge::Up:
			this->m_enemy_pos = { rand() % (WINDOWS_WIDTH - this->ENEMY_WIDTH), -this->ENEMY_HEIGHT };
			break;
		case SpawnEdge::Down:
			this->m_enemy_pos = { rand() % (WINDOWS_WIDTH - this->ENEMY_WIDTH), WINDOWS_HEIGHT };
			break;
		case SpawnEdge::Left:
			this->m_enemy_pos = { -this->ENEMY_WIDTH, rand() % (WINDOWS_HEIGHT - this->ENEMY_HEIGHT) };
			break;
		case SpawnEdge::Right:
			this->m_enemy_pos = { WINDOWS_WIDTH, rand() % (WINDOWS_HEIGHT - this->ENEMY_HEIGHT) };
			break;
		}
	}

	~Enemy()
	{
		delete this->m_anim_left_enemy;
		delete this->m_anim_right_enemy;
	}

public:
	bool checkPlayerCollision(const Player& player)
	{
		const POINT& player_pos = player.getPosition();

		return this->m_enemy_pos.x + this->ENEMY_WIDTH / 2 >= player_pos.x && this->m_enemy_pos.x + this->ENEMY_WIDTH / 2 <= player_pos.x + player.getFrameWidth()
			&& this->m_enemy_pos.y + this->ENEMY_HEIGHT / 2 >= player_pos.y && this->m_enemy_pos.y + this->ENEMY_HEIGHT / 2 <= player_pos.y + player.getFrameHeight();
	}

	bool checkBulletCollision(const Bullet& bullet)
	{
		return bullet.m_pos.x >= this->m_enemy_pos.x && bullet.m_pos.x <= this->m_enemy_pos.x + this->ENEMY_WIDTH
			&& bullet.m_pos.y >= this->m_enemy_pos.y && bullet.m_pos.y <= this->m_enemy_pos.y + this->ENEMY_HEIGHT;
	}

	void move(const Player& player)
	{
		const POINT& player_pos = player.getPosition();

		int dir_x = player_pos.x - this->m_enemy_pos.x;
		int dir_y = player_pos.y - this->m_enemy_pos.y;
		double dir_len = sqrt(pow(dir_x, 2) + pow(dir_y, 2));

		if (dir_len != 0)
		{
			double normalized_x = dir_x / dir_len;
			double normalized_y = dir_y / dir_len;

			this->m_enemy_pos.x += (int)(this->ENEMY_SPEED * normalized_x);
			this->m_enemy_pos.y += (int)(this->ENEMY_SPEED * normalized_y);
		}
	}

	void draw(int delta)
	{
		if (this->m_hit_time > 0)
		{
			this->m_hit_time -= delta;
		}

		int shadow_pos_x = this->m_enemy_pos.x + (this->ENEMY_WIDTH - this->SHADOW_WIDTH) / 4;
		int shadow_pos_y = this->m_enemy_pos.y + this->ENEMY_HEIGHT - 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		this->m_face_left ? this->m_anim_left_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta) : this->m_anim_right_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta);
		setfillcolor(RED);
		setlinecolor(RED);
		fillrectangle(shadow_pos_x, shadow_pos_y + 5, shadow_pos_x + this->SHADOW_WIDTH * this->m_health, shadow_pos_y + 10);
	}

	void hurt()
	{
		if (this->m_hit_time <= 0)
		{
			this->m_hit_time = this->HIT_TIME;
			this->m_health--;
			if (this->m_health == 0)
			{
				this->is_alive = false;
			}
		}
	}

	void kill()
	{
		this->m_health = 0;
		this->is_alive = false;
	}

	bool checkAlive()
	{
		return this->is_alive;
	}

private:
	const int HIT_TIME = 200;

	const int ENEMY_SPEED = 2;

	const int ENEMY_WIDTH = 80;
	const int ENEMY_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;

private:
	int m_health = 2;
	int m_hit_time = 0;
	
	bool is_alive = true;

	POINT m_enemy_pos;

	IMAGE m_img_shadow;
	Animation* m_anim_left_enemy;
	Animation* m_anim_right_enemy;

	bool m_face_left = false;

};

void tryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if (++counter % INTERVAL == 0)
	{
		enemy_list.push_back(new Enemy());
	}
}

void updataBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045;
	const double TANGENT_SPEED = 0.0055;

	double radian_interval = 2 * 3.141593 / bullet_list.size();
	const POINT& player_pos = player.getPosition();

	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);

	for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;

		bullet_list[i].m_pos.x = player_pos.x + player.getFrameWidth() / 2 + (int)(radius * sin(radian));
		bullet_list[i].m_pos.y = player_pos.y + player.getFrameHeight() / 2 + (int)(radius * cos(radian));
	}
}

void drawPlayerScore(int score)
{
	setbkmode(TRANSPARENT);
	settextcolor(RGB(225, 85, 185));
	outtextxy(10, 10, (L"当前玩家得分: " + std::to_wstring(score)).c_str());
}

void loadAtlas()
{
	atlas_enemy_right = new Atlas();
	atlas_player_right = new Atlas();
	atlas_enemy_left = new Atlas(L"./img/enemy_left_%d.png", 3);
	atlas_player_left = new Atlas(L"./img/player_left_%d.png", 4);

	IMAGE* img_enemy_right;
	IMAGE* img_player_right;

	for (int i = 0; i < 3; i++)
	{
		img_enemy_right = new IMAGE();

		int width = atlas_enemy_left->m_frame_list[i]->getwidth();
		int height = atlas_enemy_left->m_frame_list[i]->getheight();
		Resize(img_enemy_right, width, height);

		DWORD* color_buffer_left_image = GetImageBuffer(atlas_enemy_left->m_frame_list[i]);
		DWORD* color_buffer_right_image = GetImageBuffer(img_enemy_right);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				color_buffer_right_image[y * width + width - x - 1] = color_buffer_left_image[y * width + x];
			}
		}
		atlas_enemy_right->m_frame_list.push_back(img_enemy_right);
	}

	for (int i = 0; i < 4; i++)
	{
		img_player_right = new IMAGE();

		int width = atlas_player_left->m_frame_list[i]->getwidth();
		int height = atlas_player_left->m_frame_list[i]->getheight();
		Resize(img_player_right, width, height);

		DWORD* color_buffer_left_image = GetImageBuffer(atlas_player_left->m_frame_list[i]);
		DWORD* color_buffer_right_image = GetImageBuffer(img_player_right);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				color_buffer_right_image[y * width + width - x - 1] = color_buffer_left_image[y * width + x];
			}
		}
		atlas_player_right->m_frame_list.push_back(img_player_right);
	}
}

void loadSketch()
{
	std::vector<IMAGE* > new_left_frame_list;
	std::vector<IMAGE* > new_right_frame_list;

	IMAGE* img_left_sketch;
	IMAGE* img_right_sketch;
	for (int i = 0; i < 4; i++)
	{
		img_left_sketch = new IMAGE();

		int width = atlas_player_left->m_frame_list[i]->getwidth();
		int height = atlas_player_left->m_frame_list[i]->getheight();
		Resize(img_left_sketch, width, height);

		DWORD* color_buffer_left_image = GetImageBuffer(atlas_player_left->m_frame_list[i]);
		DWORD* color_buffer_left_sketch = GetImageBuffer(img_left_sketch);
		for (int j = 0; j < width * height; j++)
		{
			color_buffer_left_sketch[j] = ((color_buffer_left_image[j] & 0xff000000) >> 24 == 0 ? color_buffer_left_image[j] : BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24));
		}
		new_left_frame_list.push_back(atlas_player_left->m_frame_list[i]);
		new_left_frame_list.push_back(img_left_sketch);
	}
	atlas_player_left->m_frame_list = new_left_frame_list;
	for (int i = 0; i < 4; i++)
	{
		img_right_sketch = new IMAGE();

		int width = atlas_player_right->m_frame_list[i]->getwidth();
		int height = atlas_player_right->m_frame_list[i]->getheight();
		Resize(img_right_sketch, width, height);

		DWORD* color_buffer_right_image = GetImageBuffer(atlas_player_right->m_frame_list[i]);
		DWORD* color_buffer_right_sketch = GetImageBuffer(img_right_sketch);
		for (int j = 0; j < width * height; j++)
		{
			color_buffer_right_sketch[j] = ((color_buffer_right_image[j] & 0xff000000) >> 24 == 0 ? color_buffer_right_image[j] : BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24));
		}
		new_right_frame_list.push_back(atlas_player_right->m_frame_list[i]);
		new_right_frame_list.push_back(img_right_sketch);
	}
	atlas_player_right->m_frame_list = new_right_frame_list;
}

void addBullet(std::vector<Bullet>& bullet_list)
{
	if (bullet_list.size() <= 9 && bullet_list.size() != score / 25 + 2)
	{
		bullet_list.push_back(Bullet());
	}
}

int main()
{
	initgraph(1280, 720);

	RECT region_btn_start_game, region_btn_quit_game;

	const int BUTTON_WIDTH = 128;
	const int BUTTON_HEIGHT = 32;

	region_btn_start_game.top = (WINDOWS_HEIGHT - BUTTON_HEIGHT) / 2;
	region_btn_start_game.left = (WINDOWS_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
	region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

	region_btn_quit_game.top = (WINDOWS_HEIGHT - BUTTON_HEIGHT) / 2 - BUTTON_HEIGHT * 1.5;
	region_btn_quit_game.left = (WINDOWS_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;
	region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT;

	StartGameButton btn_start_game(region_btn_start_game, L"./img/start_game_button_idle.png", L"./img/start_game_button_hovered.png", L"./img/start_game_button_pushed.png");
	QuitGameButton btn_quit_game(region_btn_quit_game, L"./img/quit_game_button_idle.png", L"./img/quit_game_button_hovered.png", L"./img/quit_game_button_pushed.png");

	loadAtlas();
	loadSketch();

	mciSendString(_T("open ./mus/background_music.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open ./mus/hit_music.mp3 alias hit"), NULL, 0, NULL);

	ExMessage msg;

	Player player;
	std::vector<Bullet> bullet_list(2);
	std::vector<Enemy*> enemy_list;

	loadimage(&img_ice, _T("./img/ice.png"));
	loadimage(&img_background, _T("./img/background.png"));
	/*loadimage(&img_gamemenu, _T("./img/gamemenu.png"));*/

	BeginBatchDraw();

	while (is_running)
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			if (is_start_game)
			{
				player.processEvent(msg);
			}
			else
			{
				btn_quit_game.processEvent(msg);
				btn_start_game.processEvent(msg);
			}
		}

		if (is_start_game)
		{
			player.move();
			tryGenerateEnemy(enemy_list);

			updataBullets(bullet_list, player);

			for (Enemy* i : enemy_list)
			{
				i->move(player);
				if (i->checkPlayerCollision(player))
				{
					i->kill();
					if (!player.hurt())
					{
						MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
						is_running = false;
						break;
					}
				}

				for (const Bullet& j : bullet_list)
				{
					if (i->checkBulletCollision(j))
					{
						i->hurt();
					}
				}
			}

			for (size_t i = 0; i < enemy_list.size(); i++)
			{
				Enemy* ey = enemy_list[i];
				if (!ey->checkAlive())
				{
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete ey;

					mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					score++;
				}
			}
		}

		addBullet(bullet_list);

		cleardevice();

		if (is_start_game)
		{
			putimage(0, 0, &img_background);
			for (Enemy* i : enemy_list) i->draw(1000 / 144);
			player.draw(1000 / 144);
			for (Bullet& i : bullet_list) i.drawBullet();
			drawPlayerScore(score);
		}
		else
		{
			btn_quit_game.draw();
			btn_start_game.draw();
		}

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}

	}

	delete atlas_enemy_left;
	delete atlas_enemy_right;
	delete atlas_player_left;
	delete atlas_player_right;

	mciSendString(_T("close bgm"), NULL, 0, NULL);
	mciSendString(_T("close hit"), NULL, 0, NULL);
	EndBatchDraw();

	return 0;
}