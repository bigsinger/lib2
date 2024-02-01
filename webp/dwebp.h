#pragma once

// ÅĞ¶ÏÊÇ·ñÊÇwebpÍ¼Æ¬
bool IsWebpImage(const char* filename);

// webpÍ¼Æ¬×ª»»ÎªpngÍ¼Æ¬
bool Webp2Image(const char* in_file, const char* out_file, const char* pixel_format = nullptr);
