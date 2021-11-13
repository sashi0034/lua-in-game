#include "main.h"


using namespace sprite;






int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    {//デバッグ用コンソールウインドウの表示
        AllocConsole();
        FILE* stream;
        freopen_s(&stream, "CONIN$", "r", stdin);
        freopen_s(&stream, "CONOUT$", "w", stdout);
    }

    return game::Process();
    
}









namespace game
{
    int Process()
    {

        // ウインドウモードで起動
        DxLib::ChangeWindowMode(TRUE);

        // フルスクリーンモード時のモニターの解像度を最大にするモードに設定
        DxLib::SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);

        // フルスクリーンモード時の画面の拡大モードを最近点モードに設定
        DxLib::SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);

        DxLib::SetGraphMode(FULL_WIDTH, FULL_HEIGHT, 16);

        DxLib::SetAlwaysRunFlag(1); //非アクティブ状態でも動かす

        if (DxLib::DxLib_Init() == -1)        // DXライブラリ初期化処理
        {
            return -1;        // エラーが起きたら直ちに終了
        }
        Sprite::Init();

        Img = new resorce::Image(); //画像読み込み
                
        // Lua読み込み
        Lua = luaL_newstate();
        luaL_openlibs(Lua);
        if (luaL_dofile(Lua, "resorce.lua"))
        {
            OutputDebugString("Luaの読み込みに失敗しました");
            return -1;
        }
        lua::DefineSpriteFunc();
        new lua::LuaDebugManager();
        


        printf("game is beginning\n");

        //シーン
        SceneTransition();

        printf("game was ended\n");

        Sprite::End();
        DxLib::DxLib_End();        // ＤＸライブラリ使用の終了処理
        return 0;
        // ソフトの終了
    }


    int SceneTransition()
    {
        //new test::Scene();
        while (DxLib::ProcessMessage() != -1)
        {
            //new top::Scene();
            new main::Scene();
        }

        return 0;
    }


    void LoopBasicUpdate()
    {
        Sprite::AllUpdate();
        DxLib::SetDrawScreen(DX_SCREEN_BACK);
        DxLib::ClearDrawScreen();
        Sprite::AllDrawing();
        DxLib::ScreenFlip();
    }
}


namespace game
{
    namespace lua
    {
        void DefineSpriteFunc()
        {
            auto fnXy = [](lua_State* lua) {
                int sp = (int)luaL_checknumber(lua, 1);
                double x = luaL_checknumber(lua, 2);
                double y = luaL_checknumber(lua, 3);
                Sprite::Offset(sp, x, y);
                return 0;
            };

            auto fnZ = [](lua_State* lua) {
                int sp = (int)luaL_checknumber(lua, 1);
                short z = (short)luaL_checknumber(lua, 2);
                Sprite::Offset(sp, z);
                return 0;
            };

            auto fnUv = [](lua_State* lua) {
                int sp = (int)luaL_checknumber(lua, 1);
                int u = (int)luaL_checknumber(lua, 2);
                int v = (int)luaL_checknumber(lua, 3);
                Sprite::Image(sp, u, v);
                return 0;
            };


            lua_pushcfunction(Lua, fnXy);
            lua_setglobal(Lua, "SpriteXY");

            lua_pushcfunction(Lua, fnZ);
            lua_setglobal(Lua, "SpriteZ");

            lua_pushcfunction(Lua, fnUv);
            lua_setglobal(Lua, "SpriteUV");

        }
    }


    namespace lua
    {
        LuaDebugManager::LuaDebugManager()
        {
            Sp = Sprite::Make();
            Sprite::Belong(Sp, this);
            Sprite::Update(Sp, LuaDebugManager::CallUpdate);
            Sprite::Destructer(Sp, LuaDebugManager::CallDestructer);
        }

        void LuaDebugManager::Update()
        {



        }


        void LuaDebugManager::CallUpdate(int hSp)
        {
            std::any_cast<LuaDebugManager*>(Sprite::GetBelong(hSp))->Update();
        }
        void LuaDebugManager::CallDestructer(int hSp)
        {
            delete std::any_cast<LuaDebugManager*>(Sprite::GetBelong(hSp));
        }


    }
}














