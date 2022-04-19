#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "rendering/Renderer.h"
#include "entitys/TestEntity.h"

int main(void){
    Renderer::getInstance();
    WPAD_Init();
    TestEntity en;
    while(true) {
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
        en.update();
        Renderer::getInstance().swapFrameBuffer();
    }

    return 0;
}
