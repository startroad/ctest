#include <stdio.h>

#define uchar char

void YUV420spRotate90(uchar *dst, const uchar *src, int srcWidth, int srcHeight);

void r(char *buf, int len)
{
    FILE *p = fopen("a.yuv", "r");
    if(p)
    {
        fread(buf, 1, len, p);
        fclose(p);
    }
}

void s(char *buf, int len)
{
    FILE *p = fopen("b.yuv", "wb+");
    if(p)
    {
        fwrite(buf, 1, len, p);
        fclose(p);
    }

}


int main(int argc, char **argv)
{
    int h=360,w=640;
    int len = w*h*3/2;
    char buf[len];
    char obuf[len];

    r(buf, len);
    YUV420spRotate90(obuf,buf,w,h);
    s(obuf, len);

    return 0;
}


void YUV420spRotate90(uchar *dst, const uchar *src, int srcWidth, int srcHeight)
{
    static int nWidth = 0, nHeight = 0;
    static int wh = 0;
    static int uvHeight = 0;
    if(srcWidth != nWidth || srcHeight != nHeight)
    {
        nWidth = srcWidth;
        nHeight = srcHeight;
        wh = srcWidth * srcHeight;
        uvHeight = srcHeight >> 1;//uvHeight = height / 2
    }

    //旋转Y
    int k = 0;
    int i,j;
    for(i = 0; i < srcWidth; i++) {
        int nPos = 0;
        for(j = 0; j < srcHeight; j++) {
            dst[k] = src[nPos + i];
            k++;
            nPos += srcWidth;
        }
    }

    for(i = 0; i < srcWidth; i+=2){
        int nPos = wh;
        for(j = 0; j < uvHeight; j++) {
            dst[k] = src[nPos + i];
            dst[k + 1] = src[nPos + i + 1];
            k += 2;
            nPos += srcWidth;
        }
    }
    return;
}
