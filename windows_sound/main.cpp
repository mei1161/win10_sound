// Include
#include <chrono>
#include<memory>
#include <Windows.h>
#include <Audio.h>
#include <Keyboard.h>

// Linker
#pragma comment( lib, "winmm.lib" )

// Namespace
using namespace std::chrono;
using namespace DirectX;

// Prototype
LRESULT CALLBACK windowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// WinMain
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    // ウィンドウクラスの作成と登録
    WNDCLASS wnd;
    ZeroMemory( &wnd, sizeof( wnd ) );
    wnd.hInstance = hInstance;                                  // アプリケーションインスタンス
    wnd.lpszClassName = "Class";                                    // クラス名
    wnd.hCursor = LoadCursor( nullptr, IDC_ARROW );           // カーソル形状
    wnd.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1); // デフォルトの背景色
    wnd.lpfnWndProc = windowProc;                                 // コールバック関数ポインタの登録
    if( !RegisterClass( &wnd ) ) return 0;
    //COMの初期化
    CoInitializeEx( NULL, COINIT_MULTITHREADED );
    //フラグの設定
    AUDIO_ENGINE_FLAGS flags = AudioEngine_Default;

#ifdef _DEBUG
    flags = flags | AudioEngine_Debug;
#endif

    // オーディオエンジンの作成
    AudioEngine* engine = new AudioEngine( flags );

    // サウンドエフェクトの作成
    SoundEffect* sound = new SoundEffect( engine, L"sound.wav" );

    // ワンショット再生
    /*sound->Play();*/

    //SoundEffectInstanceを取得
    std::unique_ptr<SoundEffectInstance> bgm = sound->CreateInstance();
    //キーボードクラス
    Keyboard key;

    // ウィンドウの作成と表示
    HWND hWnd = CreateWindow( "Class", "Audio", WS_OVERLAPPEDWINDOW, 0, 0, 180, 120, nullptr, nullptr, nullptr, nullptr );
    ShowWindow( hWnd, SW_SHOWNORMAL );

    // 時間計測
    auto prev = high_resolution_clock::now();

    // メインループ
    MSG msg = { 0 };
    while( msg.message != WM_QUIT )
    {
        // メッセージ処理
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            // 時間計測
            const auto now = high_resolution_clock::now();
            const auto elapsed = duration_cast<microseconds>(now - prev);

            if( elapsed.count() >= 16666LL )
            {
                // 現在時刻を前回処理時刻として更新
                prev = now;

                // キー入力
                Keyboard::State input = key.GetState();

                // オーディオの更新処理
                engine->Update();

                // 再生
                if( input.Z ) bgm->Play( true );

                // 停止
                if( input.X ) bgm->Stop();

                //一時停止
                if( input.C )bgm->Pause();

                //再開
                if( input.V ) bgm->Resume();

                //Panの設定
                static float pan = 0.0F;
                if( input.Left )
                {
                    pan -= 0.01F;
                    if( pan < -1.0F )pan = -1.0F;
                    bgm->SetPan( pan );
                }
                else if( input.Right )
                {
                    pan += 0.01F;
                    if( pan > 1.0F )pan = 1.0F;
                    bgm->SetPan( pan );
                }

                //ピッチの変更
                static float pitch = 0.0F;
                if( input.A )
                {
                    pitch -= 0.01F;
                    if( pitch < -1.0f )pitch = -1.0f;
                    bgm->SetPitch( pitch );
                }
                else if( input.S )
                {
                    pitch += 0.01F;
                    if( pitch > 1.0F )pitch = 1.0F;
                    bgm->SetPitch( pitch );
                }

                //ボリュームの変更
                static float volume = 0.0F;
                if( input.D )
                {
                    volume -= 0.01F;
                    if( volume < -1.0f )volume = -1.0F;
                    bgm->SetVolume( volume );
                }
                else if( input.F )
                {
                    volume += 0.01F;
                    if( volume > 1.0F )volume = 1.0F;
                    bgm->SetVolume( volume );
                }
                // 現在の状態を取得
                SoundState state = bgm->GetState();
                // STOPPED…停止中
                // PLAYING…再生中
                // PAUSED…一時停止中
            }
        }
    }

    // メモリ解放
    delete sound;
    delete engine;

    return 0;
}

// ウィンドウプロシージャ
LRESULT CALLBACK windowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
    case WM_ACTIVATEAPP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage( msg, wParam, lParam ); break;

    case WM_KEYDOWN:
        Keyboard::ProcessMessage( msg, wParam, lParam );
        switch( wParam )
        {
        case VK_ESCAPE: PostMessage( hWnd, WM_CLOSE, 0, 0 ); break;
        }
        break;

    case WM_DESTROY: PostQuitMessage( 0 ); break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}