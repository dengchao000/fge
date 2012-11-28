/** 
*
** Author:	邓超
** QQ:		23427470
** Mail:    sen.night@hotmail.com
** Time:	2007-11-21
*
** Fly Game 2
** Copyright (C) 2005-2007, Fantasy Games
** Kernel functions
** 
** Description: 随机数
*
*/

#pragma once

//初始化随机种
void	Random_Seed(int seed);
//得到整数随机数
int		Random_Int(int min, int max);
//得到浮点随机数
float	Random_Float(float min, float max);
