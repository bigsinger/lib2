libwebp-1.0.3-windows-x86

编译好的下载：https://storage.googleapis.com/downloads.webmproject.org/releases/webp/index.html

源码下载：https://github.com/webmproject/libwebp/tree/1.0.3

编译 lib 时，将 Makefile.vc 替换原有的文件，如果不同的版本，可以参考修改：
LIBWEBP_OBJS = $(LIBWEBPDECODER_OBJS) $(ENC_OBJS) $(DSP_ENC_OBJS) \
               $(UTILS_ENC_OBJS) $(DLL_OBJS) $(IMAGEIO_DEC_OBJS)

修改为：

LIBWEBP_OBJS = $(LIBWEBPDECODER_OBJS) $(ENC_OBJS) $(DSP_ENC_OBJS) \
               $(UTILS_ENC_OBJS) $(DLL_OBJS) $(IMAGEIO_ENC_OBJS) $(IMAGEIO_DEC_OBJS) $(IMAGEIO_UTIL_OBJS)