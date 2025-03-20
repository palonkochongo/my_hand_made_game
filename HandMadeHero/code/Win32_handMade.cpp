#include <cstdint>
#include <stdint.h>

#include "handmade.cpp"
//#include "handmade.h"

#include <windows.h>
#include <Xinput.h>
#include <dsound.h>
#include <xaudio2.h>
#include <math.h>
#include <tchar.h>
#include <stdio.h>
#define internal static
#define local_persist static
#define global_variable static
#define pi32 3.1415926535897932384626433832795f
#define SIZE_OF_SOUND_POOL 256*1000*1000

typedef uint8_t uint8;
typedef int32_t bool32;

struct win32OffScreenBuffer{
BITMAPINFO info;
void *memory;
int width;
int height;
int bytePerPixel;
int pitch;
};

global_variable bool running;
global_variable struct win32OffScreenBuffer globalBuffer;
global_variable IXAudio2 *globalXAudio2;
global_variable IXAudio2MasteringVoice *globalXAudioMasteringVoice;
global_variable WAVEFORMATEX globalWfx;

//X Input Get State
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub){
  return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

//X Input Set State
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub){
  return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput()
{
    HMODULE XInputLibrary = LoadLibraryA("Xinput1_4.dll"); 
    if (!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("Xinput1_3.dll"); 
    }
    if (!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("Xinput9_1_0.dll"); 
    }

    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
    if(!XInputGetState) { XInputGetState = XInputGetStateStub; }
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    if(!XInputSetState) { XInputSetState = XInputSetStateStub; }
    }
    else
    {
        // We still don't have any XInputLibrary
        XInputGetState = XInputGetStateStub;
        XInputSetState = XInputSetStateStub;
        // TODO(casey): Diagnostic
    }
}

//Sould the interface and mastering voice be global?
//TODO: add COM initialization - why? I dont know - maybe research why.
internal void xAudio2_Init(){
  if(SUCCEEDED(XAudio2Create(&globalXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR))){
    OutputDebugStringA("XAudio2 has been initialized. \n");
  }
  if(SUCCEEDED(globalXAudio2->CreateMasteringVoice(&globalXAudioMasteringVoice))){
    OutputDebugStringA("Mastering voice has been created. \n");
  }
}


struct Win32_windowDimension{
  int width;
  int height;
};

internal Win32_windowDimension win32getWindowDimensions(HWND window){
  Win32_windowDimension result;
  RECT clientRect;
  GetClientRect(window, &clientRect);
  result.width = clientRect.right-clientRect.left;
  result.height = clientRect.bottom-clientRect.top;
return result;
}

internal void
win32resizeDIBsection(win32OffScreenBuffer *buffer, int width, int height){
          if(buffer->memory){
             VirtualFree(buffer->memory, 0, MEM_RELEASE);
          }

          buffer->width = width;
          buffer->height = height;
          buffer->bytePerPixel = 4;

          buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
          buffer->info.bmiHeader.biWidth = buffer->width;
          buffer->info.bmiHeader.biHeight = -buffer->height;
          buffer->info.bmiHeader.biPlanes = 1;
          buffer->info.bmiHeader.biBitCount = 32;
          buffer->info.bmiHeader.biCompression = BI_RGB;
          int bitmapMemorySize = (buffer->width*buffer->height)*buffer->bytePerPixel;
          buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
          buffer->pitch = width*buffer->bytePerPixel;
} 

