#pragma once

#define FRAME_SIZE_WIDTH    640    
#define FRAME_SIZE_HEIGHT   360    
#define BLOCK_COUNT_WIDTH   ((int)(FRAME_SIZE_WIDTH/10.0))
#define BLOCK_COUNT_HEIGHT  ((int)(FRAME_SIZE_HEIGHT/10.0))

class ImageProcessing
{
public:
    ImageProcessing();
    ~ImageProcessing();
    
    static void* imageProcessingThread(void *arg);

private:


};