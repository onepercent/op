/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008      Apple Inc. All Rights Reserved.

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



/*
* Support for RGBA_4_4_4_4 and RGBA_5_5_5_1 was copied from:
* https://devforums.apple.com/message/37855#37855 by a1studmuffin
*/

#include "CCTexture2D.h"
#include "CCImage.h"

NS_NODE_BEGIN

TextImgParam::TextImgParam() : 
    internalformat(GL_RGBA),
    width(1),
    height(1),
    format(GL_RGBA),
    type(GL_UNSIGNED_BYTE),
    mData(0),
    mDelete(false) {
}
TextImgParam::~TextImgParam() {
    if(mDelete) {
        delete[] mData;
    }
}

//CLASS IMPLEMENTATIONS:

// If the image has alpha, you can create RGBA8 (32-bit) or RGBA4 (16-bit) or RGB5A1 (16-bit)
// Default is: RGBA8888 (32-bit textures)
static CCTexture2DPixelFormat g_defaultAlphaPixelFormat = kCCTexture2DPixelFormat_Default;

// By default PVR images are treated as if they don't have the alpha channel premultiplied
static bool PVRHaveAlphaPremultiplied_ = false;

CCTexture2D::CCTexture2D()
: m_bPVRHaveAlphaPremultiplied(true)
, m_uPixelsWide(0)
, m_uPixelsHigh(0)
, m_bHasPremultipliedAlpha(false)
, m_bHasMipmaps(false)
, mInternal(true)
{
}

CCTexture2D::~CCTexture2D()
{
}

CCTexture2DPixelFormat CCTexture2D::getPixelFormat()
{
    return m_ePixelFormat;
}

unsigned int CCTexture2D::getPixelsWide()
{
    return m_uPixelsWide;
}

unsigned int CCTexture2D::getPixelsHigh()
{
    return m_uPixelsHigh;
}

bool CCTexture2D::hasPremultipliedAlpha()
{
    return m_bHasPremultipliedAlpha;
}

bool CCTexture2D::initWithData(const void *data, CCTexture2DPixelFormat pixelFormat, unsigned int pixelsWide, unsigned int pixelsHigh)
{
    if(mInternal) {
    }
    
    
    // Specify OpenGL texture image
    switch(pixelFormat)
    {
    case kCCTexture2DPixelFormat_RGBA8888:
            mParam.internalformat = GL_RGBA;
            mParam.format = GL_RGBA;
            mParam.type = GL_UNSIGNED_BYTE;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        break;
    case kCCTexture2DPixelFormat_RGB888:
            mParam.internalformat = GL_RGB;
            mParam.format = GL_RGB;
            mParam.type = GL_UNSIGNED_BYTE;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        break;
    case kCCTexture2DPixelFormat_RGBA4444:
            mParam.internalformat = GL_RGBA;
            mParam.format = GL_RGBA;
            mParam.type = GL_UNSIGNED_SHORT_4_4_4_4;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
        break;
    case kCCTexture2DPixelFormat_RGB5A1:
            mParam.internalformat = GL_RGBA;
            mParam.format = GL_RGBA;
            mParam.type = GL_UNSIGNED_SHORT_5_5_5_1;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
        break;
    case kCCTexture2DPixelFormat_RGB565:
            mParam.internalformat = GL_RGB;
            mParam.format = GL_RGB;
            mParam.type = GL_UNSIGNED_SHORT_5_6_5;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
        break;
    case kCCTexture2DPixelFormat_AI88:
            LOGE("conflit with gl_3.h");
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
        break;
    case kCCTexture2DPixelFormat_A8:
            mParam.internalformat = GL_ALPHA;
            mParam.format = GL_ALPHA;
            mParam.type = GL_UNSIGNED_BYTE;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
        break;
    case kCCTexture2DPixelFormat_I8:
            LOGE("conflit with gl_3.h");
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
        break;
    default:
        LOGI("initWithData %d %s", 0, "NSInternalInconsistencyException");
    }

    mParam.width = pixelsWide;
    mParam.height = pixelsHigh;

    m_uPixelsWide = pixelsWide;
    m_uPixelsHigh = pixelsHigh;
    m_ePixelFormat = pixelFormat;

    m_bHasPremultipliedAlpha = false;
    m_bHasMipmaps = false;

    return true;
}

