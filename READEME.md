# 学习记录

**总计大约900行代码**

## 第一节

### 存储图片资源IMAGE与加载图片loadimage

EasyX中使用IMAGE存储图片，使用loadimage()函数加载图片

```
void loadimage(
	IMAGE* pDstImg, //存储图片变量的指针
	LPCTSTR pImgFile, //图片文件地址
	int nWidth = 0, //缩放属性暂时忽略
	int nHeight = 0, //缩放属性暂时忽略
	bool bResize = false //缩放自适应暂时忽略
)

使用用例1：
IMAGE img;
loadimage(&img, _T("test.jpg"));

使用用例2：
IMAGE img;
loadimage(&img, L"test.jpg");

使用用例3：
int num = 1;
string path = "./test_%d.png";

int idx = path.rfind("%d");
path.repalce(path.begin() + idx, paht.begin() + idx + 2, to_string(num));

IMAGE img
loadimage(&img, wstring(path.begin(), path.end()).c_str());

```



### 图片渲染方法putimage()

EasyX中提供putimage()函数再当前设备上绘制图片

```
void putimage(
	int dstX, //绘制坐标X
	int dstY, //绘制坐标Y
	IMAGE* pSrcImg, //图片地址
	DWORD dwRop = SRCCOPY //三元光栅操作码忽略
)
注意绘制坐标是图片左上角坐标

使用用例1：
IMAGE img;
loadimage(&img, _T("test.jpg"));
putimage(100, 200, &img);//绘制在100, 200

```



### 简单的动画序列帧切换与动画加载

定义全局变量idx_current_anim存储当前动画序列帧索引以及counter当前游戏帧，假设每五个游戏帧切换一次，若动画长度为常数6则代码则可编写以下内容

```
//全局变量
int idx_current_anim = 0;
int counter = 0;
const int PLAYER_ANIM_NUM = 6;

//主循环中
if (++counter % 5 == 0) idx_current_num++;
idx_current_anim %= PLAYER_ANIM_NUM;

如果乐意当然可以放进主循环中定义，但是注意使用static保证初始化仅一次

```



### 渲染图象透明度信息

只能自行封装函数例如下

```
#pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });

	return;
}

```



### 基本的角色移动

定义POINT类型变量并渲染玩家位置为定义的位置，并根据按键修改坐标即可，注意由于按键按下产生WM_KEYDOWN一次后会产生间隔才会继续持续产生WM_KEYDOWN，且WM_KEYDOWN产生与主循环异步进行导致处理数量与操作系统与硬件相关，所以改为布尔变量判断是否触发移动

```
//全局定义变量
POINT player_pos = { 500, 500 };

bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;

//主循环事件处理部分
while (peekmessage(&msg))
{
    if (msg.message == WM_KEYDOWN)
    {
        switch (msg.vkcode)
        {
        case VK_UP:
            is_move_up = true;
            break;
        case VK_DOWN:
            is_move_down = true;
            break;
        case VK_LEFT:
            is_move_left = true;
            break;
        case VK_RIGHT:
            is_move_right = true;
            break;
        }
    }
    if (msg.message == WM_KEYUP)
    {
        switch (msg.vkcode)
        {
        case VK_UP:
            is_move_up = false;
            break;
        case VK_DOWN:
            is_move_down = false;
            break;
        case VK_LEFT:
            is_move_left = false;
            break;
        case VK_RIGHT:
            is_move_right = false;
            break;
        }
    }
}

if (is_move_up) player_pos.y -= PLAYER_SPEED;
if (is_move_down) player_pos.y += PLAYER_SPEED;
if (is_move_left) player_pos.x -= PLAYER_SPEED;
if (is_move_right) player_pos.x += PLAYER_SPEED;

//主循环渲染处
putimage(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);
```



## 第一节代码完成展示

**main.cpp**

```
#include <graphics.h>
#include <string>

int idx_current_anim = 0;

const int PLAYER_ANIM_NUM = 6;
const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

POINT player_pos = { 500,500 };
bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;


#pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });

	return;
}

void loadAnimation()
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"./img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}

	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"./img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}

	return;
}

int main()
{
	initgraph(1280, 720);

	bool running = true;

	ExMessage msg;
	IMAGE img_background;

	loadimage(&img_background, _T("./img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					break;
				case VK_RIGHT:
					is_move_right = true;
					break;
				}
			}
			if (msg.message == WM_KEYUP)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				}
			}
		}

		if (is_move_up) player_pos.y -= PLAYER_SPEED;
		if (is_move_down) player_pos.y += PLAYER_SPEED;
		if (is_move_left) player_pos.x -= PLAYER_SPEED;
		if (is_move_right) player_pos.x += PLAYER_SPEED;

		cleardevice();

		putimage(0, 0, &img_background);
		putimage_alpha(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);

		static int counter = 0;
		if (++counter % 5 == 0) idx_current_anim++;
		idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;


		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);    
		}

	}

	EndBatchDraw();

	return 0;
}

```



## 第二节

### 面向对象特性“封装”——封装动画类

对于玩家的左右分别需要编写一对相同的加载与绘制，如果有更多的对象则需要做更多重复代码造成代码冗余，所以可以考虑对对象进行封装，定义Animation类封装动画相关数据与逻辑，考虑有哪些功能与数据需要放在类内部

使用vector存储动画帧序列且定义为私有成员，且构造时加载动画，使用参数为图片地址，图片数量与帧间隔，虽然vector存储地址比较危险可能出现拷贝构造的情况，但是本次暂不涉及此部分优化，通过构造时new出的IMAGE存储动画帧，在析构时对其内存释放delete

```
class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		this->m_interval_ms = interval;
		
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);

			this->m_frame_list.push_back(frame);
		}
	}

	~Animation()
	{
		for (int i = 0; i < this->m_frame_list.size(); i++)
		{
			delete this->m_frame_list[i];
		}
	}

private:
	int m_interval_ms;
	std::vector<IMAGE* > m_frame_list;

};
```



### 计数器转计时器思想

原本的计数器控制播放动画帧改变为计时器，通过传入上次调用时间更新帧来计时确定动画，如此原本的动画更新速度不再受到硬件与操作系统处理速度的影响导致动画播放时快时慢

以下对应修改绘制功能

```
//Animation成员函数
void play(int x, int y, int delta)
{
    this->m_timer += delta;
    if (this->m_timer >= this->m_interval_ms)
    {
        this->m_idx_frame = (this->m_idx_frame + 1) % this->m_frame_list.size();
        this->m_timer = 0;
    }

    putimage_alpha(x, y, this->m_frame_list[this->m_idx_frame]);
}

//Animation成员变量
int m_timer = 0;
int m_idx_frame = 0;

//全局加载动画帧修改如下
Animation anim_left_player(L"./img/player_left_%d.png", 6, 45);
Animation anim_right_player(L"./img/player_right_%d.png", 6, 45);

//全局函数绘制动画帧修改如下代码
void drawPlayer(int delta, int dir_x)
{
    static bool facing_left = false;
    facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));
    
    facing_left ? anim_left_player.play(player_pos.x, player_pos.y, delta) : anim_right_player.play(player_pos.x, player_pos.y, delta);
}

```



