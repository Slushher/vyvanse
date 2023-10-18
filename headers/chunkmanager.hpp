#pragma once

class ChunkManager{
public:
    void UpdateLoadList();
    void UpdateRebuildList();
    void UpdateUnloadList();
    void UpdateVisibilityList();
};