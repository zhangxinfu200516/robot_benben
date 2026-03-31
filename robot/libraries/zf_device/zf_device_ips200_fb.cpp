#include "zf_device_ips200_fb.h"
#include "zf_common_font.h"
#include "zf_common_function.h"

static uint16 ips200_pencolor = IPS200_DEFAULT_PENCOLOR;
static uint16 ips200_bgcolor = IPS200_DEFAULT_BGCOLOR;

static int ips200_width;                       
static int ips200_height;                          
unsigned short *screen_base = NULL;         //映射后的显存基地址

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS200 清屏函数
// 参数说明     void
// 返回参数     void
// 使用示例     ips200_clear();
// 备注信息     将屏幕清空成背景颜色
//-------------------------------------------------------------------------------------------------------------------
void ips200_clear(void)
{
    ips200_full(IPS200_DEFAULT_BGCOLOR);
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS200 屏幕填充函数
// 参数说明     color           颜色格式 RGB565 或者可以使用 zf_common_font.h 内 rgb565_color_enum 枚举值或者自行写入
// 返回参数     void
// 使用示例     ips200_full(RGB565_BLACK);
// 备注信息     将屏幕填充成指定颜色
//-------------------------------------------------------------------------------------------------------------------
void ips200_full(const uint16 color)
{
    uint16 i,j;
    for(i=0;i<240; i++)
    {
        for(j=0; j<320; j++)
        {
            ips200_draw_point(i, j, color);
        }
    }
}

void ips200_draw_point(uint16_t x, uint16_t y, const uint16_t color)
{
    screen_base[y * ips200_width + x] = color;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS200 画线
// 参数说明     x_start         坐标x方向的起点 [0, ips200_x_max-1]
// 参数说明     y_start         坐标y方向的起点 [0, ips200_y_max-1]
// 参数说明     x_end           坐标x方向的终点 [0, ips200_x_max-1]
// 参数说明     y_end           坐标y方向的终点 [0, ips200_y_max-1]
// 参数说明     color           颜色格式 RGB565 或者可以使用 zf_common_font.h 内 rgb565_color_enum 枚举值或者自行写入
// 返回参数     void
// 使用示例     ips200_draw_line(0, 0, 10, 10, RGB565_RED);     // 坐标 0,0 到 10,10 画一条红色的线
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void ips200_draw_line (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color)
{
    int16 x_dir = (x_start < x_end ? 1 : -1);
    int16 y_dir = (y_start < y_end ? 1 : -1);
    float temp_rate = 0;
    float temp_b = 0;

    do
    {
        if(x_start != x_end)
        {
            temp_rate = (float)(y_start - y_end) / (float)(x_start - x_end);
            temp_b = (float)y_start - (float)x_start * temp_rate;
        }
        else
        {
            while(y_start != y_end)
            {
                ips200_draw_point(x_start, y_start, color);
                y_start += y_dir;
            }
            ips200_draw_point(x_start, y_start, color);
            break;
        }
        if(func_abs(y_start - y_end) > func_abs(x_start - x_end))
        {
            while(y_start != y_end)
            {
                ips200_draw_point(x_start, y_start, color);
                y_start += y_dir;
                x_start = (int16)(((float)y_start - temp_b) / temp_rate);
            }
            ips200_draw_point(x_start, y_start, color);
        }
        else
        {
            while(x_start != x_end)
            {
                ips200_draw_point(x_start, y_start, color);
                x_start += x_dir;
                y_start = (int16)((float)x_start * temp_rate + temp_b);
            }
            ips200_draw_point(x_start, y_start, color);
        }
    }while(0);
}

void ips200_show_char(uint16 x, uint16 y, const char dat)
{
    uint8 i = 0, j = 0;
    for(i = 0; 8 > i; i ++)
    {
        uint8 temp_top = ascii_font_8x16[dat - 32][i];
        uint8 temp_bottom = ascii_font_8x16[dat - 32][i + 8];
        for(j = 0; 8 > j; j ++)
        {
            if(temp_top & 0x01)
            {
                ips200_draw_point(x + i, y + j, ips200_pencolor);
            } 
            else 
            { 
                ips200_draw_point(x + i, y + j, ips200_bgcolor);
            }
            temp_top >>= 1;
        }

        for(j = 0; 8 > j; j ++)
        {
            if(temp_bottom & 0x01)
            {
                ips200_draw_point(x + i, y + j + 8, ips200_pencolor);
            }
            else
            {
                ips200_draw_point(x + i, y + j + 8, ips200_bgcolor);
            }
            temp_bottom >>= 1;
        }
    }
}

void ips200_show_string(uint16 x, uint16 y, const char dat[])
{
    uint16 j = 0;
    
    while('\0' != dat[j])
    {
        ips200_show_char(x + 8 * j,  y, dat[j]);
        j ++;
    }
}

void ips200_show_int(uint16 x, uint16 y, const int32 dat, uint8 num)
{
    int32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];

    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num+1);

    // 用来计算余数显示 123 显示 2 位则应该显示 23
    if(10 > num)
    {
        for(; 0 < num; num --)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_int_to_str(data_buffer, dat_temp);
    ips200_show_string(x, y, (const char *)&data_buffer);
}

void ips200_show_uint(uint16 x, uint16 y, const uint32 dat, uint8 num)
{

    uint32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];
    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num);

    // 用来计算余数显示 123 显示 2 位则应该显示 23
    if(10 > num)
    {
        for(; 0 < num; num --)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_uint_to_str(data_buffer, dat_temp);
    ips200_show_string(x, y, (const char *)&data_buffer);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS200 显示浮点数(去除整数部分无效的0)
// 参数说明     x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// 参数说明     y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// 参数说明     dat             需要显示的变量 数据类型 double
// 参数说明     num             整数位显示长度   最高8位
// 参数说明     pointnum        小数位显示长度   最高6位
// 返回参数     void
// 使用示例     ips200_show_float(0, 0, x, 2, 3);               // 显示浮点数   整数显示2位   小数显示三位
// 备注信息     特别注意当发现小数部分显示的值与你写入的值不一样的时候，
//              可能是由于浮点数精度丢失问题导致的，这并不是显示函数的问题，
//              有关问题的详情，请自行百度学习   浮点数精度丢失问题。
//              负数会显示一个 ‘-’号
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_float (uint16 x, uint16 y, const double dat, uint8 num, uint8 pointnum)
{
    // // 如果程序在输出了断言信息 并且提示出错位置在这里
    // // 那么一般是屏幕显示的时候超过屏幕分辨率范围了
    // zf_assert(x < ips200_x_max);
    // zf_assert(y < ips200_y_max);
    // zf_assert(0 < num);
    // zf_assert(8 >= num);
    // zf_assert(0 < pointnum);
    // zf_assert(6 >= pointnum);

    double dat_temp = dat;
    double offset = 1.0;
    char data_buffer[17];
    memset(data_buffer, 0, 17);
    memset(data_buffer, ' ', num+pointnum+2);

    // 用来计算余数显示 123 显示 2 位则应该显示 23
    for(; 0 < num; num --)
    {
        offset *= 10;
    }
    dat_temp = dat_temp - ((int)dat_temp / (int)offset) * offset;
    func_double_to_str(data_buffer, dat_temp, pointnum);
    ips200_show_string(x, y, data_buffer);
}


void ips200_show_gray_image(uint16 x, uint16 y, const uint8 *image, 
uint16 width, uint16 height)
{
    uint32 x_start = 0, y_start = 0;
    uint16 color = 0;

    for(y_start = y; y_start < (y + height); y_start++) 
    {
        for(x_start = x; x_start < (x + width); x_start++)
        {
            uint8_t grayValue = image[(x_start - x) + (y_start- y) * width];

            uint16_t r = (grayValue >> 3) & 0b11111;
            uint16_t g = (grayValue >> 2) & 0b111111;
            uint16_t b = (grayValue >> 3) & 0b11111;
            color = (r << 11) | (g << 5) | (b << 0);
            
            ips200_draw_point(x_start, y_start,  color);
        }
    }    
}


void ips200_init(const char *path)
{
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    unsigned int screen_size;
    int fd;

    if (0 > (fd = open(path, O_RDWR))) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    // if (0 > (fd = open(path, O_RDWR))) {
    //     perror("open error");
    //     exit(EXIT_FAILURE);
    // }

    /* 获取参数信息 */
    ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);

    screen_size = fb_fix.line_length * fb_var.yres;
    ips200_width = fb_var.xres;
    ips200_height = fb_var.yres;

    /* 将显示缓冲区映射到进程地址空间 */
    screen_base = (unsigned short *)mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == (void *)screen_base) {
        perror("mmap error");
        close(fd);
        exit(EXIT_FAILURE);
    }


    // 刷屏全黑
    for(uint16 i=0;i<240;i++)
    {
        for(uint16 j=0;j<320;j++)
        {
            ips200_draw_point(i, j, IPS200_DEFAULT_BGCOLOR);
        }
    }

}