增加阴影绘制强调玩家位置，只需要提前加载完成后对应玩家位置绘制即可

```
//全局变量
IMAGE img_shadow;

//加载背景时同样提前加载好阴影
loadimage(&img_shadow, _T("./img/shadow_player.png"));

//全局声明变量存储玩家大小
const int PLAYER_WIDTH = 80;
const int PLAYER_HEIGHT = 80;
const int SHADOW_WIDTH = 32;

//drawPlayer()中先绘制阴影，位置居中向下偏移8像素

int shadow_pos_x = player_pos.x + (PLAYER_WIDTH + SHADOW_WIDTH) / 2;
int shadow_pos_y = player_pos.y + PLAYER_HEIGHT + 8;
putimage_alpha(shadow_pos_x, shadow_pos_y, &img_shadow);

//主循环putimage_alpha()修改为drawPlayer()
drawPlayer(1000 / 144, is_move_right - is_move_left);

```



### 速度斜向加速问题

简单对按下按键后方向位置增加，当同时按下两个按键时会导致斜向移动同时增加了两个方向单位距离，由勾股定理可得斜向移动了根号二倍距离，单纯使用if...else...修改过于繁杂，所以使用向量的思想处理，求向量方向单位向量即可

```
//主循环原坐标更新修改
int dir_x = is_move_right - is_move_left;
int dir_y = is_move_down - is_move_up;
double dir_len = sqrt(pow(dir_x, 2) + pow(dir_y, 2));

if (dir_len != 0)
{
	double normalized_x = dir_x / dir_len;
	double normalized_y = dir_y / dir_len;

	player_pos.x += (int)(PLAYER_SPEED * normalized_x);
	player_pos.y += (int)(PLAYER_SPEED * normalized_y);
}

```



注意玩家位置需要置于窗口内部，由此修正玩家坐标定义窗口大小变量且在更新位置后完成

```
//全局变量常量声明
const int WINDOWS_HEIGHT = 1280;
const int WINDOWS_WIDTH = 720;

//主循环更新坐标后修正玩家位置
player_pos.x = (player_pos.x < 0 ? 0 : (player_pos.x + PLAYER_WIDTH > WINDOWS_WIDTH ? WINDOWS_WIDTH - PLAYER_WIDTH : player_pos.x));
player_pos.y = (player_pos.y < 0 ? 0 : (player_pos.y + PLAYER_HEIGHT > WINDOWS_HEIGHT ? WINDOWS_HEIGHT - PLAYER_HEIGHT : player_pos.y));

```



### 面向对象特性“封装”——封装玩家类、敌人类与子弹类

由于敌人与玩家动画与绘制等内容混杂是不好的，所以可以将玩家类进行封装到Player对象中，地人类封装到Enemy类中，虽然也可以抽象更底层的Character角色或GameObeject对象来继承与多态，但会涉及到面向对象中除去封装的其他特性后续在做讨论

**玩家类**

```
//将玩家变量与相关函数存入Player类中
class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("./img/player_shadow.png"));
		
		this->m_anim_left_player = new Animation(L"./img/player_left_%d.png", 6, 45);
		this->m_anim_right_player = new Animation(L"./img/player_right_%d.png", 6, 45);
	}

	~Player()
	{
		delete this->m_anim_left_player;
		delete this->m_anim_right_player;
	}

private:
	const int PLAYER_SPEED = 3;

	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;

private:
	POINT m_player_pos = { 500,500 };

	IMAGE m_img_shadow;
	Animation* m_anim_left_player;
	Animation* m_anim_right_player;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

};

//成员函数processEvent()处理输入消息
void processEvent(const ExMessage& msg)
{
	if (msg.message == WM_KEYDOWN)
	{
		switch (msg.vkcode)
		{
		case VK_UP:
			this->is_move_up = true;
			break;
		case VK_DOWN:
			this->is_move_down = true;
			break;
		case VK_LEFT:
			this->is_move_left = true;
			break;
		case VK_RIGHT:
			this->is_move_right = true;
			break;
		}
	}
	if (msg.message == WM_KEYUP)
	{
		switch (msg.vkcode)
		{
		case VK_UP:
			this->is_move_up = false;
			break;
		case VK_DOWN:
			this->is_move_down = false;
			break;
		case VK_LEFT:
			this->is_move_left = false;
			break;
		case VK_RIGHT:
			this->is_move_right = false;
			break;
		}
	}
}

//成员函数move()处理角色移动
void move()
{
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

//成员函数draw()处理角色绘制
void draw(int delta)
{
	int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH + this->SHADOW_WIDTH) / 2;
	int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

	putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

	static bool facing_left = false;
	int dir_x = this->is_move_right - this->is_move_left;
	facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));

	facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta);
}

```



**子弹类**

```
//子弹类比较简单，主要是绘制与坐标
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

```



**敌人类**

```
//类比玩家类实现敌人类
class Enemy
{
public:
	Enemy()
	{
		loadimage(&this->m_img_shadow, _T("./img/player_shadow.png"));

		this->m_anim_left_enemy = new Animation(L"./img/enemy_left_%d.png", 6, 45);
		this->m_anim_right_enemy = new Animation(L"./img/enemy_right_%d.png", 6, 45);
	}

	~Enemy()
	{
		delete this->m_anim_left_enemy;
		delete this->m_anim_right_enemy;
	}

public:


private:
	const int ENEMY_SPEED = 2;

	const int ENEMY_WIDTH = 80;
	const int ENEMY_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;

private:
	POINT m_enemy_pos = { 500, 500 };

	IMAGE m_img_shadow;
	Animation* m_anim_left_enemy;
	Animation* m_anim_right_enemy;

	bool m_face_left = false;

};

//生成敌人坐标在构造函数中使用枚举类随机获取边界类型
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

//成员函数checkPlayerCollision()检查是否与玩家发生碰撞，暂时返回false
bool checkPlayerCollision(const Player& player)
{
	return false;
}

//成员函数checkBulletCollision()检查是否与子弹发生碰撞，暂时返回false
bool checkBulletCollision(const Bullet& bullet)
{
	return false;
}

//成员函数move()跟踪玩家位置移动
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

//成员函数draw()绘制方法
void draw(int delta)
{
	int shadow_pos_x = this->m_enemy_pos.x + (this->ENEMY_WIDTH + this->SHADOW_WIDTH) / 2;
	int shadow_pos_y = this->m_enemy_pos.y + this->ENEMY_HEIGHT + 8;

	putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

	this->m_face_left ? this->m_anim_left_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta) : this->m_anim_right_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta);
}

//全局函数尝试创建Enemy对象存储
void tryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if (++counter % INTERVAL == 0)
	{
		enemy_list.push_back(new Enemy());
	}
}

//创建std::vector<Enemy*>类型敌人数组
std::vector<Enemy*> enemy_list;

//主循环中修改原本的方法
while (peekmessage(&msg))
{
	player.processEvent(msg);
}
player.move();
tryGenerateEnemy(enemy_list);
for (Enemy* i : enemy_list) i->move(player);

cleardevice();

putimage(0, 0, &img_background);
player.draw(1000 / 144);
tryGenerateEnemy(enemy_list);
for (Enemy* i : enemy_list) i->draw(1000 / 144);

```