namespace game
{






    //メインシーン
    namespace main
    {
        Scene::Scene()
        {
            new Test();
            new BackGround();
            
            Loop();

            delete this;
        }
        void Scene::Loop()
        {

            while (DxLib::ProcessMessage() != -1)
            {
                LoopBasicUpdate();
            }
        }
    }






    // Test
    namespace main
    {
        Test* Test::GetIn = nullptr;


        // テスト
        Test::Test()
        {
            Sp = Sprite::Make();
            Sprite::Belong(Sp, this);
            Sprite::Update(Sp, Test::CallUpdate);
            Sprite::Destructer(Sp, Test::CallDestructer);
            Test::GetIn = this;

            Test::OtherSp = Sprite::Make(Img->Test, 0, 0, 128, 64);
            Sprite::Offset(Test::OtherSp, 100, 50, -200);
        }

        void Test::Update()
        {
            Sprite::Image(Sp, Img->Slime, 0, 0, 32, 32);
            lua_getglobal(Lua, "LuacallTest");
            lua_pushnumber(Lua, Sp);
            lua_pcall(Lua, 1, 1, 0);
            std::string str = luaL_checkstring(Lua, -1);
            printf(str.data());
        }


        void Test::CallUpdate(int hSp)
        {
            std::any_cast<Test*>(Sprite::GetBelong(hSp))->Update();
        }
        void Test::CallDestructer(int hSp)
        {
            delete std::any_cast<Test*>(Sprite::GetBelong(hSp));
            printf("delete sucessfully\n");
        }

    }





    // BackGround
    namespace main
    {
        BackGround* BackGround::GetIn = nullptr;


        // 背景
        BackGround::BackGround()
        {
            Image = DxLib::MakeScreen(ROUGH_WIDTH, ROUGH_HEIGHT, TRUE);
            Sp = Sprite::Make(Image, 0, 0, ROUGH_WIDTH, ROUGH_HEIGHT);
            DxLib::SetDrawScreen(Image);
            
            for (int x = 0; x < ROUGH_WIDTH; x+=32)
            {
                for (int y = 0; y < ROUGH_HEIGHT; y+=32)
                {
                    DxLib::DrawGraph(x, y, Img->Tile32, TRUE);
                }
            }

            Sprite::Offset(Sp, int(0), int(0), 4000);
            Sprite::Belong(Sp, this);
            Sprite::Update(Sp, BackGround::CallUpdate);
            Sprite::Destructer(Sp, BackGround::CallDestructer);
            BackGround::GetIn = this;

        }
        void BackGround::Update()
        {
        }


        void BackGround::CallUpdate(int hSp)
        {
            std::any_cast<BackGround*>(Sprite::GetBelong(hSp))->Update();
        }
        void BackGround::CallDestructer(int hSp)
        {
            delete std::any_cast<BackGround*>(Sprite::GetBelong(hSp));
        }

    }




#if 0
    // テンプレ
    namespace main
    {
        Templa::Templa()
        {
            Image = Img->Templa;

            Sp = Sprite::Make(Image, 0, 0, 16, 16);
            Sprite::Offset(Sp, X, Y);
            Sprite::Belong(Sp, this);
            Sprite::Update(Sp, Templa::CallUpdate);
            Sprite::Destructer(Sp, Templa::CallDestructer);
        }

        void Templa::Update()
        {
            DxLib::printfDx("Count = %d\n", Count);
            Count++;
        }


        void Templa::CallUpdate(int hSp)
        {
            std::any_cast<Templa*>(Sprite::GetBelong(hSp))->Update();
        }
        void Templa::CallDestructer(int hSp)
        {
            delete std::any_cast<Templa*>(Sprite::GetBelong(hSp));
        }

    }

    // テンプレ(手動描画)
    namespace main
    {
        Templa::Templa()
        {
            Sp = Sprite::Make();
            Sprite::Offset(Sp, -4000);
            Sprite::Belong(Sp, this);
            Sprite::Update(Sp, Templa::CallUpdate);
            Sprite::Drawing(Sp, Templa::CallDrawing);
            Sprite::Destructer(Sp, Templa::CallDestructer);
        }