void ips200_show_char_scaled(uint16_t x, uint16_t y, const char dat, uint8_t scale) {
    for(uint8_t i = 0; i < 8; i++) {
        uint8_t temp_top = ascii_font_8x16[dat - 32][i];
        uint8_t temp_bottom = ascii_font_8x16[dat - 32][i + 8];
        
        for(uint8_t j = 0; j < 16; j++) {
            uint8_t bit_mask = (j < 8) ? (temp_top >> j) : (temp_bottom >> (j-8));
            
            if(bit_mask & 0x01) {
                // 绘制放大后的像素块
                for(uint8_t s_x = 0; s_x < scale; s_x++) {
                    for(uint8_t s_y = 0; s_y < scale; s_y++) {
                        ips200_draw_point(x + i*scale + s_x, 
                                         y + j*scale + s_y, 
                                         ips200_pencolor);
                    }
                }
            } else {
                // 背景色处理（同上）
            }
        }
    }
}

// 配套的字符串显示函数
void ips200_show_string_scaled(uint16_t x, uint16_t y, const char dat[], uint8_t scale) {
    uint16_t j = 0;
    while(dat[j] != '\0') {
        ips200_show_char_scaled(x + 8 * scale * j, y, dat[j], scale);
        j++;
    }
}

// 显示单个汉字
void ips200_show_chinese_char(uint16_t x, uint16_t y, const uint8_t gb2312[2])
{
    // 在自定义字库中查找汉字
    for (int i = 0; i < CHINESE_FONT_COUNT; i++) {
        if (custom_font[i].index[0] == gb2312[0] && 
            custom_font[i].index[1] == gb2312[1]) {
            
            // 绘制汉字
            for (int col = 0; col < 16; col++) { // 16列
                uint8_t top = custom_font[i].bitmap[col*2];     // 上半部
                uint8_t bottom = custom_font[i].bitmap[col*2+1]; // 下半部
                
                for (int row = 0; row < 8; row++) { // 上半部8行
                    if (top & (0x80 >> row)) {
                        ips200_draw_point(x + col, y + row, ips200_pencolor);
                    } else {
                        ips200_draw_point(x + col, y + row, ips200_bgcolor);
                    }
                }
                
                for (int row = 0; row < 8; row++) { // 下半部8行
                    if (bottom & (0x80 >> row)) {
                        ips200_draw_point(x + col, y + row + 8, ips200_pencolor);
                    } else {
                        ips200_draw_point(x + col, y + row + 8, ips200_bgcolor);
                    }
                }
            }
            return;
        }
    }
    
    // 未找到汉字，显示问号或留空
    ips200_show_char(x, y, '?');
    ips200_show_char(x + 8, y, '?');
}

