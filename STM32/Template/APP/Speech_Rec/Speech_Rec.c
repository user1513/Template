#include "Speech_Rec.h"
#include "Voice_Access.h"
#include "stdint.h"

/*语音识别处理函数*/
void Speech_Handle(uint8_t _ucMode)

{
    static uint8_t status = 0xff;
    switch(_ucMode)
    {
        case 0:status = voice_Receive(); break;
        case 1:if(!status) {status = 0xff; voice_End_Receive(); voice_info_Send();} break;
        default: break;
    }
}