        void Templa::Update()
        {
            DxLib::printfDx("Count = %d\n", Count);
            Count++;
        }

        void Templa::Drawing(int hX, int hY)
        {

        }

        void Templa::CallUpdate(int hSp)
        {
            std::any_cast<Templa*>(Sprite::GetBelong(hSp))->Update();
        }
        void Templa::CallDrawing(int hSp, int hX, int hY)
        {
            std::any_cast<Templa*>(Sprite::GetBelong(hSp))->Drawing(hX, hY);
        }
        void Templa::CallDestructer(int hSp)
        {
            delete std::any_cast<Templa*>(Sprite::GetBelong(hSp));
        }

    }

    // コライダー付きテンプレ
    namespace main
    {
        Templa::Templa() : hit::Collider(new hit::Collider::Rectangle(0, 0, 16, 16), MASK_TEMPLA)
        {
            Image = Img->Templa;

            Sp = Sprite::Make(Image, 0, 0, 16, 16);
            Sprite::Offset(Sp, X, Y);
            Sprite::Belong(Sp, this);
            Sprite::Update(Sp, Templa::CallUpdate);
            Sprite::Destructer(Sp, Templa::CallDestructer);
        }

        void Templa::Update()
        {
            DxLib::printfDx("Count = %d\n", Count);
            Count++;
        }


        void Templa::CallUpdate(int hSp)
        {
            std::any_cast<Templa*>(Sprite::GetBelong(hSp))->Update();
        }
        void Templa::CallDestructer(int hSp)
        {
            delete std::any_cast<Templa*>(Sprite::GetBelong(hSp));
        }

    }


#endif // 0





}


























namespace useful
{
    Random::Random() : engine(device()), distribution(0, INT_MAX - 1)
    {
    }
    Random::Random(int seed) : engine(seed), distribution(0, INT_MAX - 1)
    {
    }
    int Random::Get(int max)
    {
        return distribution(engine) % max;
    }
    int Random::Get(int min, int max)
    {
        return min + (distribution(engine) % (max - min));
    }
}














namespace sprite
{
    Sprite::Sprite()
    {
        drawingMethod = Sprite::DrawingProcess::Rough;

        for (int i = 0; i < ANIM_MAX; i++)
        {
            animData[i] = std::vector<int>(0);
            isAnim[i] = false;
            animStep[i] = 0;
            animCount[i] = 0;
        }
    }

    int Sprite::nextIndex = 0;
    int Sprite::anim1step[Sprite::ANIM_MAX];
    Sprite* Sprite::sprites[Sprite::SPRITE_MAX];
    std::pair<int, short> Sprite::sprites_Z[Sprite::SPRITE_MAX];


    void Sprite::Init()
    {
        for (int i = 0; i < SPRITE_MAX; i++)
        {
            Sprite::sprites[i] = new Sprite();
            Sprite::sprites_Z[i] = std::make_pair(i, 0);
        }

        nextIndex = 0;
        anim1step[ANIMTYPE_XY] = 3;
        anim1step[ANIMTYPE_UV] = 3;
        anim1step[ANIMTYPE_BLENDMODE] = 2;
        anim1step[ANIMTYPE_BLENDPAL] = 2;
        anim1step[ANIMTYPE_ROTATIONDEG] = 2;
    }


    void Sprite::End()
    {

        Sprite::AllClear();
        for (int i = 0; i < SPRITE_MAX; i++)
        {
            delete sprites[i];
        }
    }


    int Sprite::Make()
    {
        return Make(-1, 0, 0, 0, 0);
    }
    int Sprite::Make(int image)
    {
        int x = 0, y = 0;
        DxLib::GetGraphSize(image, &x, &y);
        return Make(image, 0, 0, x, y);
    }
    int Sprite::Make(int image, int u, int v, int width, int height)
    {
        for (int i = 0; i < SPRITE_MAX; i++)
        {
            int sp = (nextIndex + i) % SPRITE_MAX;

            if (!sprites[sp]->isUsed)
            {// 未使用なら割り当て
                delete sprites[sp];

                sprites[sp] = new Sprite();
                sprites_Z[sp] = std::make_pair(sp, 0);

                Sprite* spr = sprites[sp];
                spr->isUsed = true;
                spr->image = image; // image = -1 の場合は画像を表示しない
                spr->u = u;
                spr->v = v;
                spr->width = width;
                spr->height = height;

                nextIndex = sp + 1;

                return sp;
            }
        }
        return -1;
    }