// 显示整个字符串
void ips200_show_chinese_string(uint16_t x, uint16_t y, const char *str)
{
    uint16_t x_offset = 0;
    
    while (*str) {
        // 检查是否为GB2312汉字（首字节在0xA1-0xFE）
        if ((uint8_t)*str >= 0xA1 && (uint8_t)*str <= 0xFE) {
            // 确保有下一个字节
            if (*(str+1) == '\0') break;
            
            uint8_t gb2312[2] = {(uint8_t)*str, (uint8_t)*(str+1)};
            ips200_show_chinese_char(x + x_offset, y, gb2312);
            x_offset += 16; // 每个汉字宽度16像素
            str += 2; // 跳过两个字节
        } else {
            // ASCII字符
            ips200_show_char(x + x_offset, y, *str);
            x_offset += 8; // ASCII字符宽度8像素
            str++;
        }
        
        // 换行处理（如果需要）
        if (x_offset + 16 > ips200_width) {
            x_offset = 0;
            y += 16; // 移动到下一行
        }
    }
}
// 显示放大汉字 (16x16点阵) - 横向取模版本
void ips200_show_chinese_char_scaled(uint16_t x, uint16_t y, 
                                     const uint8_t gb2312[2], 
                                     uint8_t scale) {
    const ChineseFont* font = NULL;
    
    // 在字库中查找汉字
    for (uint16_t i = 0; i < CHINESE_FONT_COUNT; i++) {
        if (chinese_font_16x16[i].index[0] == gb2312[0] && 
            chinese_font_16x16[i].index[1] == gb2312[1]) {
            font = &chinese_font_16x16[i];
            break;
        }
    }
    
    if (!font) {
        // 未找到汉字，显示两个问号代替
        ips200_show_char_scaled(x, y, '?', scale);
        ips200_show_char_scaled(x + 8 * scale, y, '?', scale);
        return;
    }
    
    const uint8_t *font_data = font->bitmap;
    
    // 遍历每一行 (0-15)
    for (uint8_t row = 0; row < 16; row++) {
        // 获取当前行的两个字节数据（前8位 + 后8位）
        uint8_t first_byte = font_data[2 * row];     // 该行前8列数据
        uint8_t second_byte = font_data[2 * row + 1]; // 该行后8列数据
        
        // 遍历当前行的每一列 (0-15)
        for (uint8_t col = 0; col < 16; col++) {
            uint8_t bit_value;
            
            // 根据列位置选择字节
            if (col < 8) {
                // 前8列：从first_byte高位开始取（MSB first）
                bit_value = (first_byte >> (7 - col)) & 0x01;
            } else {
                // 后8列：从second_byte高位开始取（MSB first）
                bit_value = (second_byte >> (7 - (col - 8))) & 0x01;
            }
            
            // 绘制放大像素
            if (bit_value) {
                for (uint8_t s_x = 0; s_x < scale; s_x++) {
                    for (uint8_t s_y = 0; s_y < scale; s_y++) {
                        ips200_draw_point(x + col * scale + s_x, 
                                         y + row * scale + s_y, 
                                         ips200_pencolor);
                    }
                }
            } else if (ips200_bgcolor != 0xFFFF) { 
                // 0xFFFF表示透明背景，不绘制背景
                for (uint8_t s_x = 0; s_x < scale; s_x++) {
                    for (uint8_t s_y = 0; s_y < scale; s_y++) {
                        ips200_draw_point(x + col * scale + s_x, 
                                         y + row * scale + s_y, 
                                         ips200_bgcolor);
                    }
                }
            }
        }
    }
}