## 第二节代码完成展示

**main.cpp**

```
#include <graphics.h>
#include <string>
#include <vector>

const int WINDOWS_HEIGHT = 1280;
const int WINDOWS_WIDTH = 720;

#pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });

	return;
}

class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		this->m_interval_ms = interval;
		
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);

			this->m_frame_list.push_back(frame);
		}
	}

	~Animation()
	{
		for (int i = 0; i < this->m_frame_list.size(); i++)
		{
			delete this->m_frame_list[i];
		}
	}

public:
	void play(int x, int y, int delta)
	{
		this->m_timer += delta;
		if (this->m_timer >= this->m_interval_ms)
		{
			this->m_idx_frame = (this->m_idx_frame + 1) % this->m_frame_list.size();
			this->m_timer = 0;
		}

		putimage_alpha(x, y, this->m_frame_list[this->m_idx_frame]);
	}

private:
	int m_timer = 0;
	int m_idx_frame = 0;
	int m_interval_ms;
	std::vector<IMAGE* > m_frame_list;

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
		
		this->m_anim_left_player = new Animation(L"./img/player_left_%d.png", 6, 45);
		this->m_anim_right_player = new Animation(L"./img/player_right_%d.png", 6, 45);
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
			case VK_UP:
				this->is_move_up = true;
				break;
			case VK_DOWN:
				this->is_move_down = true;
				break;
			case VK_LEFT:
				this->is_move_left = true;
				break;
			case VK_RIGHT:
				this->is_move_right = true;
				break;
			}
		}
		if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				this->is_move_up = false;
				break;
			case VK_DOWN:
				this->is_move_down = false;
				break;
			case VK_LEFT:
				this->is_move_left = false;
				break;
			case VK_RIGHT:
				this->is_move_right = false;
				break;
			}
		}
	}

	void move()
	{
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
		int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH + this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		static bool facing_left = false;
		int dir_x = this->is_move_right - this->is_move_left;
		facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));

		facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta);
	}

	const POINT& getPosition() const
	{
		return this->m_player_pos;
	}

private:
	const int PLAYER_SPEED = 3;

	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;

private:
	POINT m_player_pos = { 500, 500 };

	IMAGE m_img_shadow;
	Animation* m_anim_left_player;
	Animation* m_anim_right_player;

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

		this->m_anim_left_enemy = new Animation(L"./img/enemy_left_%d.png", 6, 45);
		this->m_anim_right_enemy = new Animation(L"./img/enemy_right_%d.png", 6, 45);

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
		return false;
	}

	bool checkBulletCollision(const Bullet& bullet)
	{
		return false;
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
		int shadow_pos_x = this->m_enemy_pos.x + (this->ENEMY_WIDTH + this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_enemy_pos.y + this->ENEMY_HEIGHT + 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		this->m_face_left ? this->m_anim_left_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta) : this->m_anim_right_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta);
	}

private:
	const int ENEMY_SPEED = 2;

	const int ENEMY_WIDTH = 80;
	const int ENEMY_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;

private:
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

int main()
{
	initgraph(1280, 720);

	bool running = true;

	ExMessage msg;
	IMAGE img_background;

	Player player;
	std::vector<Enemy*> enemy_list;

	loadimage(&img_background, _T("./img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			player.processEvent(msg);
		}
		player.move();
		tryGenerateEnemy(enemy_list);
		for (Enemy* i : enemy_list) i->move(player);

		cleardevice();

		putimage(0, 0, &img_background);
		player.draw(1000 / 144);
		for (Enemy* i : enemy_list) i->draw(1000 / 144);
		
		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);    
		}

	}

	EndBatchDraw();

	return 0;
}
```



## 第三节

### 碰撞检测——点与矩形的判定

完成前面未完成的碰撞检测，完成敌人生命周期的维护，当敌人被子弹击中后死亡释放内存

```
//Enemy对象成员函数checkBulletCollision()等效将子弹视作点，只需检测点是否在矩形内即可
bool checkBulletCollision(const Bullet& bullet)
{
	return bullet.m_pos.x >= this->m_enemy_pos.x && bullet.m_pos.x <= this->m_enemy_pos.x + this->ENEMY_WIDTH
		&& bullet.m_pos.y >= this->m_enemy_pos.y && bullet.m_pos.y <= this->m_enemy_pos.y + this->ENEMY_HEIGHT;
}

//Enemy对象成员函数checkPlayerCollision()视作矩形与矩形覆盖检测，但是需要注意的是，图像存在透明区域，若单纯视作矩形碰撞可能会产生透明区域相同覆盖而检测为碰撞的情况，且本项目同类游戏判定要求也不会过于要求严格，可以将玩家或敌人中心点视作碰撞点，此处便是将敌人中心视作碰撞点
bool checkPlayerCollision(const Player& player)
{
	const POINT& player_pos = player.getPosition();

	return this->m_enemy_pos.x + this->ENEMY_WIDTH / 2 >= player_pos.x && this->m_enemy_pos.x + this->ENEMY_WIDTH / 2 <= player_pos.x + player.getFrameWidth()
		&& this->m_enemy_pos.y + this->ENEMY_HEIGHT / 2 >= player_pos.y && this->m_enemy_pos.y + this->ENEMY_HEIGHT / 2 <= player_pos.y + player.getFrameHeight();
}

//由于需要玩家类内的图片高度与宽度，可以选择将其改为公开属性或提供get方法即可
const int getFrameWidth() const
{
    return this->PLAYER_WIDTH;
}

const int getFrameHeight() const
{
    return this->PLAYER_HEIGHT;
}

```



### 检测结果进行弹窗与结束主循环

移动结束后直接进行遍历检测即可，若发生与敌人碰撞则进行弹窗并更改循环条件终止循环

```
//主循环中，可以利用move的循环一起查看是否发生碰撞
for (Enemy* i : enemy_list)
{
	i->move(player);
	if (i->checkPlayerCollision(player))
	{
		MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
		running = false;
		break;
	}
}

```



### 环绕某点的波浪型旋转子弹实现与相关功能

创建std::vector\<Bullet\>向量数组存储子弹类初始化为3，同时实现更多子弹的效果

