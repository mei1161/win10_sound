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
    // �E�B���h�E�N���X�̍쐬�Ɠo�^
    WNDCLASS wnd;
    ZeroMemory( &wnd, sizeof( wnd ) );
    wnd.hInstance = hInstance;                                  // �A�v���P�[�V�����C���X�^���X
    wnd.lpszClassName = "Class";                                    // �N���X��
    wnd.hCursor = LoadCursor( nullptr, IDC_ARROW );           // �J�[�\���`��
    wnd.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1); // �f�t�H���g�̔w�i�F
    wnd.lpfnWndProc = windowProc;                                 // �R�[���o�b�N�֐��|�C���^�̓o�^
    if( !RegisterClass( &wnd ) ) return 0;
    //COM�̏�����
    CoInitializeEx( NULL, COINIT_MULTITHREADED );
    //�t���O�̐ݒ�
    AUDIO_ENGINE_FLAGS flags = AudioEngine_Default;

#ifdef _DEBUG
    flags = flags | AudioEngine_Debug;
#endif

    // �I�[�f�B�I�G���W���̍쐬
    AudioEngine* engine = new AudioEngine( flags );

    // �T�E���h�G�t�F�N�g�̍쐬
    SoundEffect* sound = new SoundEffect( engine, L"sound.wav" );

    // �����V���b�g�Đ�
    /*sound->Play();*/

    //SoundEffectInstance���擾
    std::unique_ptr<SoundEffectInstance> bgm = sound->CreateInstance();
    //�L�[�{�[�h�N���X
    Keyboard key;

    // �E�B���h�E�̍쐬�ƕ\��
    HWND hWnd = CreateWindow( "Class", "Audio", WS_OVERLAPPEDWINDOW, 0, 0, 180, 120, nullptr, nullptr, nullptr, nullptr );
    ShowWindow( hWnd, SW_SHOWNORMAL );

    // ���Ԍv��
    auto prev = high_resolution_clock::now();

    // ���C�����[�v
    MSG msg = { 0 };
    while( msg.message != WM_QUIT )
    {
        // ���b�Z�[�W����
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            // ���Ԍv��
            const auto now = high_resolution_clock::now();
            const auto elapsed = duration_cast<microseconds>(now - prev);

            if( elapsed.count() >= 16666LL )
            {
                // ���ݎ�����O�񏈗������Ƃ��čX�V
                prev = now;

                // �L�[����
                Keyboard::State input = key.GetState();

                // �I�[�f�B�I�̍X�V����
                engine->Update();

                // �Đ�
                if( input.Z ) bgm->Play( true );

                // ��~
                if( input.X ) bgm->Stop();

                //�ꎞ��~
                if( input.C )bgm->Pause();

                //�ĊJ
                if( input.V ) bgm->Resume();

                //Pan�̐ݒ�
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

                //�s�b�`�̕ύX
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

                //�{�����[���̕ύX
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
                // ���݂̏�Ԃ��擾
                SoundState state = bgm->GetState();
                // STOPPED�c��~��
                // PLAYING�c�Đ���
                // PAUSED�c�ꎞ��~��
            }
        }
    }

    // ���������
    delete sound;
    delete engine;

    return 0;
}

// �E�B���h�E�v���V�[�W��
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