// 显示放大中文字符串（支持混合显示）
void ips200_show_chinese_string_scaled(uint16_t x, uint16_t y, 
                                      const char *str, 
                                      uint8_t scale) {
    uint16_t x_offset = 0;
    uint16_t y_offset = 0;
    const char *ptr = str;
    
    while (*ptr) {
        // 检查是否为GB2312汉字（首字节在0xA1-0xFE）
        if ((uint8_t)*ptr >= 0xA1 && (uint8_t)*ptr <= 0xFE) {
            // 确保有下一个字节
            if (*(ptr+1) == '\0') break;
            
            // 显示汉字
            uint8_t gb2312[2] = {(uint8_t)*ptr, (uint8_t)*(ptr+1)};
            ips200_show_chinese_char_scaled(x + x_offset, y + y_offset, 
                                           gb2312, scale);
            x_offset += 16 * scale; // 汉字宽度放大
            ptr += 2;
        } else {
            // 显示英文字符
            ips200_show_char_scaled(x + x_offset, y + y_offset, 
                                   *ptr, scale);
            x_offset += 8 * scale; // 英文字符宽度放大
            ptr++;
        }
        
        // 换行处理（考虑放大后的宽度）
        if (x_offset + 16 * scale > ips200_width) {
            x_offset = 0;
            y_offset += 16 * scale; // 行高放大
            
            // 检查是否超出屏幕底部
            if (y + y_offset + 16 * scale > ips200_height) {
                break;
            }
        }
    }
}