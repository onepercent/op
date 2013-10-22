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

#include "CCImage.h"
#include <png.h>
//#include <pngconf.h>
//#include <jpeglib.h>
//#include <decode.h>
//#include <types.h>
#include <ctype.h>

NS_NODE_BEGIN

// premultiply alpha, or the effect will wrong when want to use other pixel format in CCTexture2D,
// such as RGB888, RGB5A1
#define CC_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
    (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
    ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
    ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
    ((unsigned)(unsigned char)(va) << 24))

// on ios, we should use platform/ios/CCImage_ios.mm instead

typedef struct 
{
    unsigned char* data;
    int size;
    int offset;
}tImageSource;

static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

    if((int)(isource->offset + length) <= isource->size)
    {
        memcpy(data, isource->data+isource->offset, length);
        isource->offset += length;
    }
    else
    {
        png_error(png_ptr, "pngReaderCallback failed");
    }
}

//////////////////////////////////////////////////////////////////////////
// Implement CCImage
//////////////////////////////////////////////////////////////////////////

CCImage::CCImage()
: m_nWidth(0)
, m_nHeight(0)
, m_nBitsPerComponent(0)
, m_pData(0)
, m_bHasAlpha(false)
, m_bPreMulti(false)
{

}

CCImage::~CCImage()
{
    CC_SAFE_DELETE_ARRAY(m_pData);
}
CCImage::EImageFormat CCImage::getType(std::string& filename)
{
    CCImage::EImageFormat ret = CCImage::kFmtUnKnown;

    if ((std::string::npos != filename.find(".jpg")) || (std::string::npos != filename.find(".jpeg")))
    {
        ret = CCImage::kFmtJpg;
    }
    else if ((std::string::npos != filename.find(".png")) || (std::string::npos != filename.find(".PNG")))
    {
        ret = CCImage::kFmtPng;
    }
    else if ((std::string::npos != filename.find(".tiff")) || (std::string::npos != filename.find(".TIFF")))
    {
        ret = CCImage::kFmtTiff;
    }
    else if ((std::string::npos != filename.find(".webp")) || (std::string::npos != filename.find(".WEBP")))
    {
        ret = CCImage::kFmtWebp;
    }

    return ret;
}

bool CCImage::initWithImageData(void * pData,
                                int nDataLen,
                                EImageFormat eFmt/* = eSrcFmtPng*/,
                                int nWidth/* = 0*/,
                                int nHeight/* = 0*/,
                                int nBitsPerComponent/* = 8*/)
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! pData || nDataLen <= 0);

        if (kFmtPng == eFmt)
        {
            bRet = _initWithPngData(pData, nDataLen);
            break;
        }
//        else if (kFmtJpg == eFmt)
//        {
//            bRet = _initWithJpgData(pData, nDataLen);
//            break;
//        }
//        else if (kFmtTiff == eFmt)
//        {
//            bRet = _initWithTiffData(pData, nDataLen);
//            break;
//        }
        else if (kFmtWebp == eFmt)
        {
            bRet = _initWithWebpData(pData, nDataLen);
            break;
        }
        else if (kFmtRawData == eFmt)
        {
            bRet = _initWithRawData(pData, nDataLen, nWidth, nHeight, nBitsPerComponent, false);
            break;
        }
        else
        {
            // if it is a png file buffer.
            if (nDataLen > 8)
            {
                unsigned char* pHead = (unsigned char*)pData;
                if (   pHead[0] == 0x89
                    && pHead[1] == 0x50
                    && pHead[2] == 0x4E
                    && pHead[3] == 0x47
                    && pHead[4] == 0x0D
                    && pHead[5] == 0x0A
                    && pHead[6] == 0x1A
                    && pHead[7] == 0x0A)
                {
                    bRet = _initWithPngData(pData, nDataLen);
                    break;
                }
            }

//            // if it is a tiff file buffer.
//            if (nDataLen > 2)
//            {
//                unsigned char* pHead = (unsigned char*)pData;
//                if (  (pHead[0] == 0x49 && pHead[1] == 0x49)
//                    || (pHead[0] == 0x4d && pHead[1] == 0x4d)
//                    )
//                {
//                    bRet = _initWithTiffData(pData, nDataLen);
//                    break;
//                }
//            }

            // if it is a jpeg file buffer.
//            if (nDataLen > 2)
//            {
//                unsigned char* pHead = (unsigned char*)pData;
//                if (   pHead[0] == 0xff
//                    && pHead[1] == 0xd8)
//                {
//                    bRet = _initWithJpgData(pData, nDataLen);
//                    break;
//                }
//            }
        }
    } while (0);
    return bRet;
}

/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * We override the "error_exit" method so that control is returned to the
 * library's caller when a fatal error occurs, rather than calling exit()
 * as the standard error_exit method does.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