```
//编写updataBullets()全局函数，更新子弹位置，假设中心点与原点为(x, y)偏转角度为α，则子弹位置为(x + cosα * r, y + sinα * r)，想让α连续变化的简单方法只需要获取时间即可
void updataBullets(std::vector<Bullet>& bullet_list, const Player& player) 
{
	const double RADIAL_SPEED = 0.0045;
	const double TANGENT_SPEED = 0.0055;

	const POINT& player_pos = player.getPosition();
	double radian_interval = 2 * 3.141593 / bullet_list.size();

	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);

	int t = 0;
	for (Bullet& i : bullet_list)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * t++;
		i.m_pos.x = player_pos.x + player.getFrameWidth() / 2 + (int)(radius * sin(radian));
		i.m_pos.y = player_pos.y + player.getFrameHeight() / 2 + (int)(radius * cos(radian));
	}
}

//主循环增加子弹绘制
putimage(0, 0, &img_background);
for (Enemy* i : enemy_list) i->draw(1000 / 144);
player.draw(1000 / 144);
for (Bullet& i : bullet_list) i.drawBullet();

```



### 子弹碰撞检测与敌人类生命周期

增加受击函数hurt()与查看是否存活checkAlive()确定是否释放内存，添加是否存活标记alive管理

```
//Enemy对象中添加hurt()成员函数标记受击死亡
void hurt()
{
	this->is_alive = false;
}

//Enemy对象中添加查看是否存活checkAlive()成员函数
bool checkAlive()
{
    return this->is_alive;
}

//Enemy对象成员变量标记
bool is_alive = true;

//主循环中修改子弹检测受击敌人，当击中后标记敌人死亡
for (Enemy* i : enemy_list)
{
	i->move(player);
	if (i->checkPlayerCollision(player))
	{
		MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
		running = false;
		break;
	}

	for (const Bullet& j : bullet_list)
	{
		if (i->checkBulletCollision(j)) i->hurt();
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
	}
}

```



### 简单的计分机制实现

定义score变量记录分数，并在敌人死亡时增加得分，提供绘制得分的函数drawPlayerScore()绘制得分

```
//绘制分数为固定位置所以仅传入分数即可，或者可以创建全局变量不用参数也可以
void drawPlayerScore(int score)
{
	setbkmode(TRANSPARENT);
	settextcolor(RGB(225, 85, 185));
	outtextxy(10, 10, (L"当前玩家得分: " + std::to_wstring(score)).c_str());
}

//此处不粘贴代码，主要需要定义score变量，随后在delete敌人后增加分数，最后再在最顶层绘制分数

```



### 简单轻巧的音乐播放方式mciSendString

目录文件下mus存放音效文件，随后代码中链接Winmm.lib库

**加载音乐**代码调用使用代码如下

```
mciSendString(_T("open 音乐资源路径 alias 音乐名称"), NULL, 0, NULL);

使用用例：
mciSendString(_T("open ./mus/test.mp3 alias bgm"), NULL, 0, NULL);

```



**单次播放**音乐使用代码如下

```
mciSendString(_T("play 音乐名称 from 开始位置"), NULL, 0, NULL);//开始位置单位毫秒

使用用例：
mciSendString(_T("open ./mus/test.mp3 alias bgm"), NULL, 0, NULL);

mciSendString(_T("play bgm from 0"), NULL, 0, NULL);

```



**循环播放**音乐使用代码如下

```
mciSendString(_T("play 音乐名称 repeat from 开始位置"), NULL, 0, NULL);//开始位置单位毫秒

使用用例：
mciSendString(_T("open ./mus/test.mp3 alias bgm"), NULL, 0, NULL);

mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);

```



**阻塞播放**即播放结束后才继续执行使用代码如下

```
mciSendString(_T("play 音乐名称 wait"), NULL, 0, NULL);

使用用例：
mciSendString(_T("open ./mus/test.mp3 alias bgm"), NULL, 0, NULL);

mciSendString(_T("play bgm wait"), NULL, 0, NULL);

```



**关闭播放**使用如下代码释放资源

```
mciSendString(_T("close 音乐名称"), NULL, 0, NULL);

使用用例：
mciSendString(_T("open ./mus/test.mp3 alias bgm"), NULL, 0, NULL);

mciSendString(_T("close bgm"), NULL, 0, NULL);
```



**暂停继续**使用如下代码进行

```
mciSendString(_T("pause 音乐名称"), NULL, 0, NULL);
mciSendString(_T("stop 音乐名称"), NULL, 0, NULL);

mciSendString(_T("resume 音乐名称"), NULL, 0, NULL);

使用用例：
mciSendString(_T("open ./mus/test.mp3 alias bgm"), NULL, 0, NULL);

mciSendString(_T("play bgm"), NULL, 0, NULL);

mciSendString(_T("stop bgm), NULL, 0, NULL);
Sleep(10);
mciSendString(_T("resume bgm"), NULL, 0, NULL);

mciSendString(_T("pause bgm"), NULL, 0, NULL);
Sleep(10);
mciSendString(_T("resume bgm"), NULL, 0, NULL);

```



**获取音量**使用如下代码，注意获取的代码为字符类型，若想获取数字需要使用atoi转换

```
char lo[128];
mciSendString(_T("status 音乐名称 volume"), lo, 100, 0);

int nlo = atoi(lo);//获得的音量先加200再除1000再乘媒体音量即为播放出的音量
mciSendString((L"setaudio 音乐名称 volume to " + to_wstring(nlo % 1000 + 200)).c_str(), 0, 0, 0);
```



**跳转播放**到目标位置，单位毫秒代码如下

```
sciSendString(_T("seek 音乐名称 to 开始位置"), 0, 0, 0);
//可以填入时间单位毫秒，也可以填入例如: start或end表示从头或直接到末尾

使用用例：
sciSendString(_T("seek bgm to 7*1000"), 0, 0, 0);即跳转到7秒处开始播放

```



**进度获取**有播放长度与音乐总长度使用如下代码

```
char len[128]
sciSendString(_T("status 音乐名称 length"), len, 100, 0);//获取音乐长度

char now[128];
sciSendString(_T("status 音乐名称 position"), now, 100, 0);

//同样都使用atoi()转换即可
```



接下来直接在加载资源时加载音乐资源，并以开头循环播放背景音乐，在击中敌人时播放击中音效即可

```
//加载音效与背景音乐
mciSendString(_T("open ./mus/background_bgm.mp3 alias bkbgm"), NULL, 0, NULL);
mciSendString(_T("open ./mus/hit.mp3 alias hit"), NULL, 0, NULL);
//循环播放背景音乐
mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);

//在子弹检测到敌人碰撞时播放一次受击音乐
mciSendString(_T("play hit from 0"), NULL, 0, NULL);

//结束时释放资源
mciSendString(_T("close bgm"), NULL, 0, NULL);
mciSendString(_T("close hit"), NULL, 0, NULL);
```



