#include "voewrapper.h"
extern "C"
{
#include "static.h"
}


int test_add(int x, int y)
{
    // 调用static里面的方法
    return add(x, y);
}

void CreateVoE()
{
    VoiceEngine* voe = VoiceEngine::Create();
    VoEBase* base = VoEBase::GetInterface(voe);
    base->Init();
    int ch = base->CreateChannel();
    base->StartPlayout(ch);
    base->DeleteChannel(ch);
    base->Terminate();
    base->Release();
    VoiceEngine::Delete(voe);
}