bool CCTexture2D::initWithImage(CCImage *uiImage)
{
    if (uiImage == NULL)
    {
        LOGI("cocos2d: CCTexture2D. Can't create Texture. UIImage is nil");
        return false;
    }
    
    unsigned int imageWidth = uiImage->getWidth();
    unsigned int imageHeight = uiImage->getHeight();

    // always load premultiplied images
    return initPremultipliedATextureWithImage(uiImage, imageWidth, imageHeight);
}

bool CCTexture2D::initPremultipliedATextureWithImage(CCImage *image, unsigned int width, unsigned int height)
{
    unsigned char*            tempData = image->getData();
    unsigned int*             inPixel32  = NULL;
    unsigned char*            inPixel8 = NULL;
    unsigned short*           outPixel16 = NULL;
    bool                      hasAlpha = image->hasAlpha();
    CCTexture2DPixelFormat    pixelFormat;
    size_t                    bpp = image->getBitsPerComponent();

    // compute pixel format
    if (hasAlpha)
    {
    	pixelFormat = g_defaultAlphaPixelFormat;
    }
    else
    {
        if (bpp >= 8)
        {
            pixelFormat = kCCTexture2DPixelFormat_RGB888;
        }
        else
        {
            pixelFormat = kCCTexture2DPixelFormat_RGB565;
        }

    }

    // Repack the pixel data into the right format
    unsigned int length = width * height;

    if (pixelFormat == kCCTexture2DPixelFormat_RGB565)
    {
        if (hasAlpha)
        {
            // Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGGBBBBB"

            tempData = new unsigned char[width * height * 2];
            outPixel16 = (unsigned short*)tempData;
            inPixel32 = (unsigned int*)image->getData();

            for(unsigned int i = 0; i < length; ++i, ++inPixel32)
            {
                *outPixel16++ =
                ((((*inPixel32 >>  0) & 0xFF) >> 3) << 11) |  // R
                ((((*inPixel32 >>  8) & 0xFF) >> 2) << 5)  |  // G
                ((((*inPixel32 >> 16) & 0xFF) >> 3) << 0);    // B
            }
        }
        else
        {
            // Convert "RRRRRRRRRGGGGGGGGBBBBBBBB" to "RRRRRGGGGGGBBBBB"

            tempData = new unsigned char[width * height * 2];
            outPixel16 = (unsigned short*)tempData;
            inPixel8 = (unsigned char*)image->getData();

            for(unsigned int i = 0; i < length; ++i)
            {
                *outPixel16++ =
                (((*inPixel8++ & 0xFF) >> 3) << 11) |  // R
                (((*inPixel8++ & 0xFF) >> 2) << 5)  |  // G
                (((*inPixel8++ & 0xFF) >> 3) << 0);    // B
            }
        }
    }
    else if (pixelFormat == kCCTexture2DPixelFormat_RGBA4444)
    {
        // Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRGGGGBBBBAAAA"

        inPixel32 = (unsigned int*)image->getData();
        tempData = new unsigned char[width * height * 2];
        outPixel16 = (unsigned short*)tempData;

        for(unsigned int i = 0; i < length; ++i, ++inPixel32)
        {
            *outPixel16++ =
            ((((*inPixel32 >> 0) & 0xFF) >> 4) << 12) | // R
            ((((*inPixel32 >> 8) & 0xFF) >> 4) <<  8) | // G
            ((((*inPixel32 >> 16) & 0xFF) >> 4) << 4) | // B
            ((((*inPixel32 >> 24) & 0xFF) >> 4) << 0);  // A
        }
    }
    else if (pixelFormat == kCCTexture2DPixelFormat_RGB5A1)
    {
        // Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGBBBBBA"
        inPixel32 = (unsigned int*)image->getData();
        tempData = new unsigned char[width * height * 2];
        outPixel16 = (unsigned short*)tempData;

        for(unsigned int i = 0; i < length; ++i, ++inPixel32)
        {
            *outPixel16++ =
            ((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) | // R
            ((((*inPixel32 >> 8) & 0xFF) >> 3) <<  6) | // G
            ((((*inPixel32 >> 16) & 0xFF) >> 3) << 1) | // B
            ((((*inPixel32 >> 24) & 0xFF) >> 7) << 0);  // A
        }
    }
    else if (pixelFormat == kCCTexture2DPixelFormat_A8)
    {
        // Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "AAAAAAAA"
        inPixel32 = (unsigned int*)image->getData();
        tempData = new unsigned char[width * height];
        unsigned char *outPixel8 = tempData;

        for(unsigned int i = 0; i < length; ++i, ++inPixel32)
        {
            *outPixel8++ = (*inPixel32 >> 24) & 0xFF;  // A
        }
    }

    if (hasAlpha && pixelFormat == kCCTexture2DPixelFormat_RGB888)
    {
        // Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRRRRGGGGGGGGBBBBBBBB"
        inPixel32 = (unsigned int*)image->getData();
        tempData = new unsigned char[width * height * 3];
        unsigned char *outPixel8 = tempData;

        for(unsigned int i = 0; i < length; ++i, ++inPixel32)
        {
            *outPixel8++ = (*inPixel32 >> 0) & 0xFF; // R
            *outPixel8++ = (*inPixel32 >> 8) & 0xFF; // G
            *outPixel8++ = (*inPixel32 >> 16) & 0xFF; // B
        }
    }

    initWithData(tempData, pixelFormat, width, height);

    mParam.mData = tempData;
    mParam.mDelete = tempData != image->getData();

//    if (tempData != image->getData())
//    {
//        delete [] tempData;
//    }

    m_bHasPremultipliedAlpha = image->isPremultipliedAlpha();
    return true;
}

void CCTexture2D::PVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied)
{
    PVRHaveAlphaPremultiplied_ = haveAlphaPremultiplied;
}

    
//
// Use to apply MIN/MAG filter
//
// implementation CCTexture2D (GLFilter)