## 第三节代码完成展示

**main.cpp**

```
#include <graphics.h>
#include <string>
#include <vector>

const int WINDOWS_HEIGHT = 720;
const int WINDOWS_WIDTH = 1280;

#pragma comment(lib, "Winmm.lib")

#pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });

	return;
}

class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		this->m_interval_ms = interval;
		
		TCHAR path_file[256];
		for (size_t i = 1; i <= num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);

			this->m_frame_list.push_back(frame);
		}
	}

	~Animation()
	{
		for (int i = 0; i < this->m_frame_list.size(); i++)
		{
			delete this->m_frame_list[i];
		}
	}

public:
	void play(int x, int y, int delta)
	{
		this->m_timer += delta;
		if (this->m_timer >= this->m_interval_ms)
		{
			this->m_idx_frame = (this->m_idx_frame + 1) % this->m_frame_list.size();
			this->m_timer = 0;
		}

		putimage_alpha(x, y, this->m_frame_list[this->m_idx_frame]);
	}

private:
	int m_timer = 0;
	int m_idx_frame = 0;
	int m_interval_ms;
	std::vector<IMAGE* > m_frame_list;

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
		
		this->m_anim_left_player = new Animation(L"./img/player_left_%d.png", 4, 45);
		this->m_anim_right_player = new Animation(L"./img/player_right_%d.png", 4, 45);
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
			case VK_UP:
				this->is_move_up = true;
				break;
			case VK_DOWN:
				this->is_move_down = true;
				break;
			case VK_LEFT:
				this->is_move_left = true;
				break;
			case VK_RIGHT:
				this->is_move_right = true;
				break;
			}
		}
		if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				this->is_move_up = false;
				break;
			case VK_DOWN:
				this->is_move_down = false;
				break;
			case VK_LEFT:
				this->is_move_left = false;
				break;
			case VK_RIGHT:
				this->is_move_right = false;
				break;
			}
		}
	}

	void move()
	{
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
		int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH - this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		static bool facing_left = false;
		int dir_x = this->is_move_right - this->is_move_left;
		facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));

		facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta);
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
	const int PLAYER_SPEED = 3;

	const int PLAYER_WIDTH = 48;
	const int PLAYER_HEIGHT = 64;
	const int SHADOW_WIDTH = 32;

private:
	POINT m_player_pos = { 500, 500 };

	IMAGE m_img_shadow;
	Animation* m_anim_left_player;
	Animation* m_anim_right_player;

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

		this->m_anim_left_enemy = new Animation(L"./img/enemy_left_%d.png", 3, 45);
		this->m_anim_right_enemy = new Animation(L"./img/enemy_right_%d.png", 3, 45);

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
		int shadow_pos_x = this->m_enemy_pos.x + (this->ENEMY_WIDTH - this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_enemy_pos.y + this->ENEMY_HEIGHT + 4;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		this->m_face_left ? this->m_anim_left_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta) : this->m_anim_right_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta);
	}

	void hurt()
	{
		this->is_alive = false;
	}

	bool checkAlive()
	{
		return this->is_alive;
	}

private:
	const int ENEMY_SPEED = 2;

	const int ENEMY_WIDTH = 48;
	const int ENEMY_HEIGHT = 56;
	const int SHADOW_WIDTH = 32;

private:
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

int main()
{
	initgraph(1280, 720);

	bool running = true;

	int score = 0;
	ExMessage msg;
	IMAGE img_background;

	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);

	mciSendString(_T("open ./mus/background_music.mp3 alias bgm"), 0, 0, 0);
	mciSendString(_T("open ./mus/hit_music.mp3 alias hit"), 0, 0, 0);
	loadimage(&img_background, _T("./img/background.png"));

	mciSendString(_T("play bgm repeat from 0"), 0, 0, 0);
	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			player.processEvent(msg);
		}
		player.move();
		tryGenerateEnemy(enemy_list);
		
		updataBullets(bullet_list, player);
		
		for (Enemy* i : enemy_list)
		{
			i->move(player);
			if (i->checkPlayerCollision(player))
			{
				MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
				running = false;
				break;
			}

			for (const Bullet& j : bullet_list)
			{
				if (i->checkBulletCollision(j))
				{
					i->hurt();
					mciSendString(_T("play hit from 0"), 0, 0, 0);
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

				score++;
			}
		}

		cleardevice();

		putimage(0, 0, &img_background);
		for (Enemy* i : enemy_list) i->draw(1000 / 144);
		player.draw(1000 / 144);
		for (Bullet& i : bullet_list) i.drawBullet();
		drawPlayerScore(score);

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);    
		}

	}

	mciSendString(_T("close bgm"), 0, 0, 0);
	mciSendString(_T("close hit"), 0, 0, 0);
	EndBatchDraw();

	return 0;
}
```



## 第四节

### 享元模式

假如在项目中需要渲染一棵树，图片资源与模型资源的占比通常相当大，一棵树的渲染通常起不到特别大的资源占用，但如果需要一整片树林，资源的加载通常就会占用特别多时间资源与内存资源，如果这些树的模型与图片都是相同的，我们就可以加载仅一次资源，之后重复利用素材资源节省大量加载时间与内存空间。

在我们的代码中，每一个Animation类中都有一个自己的动画帧序列m_frame_list，每个Enemy都有两个Animation动画类对象，这意味着每个敌人刷新都会进行尝试从磁盘加载两套动画，而磁盘读取的IO操作通常都相当费事，而主循环中应当尽量避免阻塞的或耗时过长的工作，任何数据与资源加载应当尽可能在进入程序前完成。

查看当前代码，可以共享的资源有哪些？很明显是m_frame_list数组了，接下来拆解Animation类定义图集Atlas类，说明共有资源玩家与敌人的左右方向资源，注意Atlas属于共有资源，其生命周期应由上层代码进行控制。

此处非常重要的点，需要注意公共变量必须要最最先完成创建，否则其他变量尝试获取公共数据时还是空指针，即使后续公共数据初始化完成，原先变量指向的地址并非是指向一个指向空指针的地址而是指向空指针，所以初始化后的结果依旧还是空指针。

```
class Atlas
{
public:
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

//全局变量，后续在主函数中加载资源初始化
Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;

//Animation中修改变量持有Atlas类
private:
	Atlas* m_anim_atlas;

//Animation中构造函数指定Atlas指针，注意由于是共享资源，所以析构中绝对不要释放Atlas资源
Animation(Atlas* atlas, int interval)
{
    this->m_interval_ms = interval;
    this->m_anim_atlas = atlas;
}

//其余部分修改
//将原本this->m_frame_list修改为this->m_anim_atlas->m_frame_list即可
Player()
{
    loadimage(&this->m_img_shadow, _T("./img/player_shadow.png"));

    this->m_anim_left_player = new Animation(atlas_player_left, 45);
    this->m_anim_right_player = new Animation(atlas_player_right, 45);
}

//主函数加载资源
atlas_enemy_left = new Atlas(L"./img/enemy_left_%d.png", 3);
atlas_enemy_right = new Atlas(L"./img/enemy_right_%d.png", 3);
atlas_player_left = new Atlas(L"./img/player_left_%d.png", 4);
atlas_player_right = new Atlas(L"./img/player_right_%d.png", 4);

//退出循环释放资源
delete atlas_enemy_left;
delete atlas_enemy_right;
delete atlas_player_left;
delete atlas_player_right;

```



