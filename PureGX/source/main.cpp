#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "rendering/RenderMeshFactory.h"
#include "rendering/Renderer.h"
#include "entitys/TestEntity.h"
#include "entitys/RenderEntity.h"

int main(void){
    Renderer::getInstance().setLookAndPosition(Vector3f(0,0,20), Vector3f(0,0,0));
    WPAD_Init();
    RenderEntity room(RM3D_ROOM);
    TestEntity dart;
    room.addChild((Entity*)&dart);
    while(true) {
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
        room.update();
        Renderer::getInstance().swapFrameBuffer();
    }

    return 0;
}