//struct my_error_mgr {
//  struct jpeg_error_mgr pub;	/* "public" fields */
//
//  jmp_buf setjmp_buffer;	/* for return to caller */
//};
//
//typedef struct my_error_mgr * my_error_ptr;
//
///*
// * Here's the routine that will replace the standard error_exit method:
// */
//METHODDEF(void)
//my_error_exit (j_common_ptr cinfo)
//{
//  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
//  my_error_ptr myerr = (my_error_ptr) cinfo->err;
//
//  /* Always display the message. */
//  /* We could postpone this until after returning, if we chose. */
//  (*cinfo->err->output_message) (cinfo);
//
//  /* Return control to the setjmp point */
//  longjmp(myerr->setjmp_buffer, 1);
//}
//
//bool CCImage::_initWithJpgData(void * data, int nSize)
//{
//    /* these are standard libjpeg structures for reading(decompression) */
//    struct jpeg_decompress_struct cinfo;
//    /* We use our private extension JPEG error handler.
//	 * Note that this struct must live as long as the main JPEG parameter
//	 * struct, to avoid dangling-pointer problems.
//	 */
//	struct my_error_mgr jerr;
//    /* libjpeg data structure for storing one row, that is, scanline of an image */
//    JSAMPROW row_pointer[1] = {0};
//    unsigned long location = 0;
//    unsigned int i = 0;
//
//    bool bRet = false;
//    do 
//    {
//        /* We set up the normal JPEG error routines, then override error_exit. */
//		cinfo.err = jpeg_std_error(&jerr.pub);
//		jerr.pub.error_exit = my_error_exit;
//		/* Establish the setjmp return context for my_error_exit to use. */
//		if (setjmp(jerr.setjmp_buffer)) {
//			/* If we get here, the JPEG code has signaled an error.
//			 * We need to clean up the JPEG object, close the input file, and return.
//			 */
//			LOGI("%d", bRet);
//			jpeg_destroy_decompress(&cinfo);
//			break;
//		}
//
//        /* setup decompression process and source, then read JPEG header */
//        jpeg_create_decompress( &cinfo );
//
//        jpeg_mem_src( &cinfo, (unsigned char *) data, nSize );
//
//        /* reading the image header which contains image information */
//#if (JPEG_LIB_VERSION >= 90)
//        // libjpeg 0.9 adds stricter types.
//        jpeg_read_header( &cinfo, TRUE );
//#else
//        jpeg_read_header( &cinfo, true );
//#endif
//
//        // we only support RGB or grayscale
//        if (cinfo.jpeg_color_space != JCS_RGB)
//        {
//            if (cinfo.jpeg_color_space == JCS_GRAYSCALE || cinfo.jpeg_color_space == JCS_YCbCr)
//            {
//                cinfo.out_color_space = JCS_RGB;
//            }
//        }
//        else
//        {
//            break;
//        }
//
//        /* Start decompression jpeg here */
//        jpeg_start_decompress( &cinfo );
//
//        /* init image info */
//        m_nWidth  = (short)(cinfo.output_width);
//        m_nHeight = (short)(cinfo.output_height);
//        m_bHasAlpha = false;
//        m_bPreMulti = false;
//        m_nBitsPerComponent = 8;
//        row_pointer[0] = new unsigned char[cinfo.output_width*cinfo.output_components];
//        CC_BREAK_IF(! row_pointer[0]);
//
//        m_pData = new unsigned char[cinfo.output_width*cinfo.output_height*cinfo.output_components];
//        CC_BREAK_IF(! m_pData);
//
//        /* now actually read the jpeg into the raw buffer */
//        /* read one scan line at a time */
//        while( cinfo.output_scanline < cinfo.output_height )
//        {
//            jpeg_read_scanlines( &cinfo, row_pointer, 1 );
//            for( i=0; i<cinfo.output_width*cinfo.output_components;i++) 
//            {
//                m_pData[location++] = row_pointer[0][i];
//            }
//        }
//
//		/* When read image file with broken data, jpeg_finish_decompress() may cause error.
//		 * Besides, jpeg_destroy_decompress() shall deallocate and release all memory associated
//		 * with the decompression object.
//		 * So it doesn't need to call jpeg_finish_decompress().
//		 */
//		//jpeg_finish_decompress( &cinfo );
//        jpeg_destroy_decompress( &cinfo );
//        /* wrap up decompression, destroy objects, free pointers and close open files */        
//        bRet = true;
//    } while (0);
//
//    CC_SAFE_DELETE_ARRAY(row_pointer[0]);
//    return bRet;
//}
//
bool CCImage::_initWithPngData(void * pData, int nDatalen)
{
// length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8
    bool bRet = false;
    png_byte        header[PNGSIGSIZE]   = {0}; 
    png_structp     png_ptr     =   0;
    png_infop       info_ptr    = 0;

    do 
    {
        // png header len is 8 bytes
        CC_BREAK_IF(nDatalen < PNGSIGSIZE);

        // check the data is png or not
        memcpy(header, pData, PNGSIGSIZE);
        CC_BREAK_IF(png_sig_cmp(header, 0, PNGSIGSIZE));

        // init png_struct
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        CC_BREAK_IF(! png_ptr);

        // init png_info
        info_ptr = png_create_info_struct(png_ptr);
        CC_BREAK_IF(!info_ptr);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA && CC_TARGET_PLATFORM != CC_PLATFORM_NACL)
        CC_BREAK_IF(setjmp(png_jmpbuf(png_ptr)));
#endif

        // set the read call back function
        tImageSource imageSource;
        imageSource.data    = (unsigned char*)pData;
        imageSource.size    = nDatalen;
        imageSource.offset  = 0;
        png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

        // read png header info
        
        // read png file info
        png_read_info(png_ptr, info_ptr);
        
        m_nWidth = png_get_image_width(png_ptr, info_ptr);
        m_nHeight = png_get_image_height(png_ptr, info_ptr);
        m_nBitsPerComponent = png_get_bit_depth(png_ptr, info_ptr);
        png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

        //LOGI("color type %u", color_type);
        
        // force palette images to be expanded to 24-bit RGB
        // it may include alpha channel
        if (color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(png_ptr);
        }
        // low-bit-depth grayscale images are to be expanded to 8 bits
        if (color_type == PNG_COLOR_TYPE_GRAY && m_nBitsPerComponent < 8)
        {
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        }
        // expand any tRNS chunk data into a full alpha channel
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(png_ptr);
        }  
        // reduce images with 16-bit samples to 8 bits
        if (m_nBitsPerComponent == 16)
        {
            png_set_strip_16(png_ptr);            
        } 
        // expand grayscale images to RGB
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        {
            png_set_gray_to_rgb(png_ptr);
        }

        // read png data
        // m_nBitsPerComponent will always be 8
        m_nBitsPerComponent = 8;
        png_uint_32 rowbytes;
        png_bytep* row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * m_nHeight );
        
        png_read_update_info(png_ptr, info_ptr);
        
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        
        m_pData = new unsigned char[rowbytes * m_nHeight];
        CC_BREAK_IF(!m_pData);
        
        for (unsigned short i = 0; i < m_nHeight; ++i)
        {
            row_pointers[i] = m_pData + i*rowbytes;
        }
        png_read_image(png_ptr, row_pointers);
        
        png_read_end(png_ptr, NULL);
        
        png_uint_32 channel = rowbytes/m_nWidth;
        if (channel == 4)
        {
            m_bHasAlpha = true;
            unsigned int *tmp = (unsigned int *)m_pData;
            for(unsigned short i = 0; i < m_nHeight; i++)
            {
                for(unsigned int j = 0; j < rowbytes; j += 4)
                {
                    *tmp++ = CC_RGB_PREMULTIPLY_ALPHA( row_pointers[i][j], row_pointers[i][j + 1], 
                                                      row_pointers[i][j + 2], row_pointers[i][j + 3] );
                }
            }
            
            m_bPreMulti = true;
        }

        CC_SAFE_FREE(row_pointers);

        bRet = true;
    } while (0);

    if (png_ptr)
    {
        png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
    }
    return bRet;
}