### UI界面的按钮与交互

一个按钮之所以是按钮不是因为它长得像一个按钮，而是因为它能够对交互事件作出响应。

无论是图片还是文本，能够对玩家点击事件进行补货并修改作出对应数据响应就是按钮，通常按钮都带有三个状态的图片即：默认状态，悬停状态，按下状态。

设计按钮类，首先是创建类对象，描述按钮坐标以及存储按钮状态三个图片对象，通过枚举类定义按钮的三种状态：Idle，Hovered，Push，状态改变仅在消息处理阶段进行切换，即有按键或鼠标消息时才进行更新，注意三个状态的跳转切换Idle(默认)←→Hovered(悬停)←→Push(按下)，所以需要注意按钮要对鼠标移动，左键按下抬起消息处理，以及定义纯虚函数onClick函数使得Button类变为接口，创建StartGameButton与QuitGameButton实现接口定义

```
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

//继承实现StartGameButton与QuitGameButton类
class StartGameButton : public Button
{
public:
	StartGameButton(RECT region, LPCTSTR path_idle, LPCTSTR path_hovered, LPCTSTR path_pushed) : Button(region, path_idle, path_hovered, path_pushed) {}

	~StartGameButton() = default;

public:
	void onClick()
	{
		is_start_game = true;
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
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}

};

//注意此处将running修改为is_running方便检索bool类型，同时添加is_start_game标记是否启动，并且将数据置于全局区为全局变量，注意背景音乐的播放应置于开始时，所以移动bgm的播放位置

//主函数初始化创建与加载
/*IMAGE img_gamemenu;//由于没有彩蛋背景可供加载，所以注释掉了，如果有资源了可以加载*/

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

StartGameButton btn_start_game(region_btn_start_game, L"./img/start_game_button_idle.png", L"./img/start_game_button_hovered.png", L"./img/start_pushed_button_game.png");
QuitGameButton btn_quit_game(region_btn_quit_game, L"./img/quit_game_button_idle.png", L"./img/quit_game_button_hovered.png", L"./img/quit_pushed_button_game.png");

/*loadimage(&img_gamemenu, _T("./img/gamemenu.png"));//注释原因见声明处*/

```



### 简单的菜单界面与游戏界面逻辑修改

根据声明的is_start_game区分事件处理部分，渲染与更新部分同样需要使用if判断区分

```
//事件处理部分
while (peekmessage(&msg))
{
	if (is_start_game)
	{
		player.processEvent(msg);
	}
	else
	{
		btn_quit_game.draw();
		btn_start_game.draw();
	}
}

//数据更新部分
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
			MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
			is_running = false;
			break;
		}

		for (const Bullet& j : bullet_list)
		{
			if (i->checkBulletCollision(j))
			{
				i->hurt();
				mciSendString(_T("play hit from 0"), NULL, 0, NULL);
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

			score++;
		}
	}
}

//图形渲染部分
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

```



## 第四节代码完成展示

**main.cpp**

```
#include <graphics.h>
#include <string>
#include <vector>

bool is_running = true;
bool is_start_game = false;

const int WINDOWS_HEIGHT = 720;
const int WINDOWS_WIDTH = 1280;

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "MSIMG32.LIB")

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
	void play(int x, int y, int delta)
	{
		this->m_timer += delta;
		if (this->m_timer >= this->m_interval_ms)
		{
			this->m_idx_frame = (this->m_idx_frame + 1) % this->m_anim_atlas->m_frame_list.size();
			this->m_timer = 0;
		}

		putimage_alpha(x, y, this->m_anim_atlas->m_frame_list[this->m_idx_frame]);
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
		
		this->m_anim_left_player = new Animation(atlas_player_left, 45);
		this->m_anim_right_player = new Animation(atlas_player_right, 45);
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
			case VK_UP:
				this->is_move_up = true;
				break;
			case VK_DOWN:
				this->is_move_down = true;
				break;
			case VK_LEFT:
				this->is_move_left = true;
				break;
			case VK_RIGHT:
				this->is_move_right = true;
				break;
			}
		}
		if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				this->is_move_up = false;
				break;
			case VK_DOWN:
				this->is_move_down = false;
				break;
			case VK_LEFT:
				this->is_move_left = false;
				break;
			case VK_RIGHT:
				this->is_move_right = false;
				break;
			}
		}
	}

	void move()
	{
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
		int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH - this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		static bool facing_left = false;
		int dir_x = this->is_move_right - this->is_move_left;
		facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));

		facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta);
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
	const int PLAYER_SPEED = 3;

	const int PLAYER_WIDTH = 48;
	const int PLAYER_HEIGHT = 64;
	const int SHADOW_WIDTH = 32;

private:
	POINT m_player_pos = { 500, 500 };

	IMAGE m_img_shadow;
	Animation* m_anim_left_player;
	Animation* m_anim_right_player;

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

		this->m_anim_left_enemy = new Animation(atlas_enemy_left, 45);
		this->m_anim_right_enemy = new Animation(atlas_enemy_right, 45);

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
		int shadow_pos_x = this->m_enemy_pos.x + (this->ENEMY_WIDTH - this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_enemy_pos.y + this->ENEMY_HEIGHT + 4;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		this->m_face_left ? this->m_anim_left_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta) : this->m_anim_right_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta);
	}

	void hurt()
	{
		this->is_alive = false;
	}

	bool checkAlive()
	{
		return this->is_alive;
	}

private:
	const int ENEMY_SPEED = 2;

	const int ENEMY_WIDTH = 48;
	const int ENEMY_HEIGHT = 56;
	const int SHADOW_WIDTH = 32;

private:
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

	atlas_enemy_left = new Atlas(L"./img/enemy_left_%d.png", 3);
	atlas_enemy_right = new Atlas(L"./img/enemy_right_%d.png", 3);
	atlas_player_left = new Atlas(L"./img/player_left_%d.png", 4);
	atlas_player_right = new Atlas(L"./img/player_right_%d.png", 4);

	mciSendString(_T("open ./mus/background_music.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open ./mus/hit_music.mp3 alias hit"), NULL, 0, NULL);

	int score = 0;
	ExMessage msg;

	IMAGE img_background/*, img_gamemenu*/;
	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);

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
					MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
					is_running = false;
					break;
				}

				for (const Bullet& j : bullet_list)
				{
					if (i->checkBulletCollision(j))
					{
						i->hurt();
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
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

					score++;
				}
			}
		}

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
```



