#define HAVE_WINCODEC_H

#include "dwebp.h"
#include "./decode.h"
#include "./imageio/image_enc.h"
#include "./imageio/webpdec.h"
#include <locale>
#include <codecvt>
#include <string>


// webpÍ¼Æ¬×ª»»ÎªÍ¼Æ¬
// ²Î¿¼libwebpÔ´Âë£ºexamples/dwebp.c https://github.com/webmproject/libwebp/blob/1.0.3/examples/dwebp.c
#pragma comment(lib,"libwebp.lib")
#pragma comment(lib,"libwebpdemux.lib")
bool Webp2Image(const char* in_file, const char* out_file, const char* pixel_format/* = nullptr*/) {
	bool success = false;

    WebPDecoderConfig config;
    WebPDecBuffer* const output_buffer = &config.output;
    WebPBitstreamFeatures* const bitstream = &config.input;
    WebPOutputFileFormat format = PNG;
    const uint8_t* data = NULL;

    int incremental = 0;

    VP8StatusCode status = VP8_STATUS_OK;
    size_t data_size = 0;


    if (!WebPInitDecoderConfig(&config)) {
        return success;
    }

    if (pixel_format) {
        const char* const fmt = pixel_format;
        if (!strcmp(fmt, "RGB"))  format = RGB;
        else if (!strcmp(fmt, "RGBA")) format = RGBA;
        else if (!strcmp(fmt, "BGR"))  format = BGR;
        else if (!strcmp(fmt, "BGRA")) format = BGRA;
        else if (!strcmp(fmt, "ARGB")) format = ARGB;
        else if (!strcmp(fmt, "RGBA_4444")) format = RGBA_4444;
        else if (!strcmp(fmt, "RGB_565")) format = RGB_565;
        else if (!strcmp(fmt, "rgbA")) format = rgbA;
        else if (!strcmp(fmt, "bgrA")) format = bgrA;
        else if (!strcmp(fmt, "Argb")) format = Argb;
        else if (!strcmp(fmt, "rgbA_4444")) format = rgbA_4444;
        else if (!strcmp(fmt, "YUV"))  format = YUV;
        else if (!strcmp(fmt, "YUVA")) format = YUVA;
    }
    
    if (!LoadWebP(in_file, &data, &data_size, bitstream)) {
        goto Exit;
    }

    switch (format) {
    case PNG:
#ifdef HAVE_WINCODEC_H
        output_buffer->colorspace = bitstream->has_alpha ? MODE_BGRA : MODE_BGR;
#else
        output_buffer->colorspace = bitstream->has_alpha ? MODE_RGBA : MODE_RGB;
#endif
        break;
    case PAM:
        output_buffer->colorspace = MODE_RGBA;
        break;
    case PPM:
        output_buffer->colorspace = MODE_RGB;  // drops alpha for PPM
        break;
    case BMP:
        output_buffer->colorspace = bitstream->has_alpha ? MODE_BGRA : MODE_BGR;
        break;
    case TIFF:
        output_buffer->colorspace = bitstream->has_alpha ? MODE_RGBA : MODE_RGB;
        break;
    case PGM:
    case RAW_YUV:
        output_buffer->colorspace = bitstream->has_alpha ? MODE_YUVA : MODE_YUV;
        break;
    case ALPHA_PLANE_ONLY:
        output_buffer->colorspace = MODE_YUVA;
        break;
        // forced modes:
    case RGB: output_buffer->colorspace = MODE_RGB; break;
    case RGBA: output_buffer->colorspace = MODE_RGBA; break;
    case BGR: output_buffer->colorspace = MODE_BGR; break;
    case BGRA: output_buffer->colorspace = MODE_BGRA; break;
    case ARGB: output_buffer->colorspace = MODE_ARGB; break;
    case RGBA_4444: output_buffer->colorspace = MODE_RGBA_4444; break;
    case RGB_565: output_buffer->colorspace = MODE_RGB_565; break;
    case rgbA: output_buffer->colorspace = MODE_rgbA; break;
    case bgrA: output_buffer->colorspace = MODE_bgrA; break;
    case Argb: output_buffer->colorspace = MODE_Argb; break;
    case rgbA_4444: output_buffer->colorspace = MODE_rgbA_4444; break;
    case YUV: output_buffer->colorspace = MODE_YUV; break;
    case YUVA: output_buffer->colorspace = MODE_YUVA; break;
    default: goto Exit;
    }

    if (incremental) {
        status = DecodeWebPIncremental(data, data_size, &config);
    } else {
        status = DecodeWebP(data, data_size, &config);
    }

    if (status != VP8_STATUS_OK) {
        PrintWebPError(in_file, status);
        goto Exit;
    }

    if (out_file != NULL) {
       success = WebPSaveImage(output_buffer, format, out_file);
    }

Exit:
    WebPFreeDecBuffer(output_buffer);
    WebPFree((void*)data);

	return success;
}