internal void
win32DisplayBufferInWindow(win32OffScreenBuffer *buffer,
                           HDC bitmapDeviceContext,
                           int windowWidth,
                           int windowHeight,
                           int x,
                           int y,
                           int width,
                           int height){
  StretchDIBits(
                bitmapDeviceContext,
                /*
                x, y, width, height,
                x, y, width, height,
                */
                0, 0, windowWidth, windowHeight,
                0, 0, buffer->width, buffer->height,
                buffer->memory,
                &buffer->info,
                DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK
win32mainWindowsCallback( HWND window,
                          UINT Message,
                          WPARAM WParam,
                          LPARAM LParam)
{
  LRESULT Result = 0;
switch (Message)
        {
          case WM_SIZE:
          {
            OutputDebugStringA("WM_SIZE\n");
          }
          break;
          case WM_DESTROY:
          {
            running = false;
            OutputDebugStringA("WM_DESTROY\n");
          }
          break;
          case WM_CLOSE:
          {
            running = false;
            OutputDebugStringA("WM_CLOSE\n");
          }
          break;
          case WM_ACTIVATEAPP:
          {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
          }
          break;
          case WM_KEYDOWN:
          case WM_KEYUP:
          case WM_SYSKEYDOWN:
          case WM_SYSKEYUP:
          {
            OutputDebugStringA("WM_KEYDOWN\n");
            uint32_t key = (uint32_t)WParam;
            bool WasDown = ((LParam & 1<<30) != 0);
            bool IsDown = ((LParam & 1<<31) == 0);
            if(WasDown != IsDown){
              if (key == 'W')
              {
              }
              else if (key == 'A')
              {
              }
              else if (key == 'S')
              {
              }
              else if (key == 'Q')
              {
              }
              else if (key == 'E')
              {
              }
              else if (key == VK_UP)
              {
              }
              else if (key == VK_DOWN)
              {
              }
              else if (key == VK_RIGHT)
              {
              }
              else if (key == VK_LEFT)
              {
              }
              else if (key == VK_SPACE)
              {
              }
              else if (key == VK_ESCAPE)
              {
              }
              else if (key == VK_RETURN)
              {
              }
            }
          }
          break;
          case WM_PAINT:
          {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            
            // All painting occurs here, between BeginPaint and EndPaint.
            RECT rect = paint.rcPaint;
            int top = rect.top;
            int left = rect.left;
            int width = rect.right-left;
            int height = rect.bottom-top;

            Win32_windowDimension dimensions = win32getWindowDimensions(window);
            win32DisplayBufferInWindow(&globalBuffer, deviceContext, dimensions.width, dimensions.height, left, top, dimensions.width, dimensions.height);
            EndPaint(window, &paint);
          }
          break;
          default:
          {
      //      OutputDebugStringA("Default\n");
            Result = DefWindowProcA(window, Message, WParam, LParam);
          }
          break;
        }
    return Result;
}

//*******************************************BUMP ALLOCATOR STUFF************************************************
struct BumpAllocator{
  size_t size;
  size_t memoryUsed;
  char* memory;
};

BumpAllocator bumpAllocatorCreate(size_t size){
  BumpAllocator bp = {};
  bp.memory = (char*)malloc(size);
  if(bp.memory){
    bp.size = size;
  memset(bp.memory, 0, size);
  }
  else{
    OutputDebugStringA("Failed to allocate memory!");
  }
  return bp;
}

char* bumpAlloc(BumpAllocator* bumpAllocator, size_t size){
  char* result;
  size_t allignedSize = (size_t)(size + 7) & ~ 7; //Aligns per 8 bytes.
  if(bumpAllocator->memoryUsed + allignedSize <= bumpAllocator->size){
    result = bumpAllocator->memory + bumpAllocator->memoryUsed;
    bumpAllocator->memoryUsed += allignedSize;
  }
  else{
    OutputDebugStringA("Not enought memory in bumpAllocator!");
  }
  return result;
}

//*******************************************SOUND******************************************


//For XAudio2 to handle different filetypes. FindChuck included.
#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
                hr = HRESULT_FROM_WIN32( GetLastError() );
            break;

        default:
            if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
            return HRESULT_FROM_WIN32( GetLastError() );            
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );
    DWORD dwRead;
    if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
        hr = HRESULT_FROM_WIN32( GetLastError() );
    return hr;
}

struct Sound{
  const char* name;
  UINT32 size;
  BYTE *data;
};

IXAudio2SourceVoice* makeSourceVoice(WAVEFORMATEX* wfx_ptr) {
  IXAudio2SourceVoice* XAudio2SourceVoice{};
  if(SUCCEEDED(globalXAudio2->CreateSourceVoice(&XAudio2SourceVoice, &globalWfx))){
    OutputDebugStringA("Source voice has been created\n");
  }
  return XAudio2SourceVoice;
}

void submitBuffer(IXAudio2SourceVoice* sv, Sound *sd) {
  XAUDIO2_BUFFER buffer = {};
  buffer.AudioBytes = sd -> size;
  buffer.pAudioData = sd -> data;
  buffer.Flags = XAUDIO2_END_OF_STREAM;

  if(SUCCEEDED(sv->SubmitSourceBuffer(&buffer))){
    OutputDebugStringA("Buffer has been submitted to source\n");
  }
}