## 第五节

### 图片像素和显示缓存

将图片放大到直接可见由无数纯色矩形组成的图案，单个矩形即为1个像素，一个像素有且仅有一种颜色填充构成

RGB色彩空间使用一套规则描述色彩即R红色，G绿色，B蓝色按一定强度混合得到所有颜色，若使用代码表示则可以使用一个包含了三个int的类表示，而图片即这个类的二维数组，平时对玩家角色在地图上绘制渲染，即是将玩家图片覆盖转移到更大的地图图片上罢了

```
//一个像素
class Color
{
public:
	int r;
	int g;
	int b;
};

Color pictrue[100][100];//一个大小为100*100px的图片

```



阅读EasyX代码中可以找到其中，IMAGE类中存在一个名为m_pBuffer的指针，注释Memory buffer of the image（图片内存缓冲地址）用于存储图片色彩缓冲区地址，也就是说实际上EasyX中存储数据的方式为将图片的像素二维数组每一行末尾与下一行开头相接的一维数组

由此，若在我们的例子中访问第i行第j个像素的方法为 pictrue\[i\]\[j\]，而在EasyX中访问的方法为m_pBuffer[i*PICTRUE_WIDTH + j]，这里的PICTRUE_WIDTH即指图片宽度

通过查阅文档可以找到，获取缓存地址的方法为函数GetImageBuffer()函数

```
DWORD GetImageBuffer(IMAGE* pImg = NULL);

使用示例：
DWORD* buffer = GetImageBuffer(&img);

```



一个DWORD占四个字节存储RGB色彩元素和额外的透明度通道信息，接下来由上面学习的内容，下面来完成几个图像处理功能



### 图像水平翻转

由于图片资源素材中，左右朝向的角色与敌人只有方向对称所以完全可以自动计算得到水平翻转的结果，可以大大减少素材资源的数量减少冗余，减少准备阶段的素材准备处理

加载向左动画序列帧，定义向右动画序列帧，使用循环计算每一帧向右动画序列帧，注意需要Resize设置图像大小，避免浅拷贝与空指针带来的问题，随后获取向左图片与向右图片的像素缓存区，通过双层循环拷贝到向右对应的位置上

此外为了方便直接修改另一侧图像图集，所以直接添加一个Atlas默认无参构造方法，并且将所有享元模式下共享资源的初始化均放入了函数loadAtlas()中，主函数将初始化Atlas部分更改为调用该函数即可

```
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

```



### 像素色块处理与图像闪烁效果

为了对各个像素进行处理，需要获取到每个像素RGB色彩分量，查询EasyX文档可以得到每个DWORD在内存中的顺序就是RGB，所以可以使用以下方式直接获取到RGB色彩分量的值

```
DWORD pix_color = buffer[y * width + x];
BYTE a = (pix_color & 0xff000000) >> 24;//11111111000000000000000000000000 透明度
BYTE r = (pix_color & 0x00ff0000) >> 16;//00000000111111110000000000000000   R
BYTE g = (pix_color & 0x0000ff00) >> 8; //00000000000000001111111100000000   G
BYTE b = (pix_color & 0x000000ff);      //00000000000000000000000011111111   B

```



不过也提供了三个宏可以获取，函数为getGValue()，getRValue()，getBValue()，通过传入COLORREF类型颜色值即可获取，但是注意COLORREF在内存中的顺序是BGR，所以在实际使用时要注意两色的调换

```
DWORD pix_color = buffer[y * width + x];
BYTE r = GetBValue(pix_color);
BYTE g = GetGValue(pix_color);
BYTE b = GetRValue(pix_color);

```



闪烁效果即，原本图像在纯白色剪影下穿插播放的结果，而剪影的纯白色序列帧当然也可以通过操作渲染缓冲区进行运行时动态生成

组合颜色首先可以使用RGB宏组合出ColoRREF类型值，然后使用BGR交换红蓝色顺序，最后无论是RGB还是BGR都是对三原色的色彩通道进行赋值，若希望这个色彩完全不透明还需要赋值透明通道，最终结果如下

```
//获取白色剪影 这里或的就是0xff000000 转BYTE为说明255将被作8位存储增加可读性和规范不写也没事
DWORD white_pix = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);

```



为了使用剪影功能，所以要给玩家添加血量与被攻击的判定，增加标记与血量与闪烁时长，当被撞击时标记删除攻击的敌人防止重复收到伤害，同时标记玩家收到攻击，为方便标记时同时获取玩家血量，当标记为收到攻击计时器计时，若未结束计时进行闪烁，达到计时直接重置下标即可

剪影切换为方便闪烁切换可直接加载在原对应方向的图像之间，当闪烁时只需更新图片步长为1而正常状态步长为2即可，所以在原Animation类内的play函数中添加默认参数为1的参数step，在Player的draw中声明步长根据状态改变即可

```
//Animation下的成员函数
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
//方便解除闪烁，小项目无需增加太多额外算法完成基本功能即可
void restart()
{
	this->m_idx_frame = 0;
}

//玩家类内成员变量增加，此处heart即玩家血量或者还能被碰撞次数
int m_heart = 1;

bool is_hurt = false;
int m_sketch_time = 0;

//玩家类内成员函数修改
int hurt()
{
	this->is_hurt = true;
	this->m_sketch_time = 600;

	return this->m_heart--;
}
/根据是否被撞击修改步长，接触闪烁时重置即可解除闪烁
void draw(int delta)
{
	int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH - this->SHADOW_WIDTH) / 2;
	int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

	putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

	static bool facing_left = false;
	int dir_x = this->is_move_right - this->is_move_left;
	facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));
	
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
	facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta, step) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta, step);
}

//加载方法，添加在加载完成共享资源后即loadAtlas()后面即可
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

//主循环中被敌人撞击到处修改如下即可，i为枚举的敌人
if (i->checkPlayerCollision(player))
{
    i->hurt();
    if (!player.hurt())
    {
        MessageBox(GetHWnd(), L"被碰到了", L"游戏结束", MB_OK);
        is_running = false;
        break;
    }					
}
```



### Alpha混合与图片叠加的冻结效果

在项目开始时由于EasyX的putimage()函数不考虑图片素材的透明度，所以还手写了putimage_alpha()函数解决透明部分变为纯黑的情况，在putimage_alpha()函数中我们使用了AlphaBlend()函数来解决，正是因为AlphaBlend在绘制时使用了像素透明度通道

像素透明度存在时的色彩计算公式为，注意此处的alpha不再是0到255的数而是0到1的浮点：

最终颜色 = 源颜色 * alpha + 目标颜色 * (1 - alpha);

也就是说，如果要将一个带透明度的颜色绘制到窗口某处的已有颜色处，会根据浮点权重积后的和作为最终的颜色绘制到窗口上面