void CCTexture2D::setDefaultAlphaPixelFormat(CCTexture2DPixelFormat format)
{
    g_defaultAlphaPixelFormat = format;
}

CCTexture2DPixelFormat CCTexture2D::defaultAlphaPixelFormat()
{
    return g_defaultAlphaPixelFormat;
}

unsigned int CCTexture2D::bitsPerPixelForFormat(CCTexture2DPixelFormat format)
{
	unsigned int ret=0;

	switch (format) {
		case kCCTexture2DPixelFormat_RGBA8888:
			ret = 32;
			break;
		case kCCTexture2DPixelFormat_RGB888:
			// It is 32 and not 24, since its internal representation uses 32 bits.
			ret = 32;
			break;
		case kCCTexture2DPixelFormat_RGB565:
			ret = 16;
			break;
		case kCCTexture2DPixelFormat_RGBA4444:
			ret = 16;
			break;
		case kCCTexture2DPixelFormat_RGB5A1:
			ret = 16;
			break;
		case kCCTexture2DPixelFormat_AI88:
			ret = 16;
			break;
		case kCCTexture2DPixelFormat_A8:
			ret = 8;
			break;
		case kCCTexture2DPixelFormat_I8:
			ret = 8;
			break;
		case kCCTexture2DPixelFormat_PVRTC4:
			ret = 4;
			break;
		case kCCTexture2DPixelFormat_PVRTC2:
			ret = 2;
			break;
		default:
			ret = -1;
			LOGI("bitsPerPixelForFormat: %ld, cannot give useful result", (long)format);
			break;
	}
	return ret;
}

unsigned int CCTexture2D::bitsPerPixelForFormat()
{
	return this->bitsPerPixelForFormat(m_ePixelFormat);
}

NS_NODE_END