bool CCImage::_initWithWebpData(void *pData, int nDataLen)
{
	bool bRet = false;
//	do
//	{
//        WebPDecoderConfig config;
//        if (WebPInitDecoderConfig(&config) == 0) break;
//        if (WebPGetFeatures((uint8_t*)pData, nDataLen, &config.input) != VP8_STATUS_OK) break;
//        if (config.input.width == 0 || config.input.height == 0) break;
//
//        config.output.colorspace = MODE_RGBA;
//        m_nBitsPerComponent = 8;
//        m_nWidth    = config.input.width;
//        m_nHeight   = config.input.height;
//        m_bHasAlpha = true;
//
//        int bufferSize = m_nWidth * m_nHeight * 4;
//        m_pData = new unsigned char[bufferSize];
//
//        config.output.u.RGBA.rgba = (uint8_t*)m_pData;
//        config.output.u.RGBA.stride = m_nWidth * 4;
//        config.output.u.RGBA.size = bufferSize;
//        config.output.is_external_memory = 1;
//
//        if (WebPDecode((uint8_t*)pData, nDataLen, &config) != VP8_STATUS_OK)
//        {
//            delete []m_pData;
//            m_pData = NULL;
//            break;
//        }
//
//        bRet = true;
//	} while (0);
	return bRet;
}

bool CCImage::_initWithRawData(void * pData, int nDatalen, int nWidth, int nHeight, int nBitsPerComponent, bool bPreMulti)
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(0 == nWidth || 0 == nHeight);

        m_nBitsPerComponent = nBitsPerComponent;
        m_nHeight   = (short)nHeight;
        m_nWidth    = (short)nWidth;
        m_bHasAlpha = true;
        m_bPreMulti = bPreMulti;

        // only RGBA8888 supported
        int nBytesPerComponent = 4;
        int nSize = nHeight * nWidth * nBytesPerComponent;
        m_pData = new unsigned char[nSize];
        CC_BREAK_IF(! m_pData);
        memcpy(m_pData, pData, nSize);

        bRet = true;
    } while (0);

    return bRet;
}

NS_NODE_END