    void Sprite::Reverse(int sp, bool isReverse)
    {
        sprites[sp]->isReverse = isReverse;
    }


    void Sprite::Image(int sp, int image)
    {
        Sprite* spr = sprites[sp];
        spr->image = image;
    }
    void Sprite::Image(int sp, int u, int v)
    {
        Sprite* spr = sprites[sp];
        spr->u = u;
        spr->v = v;
    }
    void Sprite::Image(int sp, int u, int v, int width, int height)
    {
        Image(sp, u, v);
        Sprite* spr = sprites[sp];
        spr->width = width;
        spr->height = height;
    }
    void Sprite::Image(int sp, int image, int u, int v, int width, int height)
    {
        Image(sp, image);
        Image(sp, u, v, width, height);
    }


    void Sprite::Offset(int sp, double x, double y)
    {
        Sprite* spr = sprites[sp];
        spr->x = x;
        spr->y = y;
    }
    void Sprite::Offset(int sp, double x, double y, short z)
    {
        Offset(sp, x, y);
        Offset(sp, z);
    }
    void Sprite::Offset(int sp, short z)
    {
        sprites_Z[sp] = std::make_pair(sp, z);
    }


    void Sprite::GetScreenXY(int sp, int* x, int* y)
    {
        Sprite* spr = sprites[sp];
        *x = int(spr->x + GetLinkDifference_X(sp));
        *y = int(spr->y + GetLinkDifference_Y(sp));
    }


    void Sprite::RotationDeg(int sp, double deg)
    {
        Sprite* spr = sprites[sp];
        spr->rotationRad = (deg / 180.0) * M_PI;
    }
    void Sprite::RotationRad(int sp, double rad)
    {
        Sprite* spr = sprites[sp];
        spr->rotationRad = rad;
    }


    void Sprite::Belong(int sp, std::any instance)
    {
        sprites[sp]->belong = instance;
    }

    std::any Sprite::GetBelong(int sp)
    {
        return sprites[sp]->belong;
    }

    void Sprite::Link(int sp, int link)
    {
        sprites[sp]->link = link;
    }


    double Sprite::GetLinkDifference_X(int sp)
    {
        Sprite* spr = sprites[sp];
        int link = spr->link;
        if (link != -1)
        {
            return sprites[link]->x + GetLinkDifference_X(link);
        }
        else
        {
            return 0;
        }
    }

    double Sprite::GetLinkDifference_Y(int sp)
    {
        Sprite* spr = sprites[sp];
        int link = spr->link;
        if (link != -1)
        {
            return sprites[link]->y + GetLinkDifference_Y(link);
        }
        else
        {
            return 0;
        }
    }

    void Sprite::Blend(int sp, int blendMode, int blendPal)
    {
        BlendMode(sp, blendMode);
        BlendPal(sp, blendPal);
    }
    void Sprite::BlendMode(int sp, int blendMode)
    {
        sprites[sp]->blendMode = blendMode;
    }
    void Sprite::BlendPal(int sp, int blendPal)
    {
        sprites[sp]->blendPal = blendPal;
    }


    void Sprite::Update(int sp, void (*updateMethod)(int hSp))
    {
        sprites[sp]->updateMethod = updateMethod;
    }
    void Sprite::Drawing(int sp, void (*drawingMethod)(int hSp, int hX, int hY))
    {
        sprites[sp]->drawingMethod = drawingMethod;
    }
    void Sprite::Destructer(int sp, void (*destructerMethod)(int hSp))
    {
        sprites[sp]->destructerMethod = destructerMethod;
    }

