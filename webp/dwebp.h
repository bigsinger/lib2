#pragma once

// �ж��Ƿ���webpͼƬ
bool IsWebpImage(const char* filename);

// webpͼƬת��ΪpngͼƬ
bool Webp2Image(const char* in_file, const char* out_file, const char* pixel_format = nullptr);
