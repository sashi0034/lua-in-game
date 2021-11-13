#pragma once

#define _USE_MATH_DEFINES
#include "DxLib.h"
#include "lua.hpp"

#include <random>
#include <any>
#include <map>
#include <math.h>

#define DEBUG

#ifdef DEBUG

#endif // DEBUG






namespace sprite
{

    class Sprite
    {
        static const int ROUGH_SCALE = 3;
        static const int SPRITE_MAX = 4096;
        static const int ANIM_MAX = 8;
        static int anim1step[];
        static int nextIndex;

        static Sprite* sprites[];
        static std::pair<int, short> sprites_Z[];


        bool isUsed = false;
        double x = 0;
        double y = 0;
        int image = -1;
        int u = 0;
        int v = 0;
        int width = 0;
        int height = 0;
        bool isReverse = false;
        bool isProtect = false;
        int link = -1;
        double rotationRad = 0;

        int blendMode = DX_BLENDMODE_ALPHA;
        int blendPal = 255;


        std::any belong = nullptr;

        void (*updateMethod)(int hSP) = nullptr;
        void (*destructerMethod)(int hSp) = nullptr;
        void (*drawingMethod)(int hSp, int hX, int hY);

        // 自動アニメ―ション関連
        std::vector<int> animData[ANIM_MAX];
        bool isAnim[ANIM_MAX];
        int animStep[ANIM_MAX];
        int animCount[ANIM_MAX];

        Sprite();


    public:

        static const int ANIMTYPE_XY = 1;
        static const int ANIMTYPE_UV = 2;
        static const int ANIMTYPE_BLENDMODE = 3;
        static const int ANIMTYPE_BLENDPAL = 4;
        static const int ANIMTYPE_ROTATIONDEG = 5;

        static void Init();
        static void End();

        static int Make();
        static int Make(int image);
        static int Make(int image, int u, int v, int width, int height);

        static void Reverse(int sp, bool isReverse);

        static void Image(int sp, int image);
        static void Image(int sp, int u, int v);
        static void Image(int sp, int u, int v, int width, int height);
        static void Image(int sp, int image, int u, int v, int width, int height);

        static void Offset(int sp, double x, double y);
        static void Offset(int sp, double x, double y, short z);
        static void Offset(int sp, short z);


        static void GetScreenXY(int sp, int* x, int* y);

        static void RotationDeg(int sp, double deg);
        static void RotationRad(int sp, double rad);


        static void Belong(int sp, std::any instance);
        static std::any GetBelong(int sp);

        static void Link(int sp, int link);

        static double GetLinkDifference_X(int sp);
        static double GetLinkDifference_Y(int sp);

        static void Blend(int sp, int blendMode, int blendPal);
        static void BlendMode(int sp, int blendMode);
        static void BlendPal(int sp, int blendPal);

        static void Update(int sp, void (*updateMethod)(int hSp));
        static void Drawing(int sp, void (*drawingMethod)(int hSp, int hX, int hY));
        static void Destructer(int sp, void (*destructerMethod)(int hSp));

        static void Clear(int sp);
        static void Clear(int sp, bool protectOnly);

        static void AllClear();
        static void AllClear(bool protectOnly);

        static void Protect(int sp, bool isProtect);

        static void AllUpdate();
        static void AllDrawing();

        static double GetUsingRate();


        class DrawingProcess
        {
            static int a;
        public:
            static void Rough(int hSp, int hX, int hY);
            static void Twice(int hSp, int hX, int hY);
            static void DotByDot(int hSp, int hX, int hY);
            static void Draw(int sp, int x, int y, int scale);
        };


    };


}






namespace useful
{
    class Random
    {
        std::random_device device;
        std::default_random_engine engine;
        std::uniform_int_distribution<int> distribution;

    public:
        Random(void);
        Random(int seed);
        int Get(int max);
        int Get(int min, int max);
    };
}





namespace game
{
    namespace resorce 
    {
        class Image
        {
        public:
            int Test = LoadGraph(R"(C:\Users\satos\source\repos\gane04-STG\bin\Debug\Assets\Images\cloud--128x64.png)");
            int Slime = LoadGraph(R"(D:\working_directory\images\bakugonR--32x32.png)");
            int Tile32 = LoadGraph(R"(C:\Users\satos\OneDrive\progyamming\Study\public_html\src\game05\images/magmaTile--32x32.png)");

#if 0
            int Templa = LoadGraph(R"(.png)");
#endif // 0

            Image()
            {
            }
        };

    }


    namespace lua
    {
        void DefineSpriteFunc();
        
        class LuaDebugManager
        {
            bool mHasLastWriteTime = false;
            SYSTEMTIME mLastWriteTime;

        public:
            LuaDebugManager();

            int Sp;
            int Count = 0;

            void Update();

            static void CallUpdate(int hSp);
            static void CallDestructer(int hSp);
        };

    }

}


namespace game{
    
    const int ROUGH_WIDTH = 416, ROUGH_HEIGHT = 240;
    const int ROUGH_SCALE = 3;
    const int FULL_WIDTH = ROUGH_WIDTH * ROUGH_SCALE;
    const int FULL_HEIGHT = ROUGH_HEIGHT * ROUGH_SCALE;

    useful::Random* Rand = new useful::Random();

    int GameState;
    bool CanRestartProgram = false;

    resorce::Image* Img;
    lua_State* Lua;


    int Process();
    int SceneTransition();

    void LoopBasicUpdate();



    namespace main 
    {
        class Scene
        {
        public:
            Scene();
            void Loop();
        };





        class Test
        {
        public:
            static Test* GetIn;

            int Sp, OtherSp;

            Test();
            void Update();

            static void CallUpdate(int hSp);
            static void CallDestructer(int hSp);
        };



 



        class BackGround
        {
        public:
            static BackGround* GetIn;

            int Sp;
            int Image;

            BackGround();
            void Update();

            static void CallUpdate(int hSp);
            static void CallDestructer(int hSp);
        };




#if 0

        // テンプレ
        class Templa
        {
        public:
            int Sp;
            int Image;
            double X = 0, Y = 0;
            int Count = 0;

            Templa();
            void Update();

            static void CallUpdate(int hSp);
            static void CallDestructer(int hSp);
        };

        // テンプレ(手動描画)
        class Templa
        {
        public:
            int Sp;
            int Count = 0;

            Templa();
            void Update();
            void Drawing(int hX, int hY);

            static void CallUpdate(int hSp);
            static void CallDrawing(int hSp, int hX, int hY);
            static void CallDestructer(int hSp);
        };

        // コライダー付きテンプレ
        class Templa : public hit::Collider
        {
        public:
            int Sp;
            int Image;
            // X, Yはいらない
            int Count = 0;

            Templa();
            void Update();

            static void CallUpdate(int hSp);
            static void CallDestructer(int hSp);
        };


#endif // 0


    }
}