void playSound(Sound *sound) {
  IXAudio2SourceVoice* sv = makeSourceVoice(&globalWfx);
  submitBuffer(sv, sound);

  if(SUCCEEDED(sv->Start(0))){
    OutputDebugStringA("Playing Sound\n");
  }
}

Sound loadSound (const char* soundName, BumpAllocator *soundPool) {
        struct Sound sd{};
        sd.name = (char*)soundName;
        char pathtmp[256];
        sprintf_s(pathtmp, sizeof(pathtmp), "../data/%s.wav", soundName); //NOTE: Hardcoded path good idea????
        TCHAR* path = (TCHAR*) _TEXT(pathtmp);

        HANDLE hFile =  CreateFile(
                                  path,
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL );

        //if( INVALID_HANDLE_VALUE == hFile ) return HRESULT_FROM_WIN32( GetLastError() );
        if( INVALID_HANDLE_VALUE == hFile ) OutputDebugStringA("File not found. \n");
        //if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) ) return HRESULT_FROM_WIN32( GetLastError() );
        if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) ) OutputDebugStringA("Invalid file pointer");

        DWORD chunkSize;
        DWORD chunkPosition;
        //check the file type, should be fourccWAVE or 'XWMA'
        FindChunk(hFile,fourccRIFF, chunkSize, chunkPosition);
        DWORD filetype;
        ReadChunkData(hFile,&filetype,sizeof(DWORD),chunkPosition);
        //if (filetype != fourccWAVE) return S_FALSE;
        if (filetype != fourccWAVE) OutputDebugStringA("Not a wave file");
        //Locates FMT chunk
        if(SUCCEEDED(FindChunk(hFile, fourccFMT, chunkSize, chunkPosition))){
          OutputDebugStringA("fourccFMT found. \n");
        }
        // Locate the DATA chunk.
        if(SUCCEEDED(FindChunk(hFile, fourccDATA, chunkSize, chunkPosition))){
          OutputDebugStringA("Audio data located. \n");
        }

        //Allocate memory for audiodata.
        sd.data = (BYTE*)bumpAlloc(soundPool, chunkSize);
        //Write into the allocated space.
        ReadChunkData(hFile, sd.data, chunkSize, chunkPosition);
        sd.size = chunkSize;
        return sd;
}

//******************************************************END*************************************************