接下来的案例中为了实现冻结效果，只需要将冻结图片以一定的透明度覆盖在玩家上即可，且仅有玩家进行所以重写draw方法，详情见下方代码

```
//为了增加冰冻的意义，将部分变量移动至外部全局变量区域
int score = 0;
IMAGE img_ice;

//由于冰冻需要获取当前帧，所以在Animation中新增方法获取当前帧图像	
IMAGE* getCurrentFrame()
{
    return this->m_anim_atlas->m_frame_list[this->m_idx_frame];
}

//Player类内增加变量，具体含义见注释
const int HURT_TIME = 600;//受伤后闪烁时间
const int FROZEN_TIME = 3000;//冰冻时间

const int THICKNESS = 5;//冰冻高亮宽度
const float RATIO = 0.25f;//冰冻图片与原图混叠比例
const float THRESHOLD = /*0.84f*/0.76f;//高亮阈值，自绘素材0.76f明显一些，一般推荐0.84f

//Player类内添加冰冻状态函数
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

//Player类内修改受伤状态如果受伤解除冰冻，同时为受伤时提供无敌状态
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

//Player类内增加了触发冰冻的按键，同时为了方便按键活动顺便增加了字母的按键移动
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

//Player类内修改move函数使其冰冻状态无法移动，实际上只是在移动前判断是否冰冻中而已
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

//Player类内重点重写draw方法
void draw(int delta)
{
	int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH - this->SHADOW_WIDTH) / 2;
	int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

	putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);//绘制阴影不变

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
	//受伤状态修改不变

	if (this->is_frozen)
	{
		this->m_frozen_time -= delta;
		if (this->m_frozen_time <= 0)
		{
			this->is_frozen = false;
		}
	}
	//冰冻状态仿照受伤状态做计时处理

	IMAGE img_current_frame;//声明当前帧图片

	static bool facing_left = false;
	int dir_x = this->is_move_right - this->is_move_left;
	facing_left = (dir_x == 0 ? facing_left : (dir_x < 0 ? true : false));//计算当前朝向

	if (this->is_frozen)
	{
		img_current_frame = IMAGE(facing_left ? *this->m_anim_left_player->getCurrentFrame() : *this->m_anim_right_player->getCurrentFrame());//通过三目确定方向获取当前帧图片
		int width = img_current_frame.getwidth();
		int height = img_current_frame.getheight();

		DWORD* color_buffer_frame_img = GetImageBuffer(&img_current_frame);
		DWORD* color_buffer_ice_img = GetImageBuffer(&img_ice);

		this->m_height_light_y = ((this->FROZEN_TIME - this->m_frozen_time) / 45) * this->THICKNESS % height;//此处计算使用计时器的使用时间除以帧间隔以宽度作为移动距离循环移动
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx = y * width + x;
				DWORD color_frame_img = color_buffer_frame_img[idx];
				DWORD color_ice_img = color_buffer_ice_img[idx];
				if ((color_frame_img & 0xff000000) >> 24)//非透明像素块
				{
					BYTE r = GetBValue(color_frame_img) * this->RATIO + GetBValue(color_ice_img) * (1 - this->RATIO);
					BYTE g = GetGValue(color_frame_img) * this->RATIO + GetGValue(color_ice_img) * (1 - this->RATIO);
					BYTE b = GetRValue(color_frame_img) * this->RATIO + GetRValue(color_ice_img) * (1 - this->RATIO);

					if ((y >= this->m_height_light_y && y <= this->m_height_light_y + this->THICKNESS)
						&& (r / 255.0f) * 0.2126f + (g / 255.0f) * 0.7152f + (b / 255.0f) * 0.0722f >= this->THRESHOLD)//在高亮区间且超过高亮阈值，此处计算方法为经验添值，推荐阈值做了修改但此处的比例未作修改
					{
						color_buffer_frame_img[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);//直接设置为纯白色
						continue;
					}
					color_buffer_frame_img[idx] = BGR(RGB(r, g, b)) | (((DWORD)(BYTE)(255)) << 24);//设置为混合颜色
				}
			}
		}
		putimage_alpha(this->m_player_pos.x, this->m_player_pos.y, &img_current_frame);
	}
	else
	{
		facing_left ? this->m_anim_left_player->play(this->m_player_pos.x, this->m_player_pos.y, delta, step) : this->m_anim_right_player->play(this->m_player_pos.x, this->m_player_pos.y, delta, step);//非冰冻状态正常绘制
	}
}

```



## 第五节代码完成展示

**main.cpp**

```
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
		
		this->m_anim_left_player = new Animation(atlas_player_left, 45);
		this->m_anim_right_player = new Animation(atlas_player_right, 45);
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
		int shadow_pos_x = this->m_player_pos.x + (this->PLAYER_WIDTH - this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_player_pos.y + this->PLAYER_HEIGHT + 8;

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

	const int PLAYER_WIDTH = 48;
	const int PLAYER_HEIGHT = 64;
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

		this->m_anim_left_enemy = new Animation(atlas_enemy_left, 45);
		this->m_anim_right_enemy = new Animation(atlas_enemy_right, 45);

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
		int shadow_pos_x = this->m_enemy_pos.x + (this->ENEMY_WIDTH - this->SHADOW_WIDTH) / 2;
		int shadow_pos_y = this->m_enemy_pos.y + this->ENEMY_HEIGHT + 4;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &this->m_img_shadow);

		this->m_face_left ? this->m_anim_left_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta) : this->m_anim_right_enemy->play(this->m_enemy_pos.x, this->m_enemy_pos.y, delta);
	}

	void hurt()
	{
		this->is_alive = false;
	}

	bool checkAlive()
	{
		return this->is_alive;
	}

private:
	const int ENEMY_SPEED = 2;

	const int ENEMY_WIDTH = 48;
	const int ENEMY_HEIGHT = 56;
	const int SHADOW_WIDTH = 32;

private:
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
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);

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
					i->hurt();
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
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
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

					score++;
				}
			}
		}

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
```



## 自行扩展

简单\*：可以自行编写一个更新子弹数量的函数，在更新时调用检查分数是否获得超过x的倍数，每超过一次增加一颗子弹直到最多y颗子弹\[✔]

简单\*：敌人血量同样增加为2点，在底侧绘制红色填充血条，血条为空才算击杀敌人\[✔\]

中等\*：自行编写一个敌人的导弹类，随机生成在地图边缘向另一个方向发射，在生成前在生成未知警告，到达地图另一侧边缘释放资源，撞击到玩家玩家掉血

困难\*：增加敌人，例如发射直线的子弹的敌人，冲刺移动的敌人，增加更多攻击方式，例如发射追踪敌人的子弹，或连锁攻击的闪电攻击，或发射直线的子弹，注意生成的子弹需要释放或回收资源

# 结束



