/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CC_IMAGE_H__
#define __CC_IMAGE_H__

#include "../global.h"
#include <string>

NS_NODE_BEGIN

/**
 * @addtogroup platform
 * @{
 */
class CCImage {
public:
    CCImage();
    ~CCImage();

    typedef enum
    {
        kFmtJpg = 0,
        kFmtPng,
        kFmtTiff,
        kFmtWebp,
        kFmtRawData,
        kFmtUnKnown
    } EImageFormat;

    /** @typedef CCTexture2DPixelFormat
     Possible texture pixel formats
     */
    typedef enum {
        //! 32-bit texture: RGBA8888
        kCCTexture2DPixelFormat_RGBA8888,
        //! 24-bit texture: RGBA888
        kCCTexture2DPixelFormat_RGB888,
        //! 16-bit texture without Alpha channel
        kCCTexture2DPixelFormat_RGB565,
        //! 8-bit textures used as masks
        kCCTexture2DPixelFormat_A8,
        //! 8-bit intensity texture
        kCCTexture2DPixelFormat_I8,
        //! 16-bit textures used as masks
        kCCTexture2DPixelFormat_AI88,
        //! 16-bit textures: RGBA4444
        kCCTexture2DPixelFormat_RGBA4444,
        //! 16-bit textures: RGB5A1
        kCCTexture2DPixelFormat_RGB5A1,
        //! 4-bit PVRTC-compressed texture: PVRTC4
        kCCTexture2DPixelFormat_PVRTC4,
        //! 2-bit PVRTC-compressed texture: PVRTC2
        kCCTexture2DPixelFormat_PVRTC2,
        
        //! Default texture format: RGBA8888
        kCCTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_RGBA8888,
    } CCTexture2DPixelFormat;

    /**
    @brief  Load image from stream buffer.

    @warning kFmtRawData only supports RGBA8888.
    @param pBuffer  stream buffer which holds the image data.
    @param nLength  data length expressed in (number of) bytes.
    @param nWidth, nHeight, nBitsPerComponent are used for kFmtRawData.
    @return true if loaded correctly.
    */
    bool initWithImageData(void * pData, 
                           int nDataLen, 
                           EImageFormat eFmt = kFmtUnKnown,
                           int nWidth = 0,
                           int nHeight = 0,
                           int nBitsPerComponent = 8);
    /** Initializes a texture from a UIImage object */
    bool initWithImage(CCImage * uiImage);

    unsigned char *   getData()               { return m_pData; }
    int               getDataLen()            { return m_nWidth * m_nHeight; }


    bool hasAlpha()                     { return m_bHasAlpha;   }
    bool isPremultipliedAlpha()         { return m_bPreMulti;   }

    CC_SYNTHESIZE_READONLY(unsigned short,   m_nWidth,       Width);
    CC_SYNTHESIZE_READONLY(unsigned short,   m_nHeight,      Height);
    CC_SYNTHESIZE_READONLY(int,     m_nBitsPerComponent,   BitsPerComponent);
    static EImageFormat getType(std::string& filename);

protected:
//    bool _initWithJpgData(void *pData, int nDatalen);
    bool _initWithPngData(void *pData, int nDatalen);
    bool _initWithTiffData(void *pData, int nDataLen);
    bool _initWithWebpData(void *pData, int nDataLen);
    // @warning kFmtRawData only support RGBA8888
    bool _initWithRawData(void *pData, int nDatalen, int nWidth, int nHeight, int nBitsPerComponent, bool bPreMulti);

    bool _saveImageToPNG(const char *pszFilePath, bool bIsToRGB = true);
    bool _saveImageToJPG(const char *pszFilePath);

    unsigned char *m_pData;
    bool m_bHasAlpha;
    bool m_bPreMulti;

    void initWithData();

private:
    bool initPremultipliedATextureWithImage(CCImage * image, unsigned int pixelsWide, unsigned int pixelsHigh);
    // noncopyable
    CCImage(const CCImage&    rImg);
    CCImage & operator=(const CCImage&);
};

// end of platform group
/// @}

NS_NODE_END

#endif    // __CC_IMAGE_H__
