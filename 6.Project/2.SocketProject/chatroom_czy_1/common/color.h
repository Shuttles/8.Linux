/*************************************************************************
	> File Name: color.h
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 16时09分24秒
 ************************************************************************/

#ifndef _COLOR_H
#define _COLOR_H

#define COLOR(a, b) "\033[" #b "m" a "\033[0m"
#define COLOR_HL(a, b) "\033[1;" #b "m" a "\033[0m"


#define BLACK(a) COLOR(a, 30)
#define RED(a) COLOR(a, 31)
#define GREEN(a) COLOR(a, 32)
#define YELLOW(a) COLOR(a, 33)
#define BLUE(a) COLOR(a, 34)
#define PURPLE(a) COLOR(a, 35)
#define CYAN(a) COLOR(a, 36)

#define BLACK_HL(a) COLOR_HL(a, 30)
#define RED_HL(a) COLOR_HL(a, 31)
#define GREEN_HL(a) COLOR_HL(a, 32)
#define YELLOW_HL(a) COLOR_HL(a, 33)
#define BLUE_HL(a) COLOR_HL(a, 34)
#define PURPLE_HL(a) COLOR_HL(a, 35)
#define CYAN_HL(a) COLOR_HL(a, 36)

#endif