    void Sprite::Clear(int sp)
    {
        Sprite* spr = sprites[sp];
        if (spr->isUsed)
        {
            if (spr->destructerMethod != nullptr)
            {
                //printf("delted %d\n", sp);
                spr->destructerMethod(sp);
            }
            spr->isUsed = false;
        }
        else
        {
            return;
        }
    }
    void Sprite::Clear(int sp, bool unprotectOnly)
    {
        Sprite* spr = sprites[sp];
        if (unprotectOnly)
        {
            if (!spr->isProtect) spr->Clear(sp);
        }
        else
        {
            spr->Clear(sp);
        }
    }

    void Sprite::AllClear()
    {
        AllClear(false);
    }
    void Sprite::AllClear(bool protectOnly)
    {
        for (int i = 0; i < SPRITE_MAX; i++)
        {
            Sprite* spr = sprites[i];
            if (protectOnly)
            {
                if (!spr->isProtect) Clear(i);
            }
            else
            {
                Clear(i);
            }
        }
    }

    void Sprite::Protect(int sp, bool isProtect)
    {
        sprites[sp]->isProtect = isProtect;
    }



    void Sprite::AllUpdate()
    {
        for (int i = 0; i < SPRITE_MAX; i++)
        {
            Sprite* spr = sprites[i];
            if (spr->isUsed)
            {
                if (spr->updateMethod != nullptr) spr->updateMethod(i);
            }
        }
        //AnimationAllUpdate();
    }


    void Sprite::AllDrawing()
    {


        std::pair<int, short> ol[SPRITE_MAX];
        memcpy(ol, sprites_Z, sizeof(sprites_Z[0]) * SPRITE_MAX);

        std::sort(&ol[0], &ol[0] + sizeof(ol) / sizeof(ol[0]),
            [](auto& left, auto& right) { return left.second > right.second;});

        for (int i = 0; i < SPRITE_MAX; i++)
        {
            int sp = ol[i].first;
            Sprite* spr = sprites[sp];
            if (spr->isUsed)
            {

                if (spr->image == -1) continue;

                int x, y;
                x = int(spr->x + GetLinkDifference_X(sp));
                y = int(spr->y + GetLinkDifference_Y(sp));

                x *= ROUGH_SCALE;
                y *= ROUGH_SCALE;
                DxLib::SetDrawBlendMode(spr->blendMode, spr->blendPal);

                if (spr->drawingMethod != nullptr)
                {
                    spr->drawingMethod(sp, x, y);
                }
            }
        }
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

    }

    double Sprite::GetUsingRate()
    {
        int use = 0;
        for (int i = 0; i < SPRITE_MAX; i++)
        {
            if (sprites[i]->isUsed) use++;
        }
        return ((double)use) / SPRITE_MAX;
    }




    void Sprite::DrawingProcess::Rough(int hSp, int hX, int hY)
    {
        Draw(hSp, hX, hY, ROUGH_SCALE);
    }
    void Sprite::DrawingProcess::Twice(int hSp, int hX, int hY)
    {
        Draw(hSp, hX, hY, 2);
    }
    void Sprite::DrawingProcess::DotByDot(int hSp, int hX, int hY)
    {
        Draw(hSp, hX, hY, 1);
    }
    void Sprite::DrawingProcess::Draw(int sp, int x, int y, int scale)
    {
        Sprite* spr = sprites[sp];
        int image = DxLib::DerivationGraph(spr->u, spr->v, spr->width, spr->height, spr->image);

        if (spr->rotationRad == 0)
        {
            DxLib::DrawRotaGraph3(x, y, 0, 0, scale, scale, 0, image, 1, spr->isReverse);
        }
        else
        {
            int x1 = spr->width / 2, y1 = spr->height / 2;
            double cos = std::cos(spr->rotationRad), sin = std::sin(spr->rotationRad);

            double x2 = x1 * cos - y1 * sin, y2 = x1 * sin + y1 * cos;

            double dx = x1 - x2, dy = y1 - y2;

            DxLib::DrawRotaGraph3(x + int(dx * scale), y + int(dy * scale),
                0, 0, // 回転中心
                scale, scale,
                spr->rotationRad, image,
                1, spr->isReverse);
        }
        DxLib::DeleteGraph(image);
    }







}

















    