int CALLBACK
WinMain(
        HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR     CommandLine,
        int       ShowCode) {


        WNDCLASSA windowClass = {};
        
        win32resizeDIBsection(&globalBuffer, 500 , 250);

        windowClass.style = CS_HREDRAW|CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = win32mainWindowsCallback;
        windowClass.hInstance = Instance;
        // windowClass.hIcon = ;
        windowClass.lpszClassName = "EliasHeroWindowClass";

        LARGE_INTEGER lastQPCount;
        QueryPerformanceCounter(&lastQPCount);
        LARGE_INTEGER QPFreq;
        QueryPerformanceFrequency(&QPFreq);
        uint64_t lastRDTSC = __rdtsc();

        Win32LoadXInput();
        
          
        if(RegisterClass(&windowClass)) {
                HWND window = CreateWindowExA(
                            0,
                            windowClass.lpszClassName,
                            "Elias Hero",
                            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            0, 0, Instance, 0
                            );

                if(window) {

                      HDC deviceContext = GetDC(window);

                      int blueOffset = 0;
                      int greenOffset = 0;


                      xAudio2_Init();

                      globalWfx = {};
                      globalWfx.wFormatTag = WAVE_FORMAT_PCM;
                      globalWfx.nChannels = 2;
                      globalWfx.nSamplesPerSec = 48000;
                      globalWfx.wBitsPerSample = 24;
                      globalWfx.nBlockAlign = (globalWfx.nChannels * globalWfx.wBitsPerSample)/8;
                      globalWfx.nAvgBytesPerSec = (globalWfx.nSamplesPerSec * globalWfx.nBlockAlign);

                      BumpAllocator sounds = bumpAllocatorCreate(SIZE_OF_SOUND_POOL);
                      
                      
                      Sound kb;

                      bool playonce = true;
                      if(playonce){
                      kb = loadSound("kb", &sounds);
                        playSound(&kb);
      }

                      running=true;
                      while(running){
                        
                          MSG Message;
                          
                          while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)){
                            if(Message.message==WM_QUIT){
                              running=false;
                            }
                              TranslateMessage(&Message);
                              DispatchMessageA(&Message);
                          }


                          DWORD dwResult;
                          for (DWORD controllerIndex=0; controllerIndex < XUSER_MAX_COUNT; controllerIndex++ ) {
                                  XINPUT_STATE controllerState;
                                  ZeroMemory( &controllerState, sizeof(XINPUT_STATE) );

                                  // Simply get the state of the controller from XInput.
                                  dwResult = XInputGetState( controllerIndex, &controllerState );

                                  if( dwResult == ERROR_SUCCESS ) {
                                        // Controller is connected
                                        //TODO: see if packetnumber increment too rapidly
                                        XINPUT_GAMEPAD *pad = &controllerState.Gamepad;

                                        bool up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                                        bool down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                                        bool left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                                        bool right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                                        bool xButton = (pad->wButtons & XINPUT_GAMEPAD_X);
                                        bool yButton = (pad->wButtons & XINPUT_GAMEPAD_Y);
                                        bool bButton= (pad->wButtons & XINPUT_GAMEPAD_B);
                                        bool aButton= (pad->wButtons & XINPUT_GAMEPAD_A);
                                        bool start = (pad->wButtons & XINPUT_GAMEPAD_START);
                                        bool back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
                                        bool leftShoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                                        bool rightShoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                                        bool leftThumb = (pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
                                        bool rightThumb = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
                                        
                                        int16_t stickLY = (pad->sThumbLY); 
                                        int16_t stickLX = (pad->sThumbLX); 
                                        int16_t stickRY = (pad->sThumbRY); 
                                        int16_t stickRX = (pad->sThumbRX); 
                                        
                                        XINPUT_VIBRATION vibrations;
                                        vibrations.wLeftMotorSpeed = 0;
                                        vibrations.wRightMotorSpeed = 0;
                                        if(xButton){
                                          vibrations.wLeftMotorSpeed = 65000;
                                        }
                                        if(yButton){
                                          vibrations.wRightMotorSpeed = 65000;
                                        }
                                        XInputSetState(0, &vibrations);



                                        blueOffset += stickLY / 6000;
                                        greenOffset += stickLX / 6000;

                                  }

                                  else
                                  {
                                    OutputDebugStringA("No controller connnected");  // Controller is not connected
                                  }
                                  
                                  gameOffScreenBuffer buffer;
                                  buffer.memory = globalBuffer.memory;
                                  buffer.width = globalBuffer.width;
                                  buffer.height = globalBuffer.height;
                                  buffer.pitch = globalBuffer.pitch;
                                  gameUpdateAndRender(&buffer, blueOffset, greenOffset);

                                
                                  Win32_windowDimension dimensions = win32getWindowDimensions(window);
                                  win32DisplayBufferInWindow(&globalBuffer, deviceContext, dimensions.width, dimensions.height, 0 , 0, dimensions.width, dimensions.height);
                                  //ReleaseDC(window, deviceContext); redundant with OWNDC and creating our DC outside the loop.

                          }

                    //Recording FPS, milleSec pr frame, megaCycles pr frame.
                          LARGE_INTEGER endQPCount;
                          float QPThisFrameTicks;
                          uint64_t endRDTSC = __rdtsc();
                          float FPS; 
                          float frameMS;
                          float MGcyclesPrFrame = (float)(endRDTSC - lastRDTSC)/1000000.0f;
                          QueryPerformanceCounter(&endQPCount);
                          QPThisFrameTicks = (float)(endQPCount.QuadPart - lastQPCount.QuadPart);
                          frameMS = (QPThisFrameTicks * 1000.0f) / (float)QPFreq.QuadPart;
                          FPS = (float)(QPFreq.QuadPart / QPThisFrameTicks);

                          char performanceBuffer[256]; 
                          snprintf(performanceBuffer, sizeof(performanceBuffer), "ms: %0.2f, FPS: %0.2f, MG cycles pr frame: %0.4f\n", frameMS, FPS, MGcyclesPrFrame);
                          OutputDebugStringA(performanceBuffer);

                          lastQPCount = endQPCount;
                          lastRDTSC = endRDTSC;

                      }
                  }
                  else{
                    //DO SOMETHING LOGGING;
                  }
        }
        else
        {
            //DO SOMETHING: Logging
        }


          
          return(0);

}
